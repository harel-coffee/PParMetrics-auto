#! /usr/bin/python3

import sys
import os

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from mpl_toolkits.mplot3d import Axes3D
from sklearn.decomposition import PCA
from sklearn.cluster import KMeans
from sklearn.datasets.samples_generator import make_blobs

def reject_outliers(data, m=2):
    return data[abs(data - np.mean(data)) < m * np.std(data)]

def create_folder(directory):
    try:
        if not os.path.exists(directory):
            os.makedirs(directory)
    except OSError:
        print ("error: could not create directory: " + directory)

import ppar

if __name__ == "__main__":

    print("=== Single metric vs loop parallelisability visualization ===")
    raw_data_filename = sys.argv[1]
    report_folder = sys.argv[2]
    std_num = int(sys.argv[3]) # outliers screening parameter
  
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
    loop_icc_classifications = data['icc']
    # prepare statistical learning features 
    features = data.drop(['loop-location','icc'], axis=1)
    features_abs = features.copy()

    print("To be plotted\n")
    for metric in ppar.metric_list:
        print(metric + "\n")

    # remove outliers from the data
    if std_num != 0:
        filtered_idxs = {}
        for metric in ppar.metric_list:  
            d = features[metric]
            filtered_idxs[metric] = features[abs(d-d.mean()) <= std_num*d.std()].index
    
        filtered_idx = features.index
        for metric in ppar.metric_list:  
            filtered_idx &= filtered_idxs[metric]

        loop_icc_classifications = loop_icc_classifications[filtered_idx]
        loop_locations = loop_locations[filtered_idx]
        idxs_to_drop = features.index.drop(filtered_idx)
        features = features.drop(idxs_to_drop)
        features_abs = features_abs.drop(idxs_to_drop)

        loop_icc_classifications = loop_icc_classifications.reset_index(drop=True)
        loop_locations = loop_locations.reset_index(drop=True)
        features = features.reset_index(drop=True)
        features_abs = features_abs.reset_index(drop=True)

    # normalize the data
    for feature in ppar.metric_list:
        features[feature] = (features[feature] - features[feature].min())/(features[feature].max() - features[feature].min())

    # prepare data for different metric groups
    metric_group_data = {}
    for metric_group in ppar.metric_groups:
        metric_group_data[metric_group] = features[ppar.metric_groups[metric_group]]

    metric_data = {}
    for metric in ppar.metric_list:
        metric_data[metric] = features[metric]

    # visualise parallelisability vs single metrics
    for metric in ppar.metric_list:
        print("Plotting " + metric + "\n")
        
        fig, ax = plt.subplots()
        
        ax.set_ylabel(metric)
        ax.set_xlabel("loops")
        
        # remove outliers out of the set
        d = metric_data[metric]
        if std_num != 0:
            x = d[abs(d-d.mean()) <= std_num*d.std()]
            y = loop_icc_classifications[abs(d-d.mean()) <= std_num*d.std()]
        else:
            x = d
            y = loop_icc_classifications

        if len(x) == 0 or len(y) == 0:
            continue

        # scatter non-parallelisible points over the figure 
        ax.scatter((x[y==0]).index, x[y==0], s=1, c='red', marker=".", alpha=1.0)
        m = (x[y==0]).mean()
        disp = (x.max()-x.min())/100 # 1% of x range
        ax.axhline(m, c='red', linestyle='--', linewidth=0.5)
        ax.text((y.index.min()+y.index.max())/2-100, m+disp, "np ~= "+"{0:.2f}".format((x[y==0]).mean()), fontsize=6, color='red')

        # scatter parallelisible points over the figure 
        ax.scatter((x[y==1]).index, x[y==1], s=1, c='green', marker="+", alpha=1.0)
        ax.axhline((x[y==1]).mean(), c='green', linestyle='--', linewidth=0.5, label=x[y==1].mean())
        m = (x[y==1]).mean()
        ax.text((y.index.min()+y.index.max())/2+100, m+disp, "p ~= "+"{0:.2f}".format((x[y==1]).mean()), fontsize=6, color='green')

        fig.savefig(normalized_folder + metric, transparent=False, dpi=320, bbox_inches="tight")
        plt.close(fig)
