#! /usr/bin/python3

import sys

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from mpl_toolkits.mplot3d import Axes3D
from sklearn.decomposition import PCA as sklearnPCA
from sklearn import svm


import ppar

if __name__ == "__main__":

    print("=== Support Vector Machines (SVM) statistical learning ===")
    raw_data_filename = sys.argv[1]
    report_filename = sys.argv[2]
    std_num = sys.argv[3]
    norm = sys.argv[4]
    print("SVM input: " + raw_data_filename)
    print("SVM report: " + report_filename)
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
    for metric_group in ppar.metrics:
        metrics_data[metric_group] = features[ppar.metrics[metric_group]]

    # prepare data for single metrics
    metric_data = {}
    for metric in ppar.metric_list:
        metric_data[metric] = features[metric]

    report_file = open(report_filename,'w')

    total_size = len(loop_icc_classifications)
    print("total size:" + str(total_size))
    for training_set_size in list(range(100,1001,100)):
        intervals_num = total_size/training_set_size
        for i in range(0,int(intervals_num)):
            interval_start = i*training_set_size
            interval_end = interval_start + training_set_size
            print("[" + str(interval_start) + ":" + str(interval_end) + "]")
    
    # print the header into the report file
    print('SVM.report', file=report_file)
    '''
    # SVM for groups of metrics
    for metric_group in ppar.metrics:
        dataset = metrics_data[metric_group]
        print("SVM with features from " + metric_group + " metric group")

        for training_set_size in list(range(100,1000,100)): 
            training_set = dataset[0:training_set_size]
            testing_set = dataset[training_set_size:]
            training_labels = loop_icc_classifications[0:training_set_size]
            testing_labels = loop_icc_classifications[training_set_size:]

            clf = svm.SVC()
            clf.fit(training_set, training_labels)
            predictions = clf.predict(testing_set)

            error = 0
            for i in range(0,len(testing_labels)):
                test = testing_labels.tolist()
                if predictions[i] != test[i]:
                    error += 1

            print("training set: [0:" + str(training_set_size) + "]" + ", svm-error: " + str(error/len(testing_labels)*100) + "%")

    # SVM for single metrics
    for metric in ppar.metric_list:
        dataset = metric_data[metric]
        dataset = dataset.values.reshape(-1,1)
        print("SVM with " + metric + " feature")

        for training_set_size in list(range(100,1000,100)): 
            training_set = dataset[0:training_set_size]
            testing_set = dataset[training_set_size:]
            training_labels = loop_icc_classifications[0:training_set_size]
            testing_labels = loop_icc_classifications[training_set_size:]

            clf = svm.SVC()
            clf.fit(training_set, training_labels)
            predictions = clf.predict(testing_set)

            error = 0
            for i in range(0,len(testing_labels)):
                test = testing_labels.tolist()
                if predictions[i] != test[i]:
                    error += 1

            print("training set: [0:" + str(training_set_size) + "]" + ", svm-error: " + str(error/len(testing_labels)*100) + "%")
    '''
