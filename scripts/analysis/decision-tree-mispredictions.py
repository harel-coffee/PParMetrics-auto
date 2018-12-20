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

    print("=== Decision Tree (DT) misprediction analysis and features debugging ===")

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
        loop_locations = loop_locations[filtered_idx]
        idxs_to_drop = features.index.drop(filtered_idx)
        features = features.drop(idxs_to_drop)

        loop_icc_classifications = loop_icc_classifications.reset_index(drop=True)
        loop_locations = loop_locations.reset_index(drop=True)
        features = features.reset_index(drop=True)

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

    report_filename = report_folder + "dt.report"
    report_file = open(report_filename,'w')

    # print the header into the report file
    print('DT.report', file=report_file)

    metric_set = 'set_0'
    dataset = metric_set_data[metric_set]
    report_file.write(metric_set + " ") 
    report_file.write("[") 
    for metric in ppar.metric_sets[metric_set]: 
        report_file.write(" " + str(metric))
    report_file.write(" ]\n") 

    random_state = 12883823
    splits_num = 20
    report_file.write("splits num: " + str(splits_num)+ "\n")

    rkf = RepeatedKFold(n_splits=splits_num, n_repeats=3, random_state=random_state)

    accuracy_samples = []
    num = 0
    for train, test in rkf.split(dataset):
        report_file.write("=== Split: " + str(num)+ " ===\n")
        report_file.write("==============================================================" + "\n")
        num = num + 1

        training_data = dataset.iloc[train]
        testing_data = dataset.iloc[test]
        training_labels = loop_icc_classifications.iloc[train]
        testing_labels = loop_icc_classifications.iloc[test]
        training_loops = loop_locations.iloc[train]
        testing_loops = loop_locations.iloc[test]
        report_file.write("Training data size: " + str(len(training_data)) + "\n")
        report_file.write("Testing data size: " + str(len(testing_data)) + "\n")

        # fit SVM model to the training dataset
        clf = tree.DecisionTreeClassifier()
        clf.fit(training_data, training_labels)
        
        tree.export_graphviz(clf, out_file=report_folder + "/" + "split_" + str(num-1) + '.tree3.dot', max_depth=3, class_names=['non-parallelizable','parallelizable'], feature_names=ppar.metric_sets[metric_set], filled=True, rounded=True)
        tree.export_graphviz(clf, out_file=report_folder + "/" + "split_" + str(num-1) + '.tree5.dot', max_depth=5, class_names=['non-parallelizable','parallelizable'], feature_names=ppar.metric_sets[metric_set], filled=True, rounded=True)
                
        predictions = clf.predict(testing_data)
        testing_labels = testing_labels.reset_index(drop=True)
        testing_loops = testing_loops.reset_index(drop=True)

        mispredicted_loops = set()
        predicted_loops = set()
        mispredicts = 0
        safe_mispredicts = 0
        unsafe_mispredicts = 0
        for i in range(0,len(predictions)):
            pred = predictions[i]
            test = testing_labels[i]
            if pred != test:
                mispredicts += 1
                if pred == 1:
                    unsafe_mispredicts += 1
                    mispredicted_loops.add(str(testing_loops[i]) + ":" + str(predictions[i]) + ":" + str(testing_labels[i]) + ":safe")
                else:
                    safe_mispredicts += 1
                    mispredicted_loops.add(str(testing_loops[i]) + ":" + str(predictions[i]) + ":" + str(testing_labels[i]) + ":unsafe")
            else:
                predicted_loops.add(str(testing_loops[i]) + ":" + str(predictions[i]) + ":" + str(testing_labels[i]))

        report_file.write("Mispredictions: " + str(mispredicts) + "\n")
        report_file.write("\n")

        report_file.write("Accuracy:" + "\n")
        report_file.write("========" + "\n")

        accuracy = accuracy_score(testing_labels, clf.predict(testing_data))
        report_file.write("dt-accuracy:" + "{0:.2f}".format(accuracy) + "\n")
        accuracy_samples.append(accuracy)
        report_file.write("mispredicts:" + "{0:.2f}".format(mispredicts/len(predictions)) + "\n")
        report_file.write("safe-mispredicts:" + "{0:.2f}".format(safe_mispredicts/mispredicts*100) + "\n")
        report_file.write("unsafe-mispredicts:" + "{0:.2f}".format(unsafe_mispredicts/mispredicts*100) + "\n")
        report_file.write("========" + "\n")

        report_file.write("\n")

        report_file.write("loop:pred:label:safe\n")
        
        report_file.write("Mispredicted loops:" + "\n")
        report_file.write("===================" + "\n")
        for loop_report in mispredicted_loops:
            report_file.write(loop_report + "\n")
        report_file.write("===================" + "\n")
      
        report_file.write("\n")
        
        report_file.write("Predicted loops:" + "\n")
        report_file.write("===================" + "\n")
        for loop_report in predicted_loops:
            report_file.write(loop_report + "\n")
        report_file.write("===================" + "\n")

        report_file.write("\n")

        report_file.write("==============================================================" + "\n")
        report_file.write("\n\n\n")

    report_file.write("dt-accuracy-mean:" + "{0:.2f}".format(pd.Series(accuracy_samples).mean()*100) + "\n")
    report_file.write("\n")
