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

    print("=== Decision Tree (DT) statistical learning ===")
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

    # visualize trees for single metrics
    for metric in ppar.metric_list:
        dataset = metric_data[metric]
        training_data = dataset
        training_labels = loop_icc_classifications
        
        clf = tree.DecisionTreeClassifier()
        clf.fit(training_data.values.reshape(-1,1), training_labels)
        
        tree.export_graphviz(clf, out_file=report_folder + "/" + str(metric) + '.tree.dot', max_depth=1, class_names=['non-parallelizable','parallelizable'], feature_names=[str(metric)], filled=True)   

    # visualize trees for sets of
    for metric_set in ppar.metric_sets:
        dataset = metric_set_data[metric_set]
        training_data = dataset
        training_labels = loop_icc_classifications
        
        clf = tree.DecisionTreeClassifier()
        clf.fit(training_data, training_labels)
        
        tree.export_graphviz(clf, out_file=report_folder + "/" + str(metric_set) + '.tree3.dot', max_depth=3, class_names=['non-parallelizable','parallelizable'], feature_names=ppar.metric_sets[metric_set], filled=True, rounded=True)
        tree.export_graphviz(clf, out_file=report_folder + "/" + str(metric_set) + '.tree5.dot', max_depth=5, class_names=['non-parallelizable','parallelizable'], feature_names=ppar.metric_sets[metric_set], filled=True, rounded=True)
