#! /usr/bin/python3

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import graphviz

from mpl_toolkits.mplot3d import Axes3D
from sklearn.decomposition import PCA as sklearnPCA
from sklearn.cluster import KMeans
from sklearn.datasets.samples_generator import make_blobs
from sklearn import tree

# prepare a set of metrics for further reference
metrics = {}
metrics['loop-proportion'] = ['loop-absolute-size', 
                              'loop-payload-fraction', 
                              'loop-proper-sccs-number']
metrics['iterator-payload-cohesion'] = ['iterator-payload-total-cohesion', 
                                        'iterator-payload-non-cf-cohesion', 
                                        'iterator-payload-mem-cohesion']
metrics['critical-payload-cohesion'] = ['critical-payload-total-cohesion', 
                                        'critical-payload-non-cf-cohesion', 
                                        'critical-payload-mem-cohesion']
metrics['payload-dependencies-number'] = ['payload-total-dependencies-number', 
                                          'payload-true-dependencies-number', 
                                          'payload-anti-dependencies-number', 
                                          'payload-output-dependencies-number']
metrics['critical-payload-dependencies-number'] = ['critical-payload-total-dependencies-number', 
                                                   'critical-payload-true-dependencies-number', 
                                                   'critical-payload-anti-dependencies-number'] 
metrics['all'] = []
for metrics_list in metrics:
    if metrics_list != 'all':
        metrics['all'].extend(metrics[metrics_list])
metrics_list = metrics['all'] 
        
# load original metrics+icc report file
data = pd.read_csv('/home/s1736883/Work/PParMetrics/analysis/raw_data.csv')
loop_locations = data['loop-location']
loop_icc_classifications = data['ICC-parallel']

features = data.drop(['loop-location','ICC-parallel'], axis=1)

training_set_size = 1000

# normalize the data
#data_kmeans = (data_kmeans-data_kmeans.mean())/data_kmeans.std()

# prepare data for different metric groups
metrics_data = {}
for metric_group in metrics:
    metrics_data[metric_group] = features[metrics[metric_group]]

# prepare data for single metrics
metric_data = {}
for metric in metrics_list:
    metric_data[metric] = features[metric]

# SVM for groups of metrics
for metric_group in metrics:
    dataset = metrics_data[metric_group]
    print("SVM with features from " + metric_group + " metric group")

    for training_set_size in list(range(100,1000,100)): 
        training_set = dataset[0:training_set_size]
        testing_set = dataset[training_set_size:]
        training_labels = loop_icc_classifications[0:training_set_size]
        testing_labels = loop_icc_classifications[training_set_size:]

        clf = tree.DecisionTreeClassifier()
        clf.fit(training_set, training_labels)
        predictions = clf.predict(testing_set)

        error = 0
        for i in range(0,len(testing_labels)):
            test = testing_labels.tolist()
            if predictions[i] != test[i]:
                error += 1

        print("training set: [0:" + str(training_set_size) + "]" + ", svm-error: " + str(error/len(testing_labels)*100) + "%")

# SVM for single metrics
for metric in metrics_list:
    dataset = metric_data[metric]
    dataset = dataset.values.reshape(-1,1)
    print("SVM with " + metric + " feature")

    for training_set_size in list(range(100,1000,100)): 
        training_set = dataset[0:training_set_size]
        testing_set = dataset[training_set_size:]
        training_labels = loop_icc_classifications[0:training_set_size]
        testing_labels = loop_icc_classifications[training_set_size:]

        clf = tree.DecisionTreeClassifier()
        clf.fit(training_set, training_labels)
        predictions = clf.predict(testing_set)

        error = 0
        for i in range(0,len(testing_labels)):
            test = testing_labels.tolist()
            if predictions[i] != test[i]:
                error += 1

        print("training set: [0:" + str(training_set_size) + "]" + ", svm-error: " + str(error/len(testing_labels)*100) + "%")
