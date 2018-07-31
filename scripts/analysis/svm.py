#! /usr/bin/python3

import sys

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from mpl_toolkits.mplot3d import Axes3D
from sklearn.decomposition import PCA as sklearnPCA
from sklearn.cluster import KMeans
from sklearn.datasets.samples_generator import make_blobs
from sklearn import svm

import ppar

if __name__ == "__main__":

    print("=== Support Vector Machines (SVM) statistical learning ===")
    raw_data_filename = sys.argv[1]
    report_filename = sys.argv[2]
    print("SVM input: " + raw_data_filename)
    print("SVM report: " + report_filename)
    
    # load raw data file
    data = pd.read_csv(raw_data_filename)
    # loop locations in benchmark source code
    loop_locations = data['loop-location']
    # prepare statistical learning labels 
    loop_icc_classifications = data['ICC-parallel']
    # prepare statistical learning features 
    features = data.drop(['loop-location','ICC-parallel'], axis=1)

    # prepare data for different metric groups
    metrics_data = {}
    for metric_group in ppar.metrics:
        metrics_data[metric_group] = features[ppar.metrics[metric_group]]

    # prepare data for single metrics
    metric_data = {}
    for metric in ppar.metric_list:
        metric_data[metric] = features[metric]

    report_file = open(report_filename,'w')

    
    for training_set_size in list(range(100,1001,100)):
        for training_set_size in list(range(100,1001,100)):
        
        
        
        print(str(training_set_size) + " ")


    
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
