#! /usr/bin/python3

import sys
import os

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from mpl_toolkits.mplot3d import Axes3D
from sklearn.decomposition import PCA as sklearnPCA
from sklearn import tree
from sklearn.model_selection import train_test_split
from sklearn.model_selection import KFold
from sklearn.model_selection import RepeatedKFold
from sklearn.metrics import accuracy_score

import ppar

def create_folder(directory):
    try:
        if not os.path.exists(directory):
            os.makedirs(directory)
    except OSError:
        print ("error: could not create directory: " + directory)

if __name__ == "__main__":

    print("=== Decision Tree statistical learning ===")
    raw_data_filename = sys.argv[1]
    report_folder = sys.argv[2]
    std_num = int(sys.argv[3]) # outliers screening parameter
    norm = sys.argv[4]

    if not os.path.exists(raw_data_filename):
        sys.exit("error: " + raw_data_filename + " data file does not exist!")

    if not os.path.exists(report_folder):
        sys.exit("error: " + report_folder + " report folder does not exist!")

    print("Raw data input: " + raw_data_filename)
    print("Report folder: " + report_folder)
    print("Screen outliers outside " + str(std_num) + "-sigma standard deviations")

    normalized_folder = report_folder + "/normalized-values/"
    absolute_folder = report_folder + "/absolute-values/"

    create_folder(normalized_folder)
    create_folder(absolute_folder)

    # load raw data file
    data = pd.read_csv(raw_data_filename)
    # loop locations in benchmark source code
    loop_locations = data['loop-location']
    # prepare statistical learning labels 
    loop_icc_classifications = data['icc-parallel']
    # prepare statistical learning features 
    features = data.drop(['loop-location','icc-parallel'], axis=1)

    # remove outliers from the data
    if std_num != 0:
        filtered_idxs = {}
        for metric in ppar.metric_list:  
            d = features[metric]
            filtered_idxs[metric] = features[abs(d-d.mean()) < std_num*d.std()].index
    
        filtered_idx = features.index
        for metric in ppar.metric_list:  
            filtered_idx &= filtered_idxs[metric]

        loop_icc_classifications = loop_icc_classifications[filtered_idx]
        idxs_to_drop = features.index.drop(filtered_idx)
        features = features.drop(idxs_to_drop)

        loop_icc_classifications = loop_icc_classifications.reset_index(drop=True)
        features = features.reset_index(drop=True)

    # normalize the data
    if norm == "norm":
        for feature in ppar.metric_list:
            features[feature] = (features[feature] - features[feature].min())/(features[feature].max() - features[feature].min())

    # prepare data for different metric groups
    metrics_data = {}
    for metric_group in ppar.metric_groups:
        metrics_data[metric_group] = features[ppar.metric_groups[metric_group]]

    # prepare data for single metrics
    metric_data = {}
    for metric in ppar.metric_list:
        metric_data[metric] = features[metric]

    # prepare data for different metric groups
    metric_set_data = {}
    for metric_set in ppar.metric_sets:
        metric_set_data[metric_set] = features[ppar.metric_sets[metric_set]]

    i = 0
    for metric in ppar.metric_list:
        d = metric_data[metric]
        min_value = d.min()
        max_value = d.max()
        
        step = (max_value - min_value)/20
        above_x = []
        below_x = []
        for x in np.arange(min_value, max_value, step):
            above = d[d >= x]
            below = d[d < x]
            
            above_num_0 = len(above[loop_icc_classifications == 0])
            above_num_1 = len(above[loop_icc_classifications == 1])
            above_total = above_num_0 + above_num_1
            if above_total != 0:
                above_percent_0 = (above_num_0/above_total)*100
                above_percent_1 = (above_num_1/above_total)*100
            else:
                above_percent_0 = 0
                above_percent_1 = 0

            above_x.append((x, above_percent_0, above_percent_1, above_num_0, above_num_1))

            below_num_0 = len(below[loop_icc_classifications == 0])
            below_num_1 = len(below[loop_icc_classifications == 1])
            below_total = below_num_0 + below_num_1
            if below_total != 0:
                below_percent_0 = (below_num_0/below_total)*100
                below_percent_1 = (below_num_1/below_total)*100
            else:
                below_percent_0 = 0
                below_percent_1 = 0

            below_x.append((x, below_percent_0, below_percent_1, below_num_0, below_num_1))

        fig = plt.figure(i)
        above_ax = fig.add_subplot(2, 1, 1)
        below_ax = fig.add_subplot(2, 1, 2)

        above_ax.set_ylabel('above')

        below_ax.set_xlabel(metric)
        below_ax.set_ylabel('below')

        w=step/4

        for (pos, percent_0, percent_1, amount_0, amount_1) in above_x:
            above_ax.bar(x=pos, height=percent_0, width=w, bottom=0, color='red')
            above_ax.text(x=pos-w/2,y=(percent_0/2), s=str(round(percent_0))+'%', color='black', fontsize=6)
            above_ax.text(x=pos-w/2,y=(percent_0/2)-5, s='('+str(amount_0)+')', color='black', fontsize=4)
            above_ax.bar(x=pos, height=percent_1, width=w, bottom=percent_0, color='green')
            above_ax.text(x=pos-w/2,y=(percent_1/2)+percent_0, s=str(round(percent_1))+'%', color='black', fontsize=6)
            above_ax.text(x=pos-w/2,y=(percent_1/2)+percent_0-5, s='('+str(amount_1)+')', color='black', fontsize=4)

        for (pos, percent_0, percent_1, amount_0, amount_1) in below_x:
            below_ax.bar(x=pos, height=percent_0, width=w, bottom=0, color='red')
            below_ax.text(x=pos-w/2,y=(percent_0/2), s=str(round(percent_0))+'%', color='black', fontsize=6)
            below_ax.text(x=pos-w/2,y=(percent_0/2)-5, s='('+str(amount_0)+')', color='black', fontsize=4)
            below_ax.bar(x=pos, height=percent_1, width=w, bottom=percent_0, color='green')
            below_ax.text(x=pos-w/2,y=(percent_1/2)+percent_0, s=str(round(percent_1))+'%', color='black', fontsize=6)
            below_ax.text(x=pos-w/2,y=(percent_1/2)+percent_0-5, s='('+str(amount_1)+')', color='black', fontsize=4)

        i = i+1
        fig.savefig(report_folder + "/" + metric, transparent=False, dpi=320, bbox_inches="tight")
