#! /usr/bin/python3

import sys
import random

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from mpl_toolkits.mplot3d import Axes3D
from sklearn.decomposition import PCA as sklearnPCA
from sklearn.cluster import KMeans
from sklearn.datasets.samples_generator import make_blobs
from sklearn.neural_network import MLPClassifier

import ppar

if __name__ == "__main__":

    print("=== MPL Classifier statistical learning ===")
    raw_data_filename = sys.argv[1]
    report_filename = sys.argv[2]
    print("MPL Classifier input: " + raw_data_filename)
    print("MPL Classifier report: " + report_filename)
    
    # load raw data file
    data = pd.read_csv(raw_data_filename)
    # loop locations in benchmark source code
    loop_locations = data['loop-location']
    # prepare statistical learning labels 
    loop_icc_classifications = data['icc-parallel']
    # prepare statistical learning features 
    features = data.drop(['loop-location','icc-parallel'], axis=1)

    # normalize the data
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

    report_file = open(report_filename,'w')

    # print the header into the report file
    print('MPLClassifier.report', file=report_file)
    
    # SVM for groups of metrics
    for metric_group in ppar.metric_groups:
        dataset = metrics_data[metric_group]
        print("MPL Classifier with features from " + metric_group + " metric group")

        for training_set_size in list(range(100,1000,100)): 
            training_set = dataset[0:training_set_size]
            testing_set = dataset[training_set_size:]
            training_labels = loop_icc_classifications[0:training_set_size]
            testing_labels = loop_icc_classifications[training_set_size:]

            clf = MLPClassifier(solver='lbfgs', alpha=1e-5, hidden_layer_sizes=(5, 2), random_state=1)
            clf.fit(training_set, training_labels)
            predictions = clf.predict(testing_set)

            random_labels = []
            for x in range(0,len(predictions)):
                random_labels.append(random.randint(0,1))

            error = 0
            random_error = 0
            for i in range(0,len(testing_labels)):
                test = testing_labels.tolist()
                if predictions[i] != test[i]:
                    error += 1
                if random_labels[i] != test[i]:
                    random_error += 1

            print("training set: [0:" + str(training_set_size) + "]" + ", mpl-classifier-error: " + str(error/len(testing_labels)*100) + "%" +
                   ", random-error: " + str(random_error/len(testing_labels)*100) + "%")

    # SVM for single metrics
    for metric in ppar.metric_list:
        dataset = metric_data[metric]
        dataset = dataset.values.reshape(-1,1)
        print("MPL Classifier with " + metric + " feature")

        for training_set_size in list(range(100,1000,100)): 
            training_set = dataset[0:training_set_size]
            testing_set = dataset[training_set_size:]
            training_labels = loop_icc_classifications[0:training_set_size]
            testing_labels = loop_icc_classifications[training_set_size:]

            clf = MLPClassifier(solver='lbfgs', alpha=1e-5, hidden_layer_sizes=(5, 2), random_state=1)
            clf.fit(training_set, training_labels)
            predictions = clf.predict(testing_set)

            random_labels = []
            for x in range(0,len(predictions)):
                random_labels.append(random.randint(0,1))

            error = 0
            random_error = 0
            for i in range(0,len(testing_labels)):
                test = testing_labels.tolist()
                if predictions[i] != test[i]:
                    error += 1
                if random_labels[i] != test[i]:
                    random_error += 1

            print("training set: [0:" + str(training_set_size) + "]" + ", mpl-classifier-error: " + str(error/len(testing_labels)*100) + "%" +
                   ", random-error: " + str(random_error/len(testing_labels)*100) + "%")
