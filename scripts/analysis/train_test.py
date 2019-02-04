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

    print("==============================================================================")
    print("=== Machine Learning vs. Intel C/C++ Compiler (ICC) competition experiment ===")
    print("==============================================================================")

    train_data_filename = sys.argv[1]
    test_data_filename = sys.argv[2]
    report_folder = sys.argv[3] + "/"
    std_num = int(sys.argv[4]) # outliers screening parameter
    norm = sys.argv[5]

    if not os.path.exists(train_data_filename):
        sys.exit("error: " + train_data_filename + " data file does not exist!")

    if not os.path.exists(test_data_filename):
        sys.exit("error: " + test_data_filename + " data file does not exist!")

    if not os.path.exists(report_folder):
        sys.exit("error: " + report_folder + " report folder does not exist!")

    print("Train data input: " + train_data_filename)
    print("Test data input: " + test_data_filename)
    print("Report folder: " + report_folder)
    print("Screen outliers outside " + str(std_num) + "-sigma standard deviations")

    ###
    # Prepare Training Data
    ###

    # load training data file
    train_data = pd.read_csv(train_data_filename)
    # loop locations in the benchmark source code
    train_loop_locations = train_data['loop-location']
    # prepare real parallel labels 
    train_par_labels = train_data['parallel']
    # prepare loop icc labels
    train_icc_labels = train_data['icc']
    # prepare statistical learning features 
    train_features = train_data.drop(['loop-location','parallel','icc'], axis=1)

    # remove outliers from the data
    if std_num != 0:
        filtered_idxs = {}
        for metric in ppar.metric_list:
            d = train_features[metric]
            filtered_idxs[metric] = train_features[abs(d-d.mean()) <= std_num*d.std()].index
    
        filtered_idx = train_features.index
        for metric in ppar.metric_list:  
            filtered_idx &= filtered_idxs[metric]

        train_loop_locations = train_loop_locations[filtered_idx]
        train_par_labels = train_par_labels[filtered_idx]
        train_icc_labels = train_icc_labels[filtered_idx]
        
        idxs_to_drop = train_features.index.drop(filtered_idx)
        train_features = train_features.drop(idxs_to_drop)

        train_loop_locations = train_loop_locations.reset_index(drop=True)
        train_par_labels = train_par_labels.reset_index(drop=True)
        train_icc_labels = train_icc_labels.reset_index(drop=True)
        train_features = train_features.reset_index(drop=True)

    # normalize the data
    if norm == "norm":
        for feature in ppar.metric_list:
            if (train_features[feature].max() - train_features[feature].min()) != 0:
                train_features[feature] = (train_features[feature] - train_features[feature].min())/(train_features[feature].max() - train_features[feature].min())

    # prepare data for different metric groups
    train_metrics_data = {}
    for metric_group in ppar.metric_groups:
        train_metrics_data[metric_group] = train_features[ppar.metric_groups[metric_group]]

    # prepare data for different metric groups
    train_metric_set_data = {}
    for metric_set in ppar.metric_sets:
        train_metric_set_data[metric_set] = train_features[ppar.metric_sets[metric_set]]

    ###
    # Prepare Testing Data
    ###

    # load training data file
    test_data = pd.read_csv(test_data_filename)
    # loop locations in the benchmark source code
    test_loop_locations = test_data['loop-location']
    # prepare real parallel labels 
    test_par_labels = test_data['parallel']
    # prepare loop icc labels
    test_icc_labels = test_data['icc']
    # prepare statistical learning features 
    test_features = test_data.drop(['loop-location','parallel','icc'], axis=1)

    # remove outliers from the data
    if std_num != 0:
        filtered_idxs = {}
        for metric in ppar.metric_list:
            d = test_features[metric]
            filtered_idxs[metric] = test_features[abs(d-d.mean()) <= std_num*d.std()].index
    
        filtered_idx = test_features.index
        for metric in ppar.metric_list:  
            filtered_idx &= filtered_idxs[metric]

        test_loop_locations = test_loop_locations[filtered_idx]
        test_par_labels = test_par_labels[filtered_idx]
        test_icc_labels = test_icc_labels[filtered_idx]
        
        idxs_to_drop = test_features.index.drop(filtered_idx)
        test_features = test_features.drop(idxs_to_drop)

        test_loop_locations = test_loop_locations.reset_index(drop=True)
        test_par_labels = test_par_labels.reset_index(drop=True)
        test_icc_labels = test_icc_labels.reset_index(drop=True)
        test_features = test_features.reset_index(drop=True)

    # normalize the data
    if norm == "norm":
        for feature in ppar.metric_list:
            if (test_features[feature].max() - test_features[feature].min()) != 0:
                test_features[feature] = (test_features[feature] - test_features[feature].min())/(test_features[feature].max() - test_features[feature].min())

    # prepare data for different metric groups
    test_metrics_data = {}
    for metric_group in ppar.metric_groups:
        test_metrics_data[metric_group] = test_features[ppar.metric_groups[metric_group]]

    # prepare data for different metric groups
    test_metric_set_data = {}
    for metric_set in ppar.metric_sets:
        test_metric_set_data[metric_set] = test_features[ppar.metric_sets[metric_set]]

    # do the ICC competition experiment for all metric sets
    for metric_set in ppar.metric_sets:
        report_filename = report_folder + metric_set + ".report"
        report_file = open(report_filename,'w')

        # print the header into the report file
        print('ML-vs-ICC.report', file=report_file)
        print("DT with features from " + metric_set + " metric set")
        report_file.write(metric_set + " ") 
        report_file.write("[") 
        for metric in ppar.metric_sets[metric_set]: 
            report_file.write(" " + str(metric))
        report_file.write(" ]\n") 
        
        train_dataset = train_metric_set_data[metric_set]
        test_dataset = test_metric_set_data[metric_set]

        # fit SVM model to the training dataset
        clf = tree.DecisionTreeClassifier()
        clf.fit(train_dataset, train_par_labels)

        tree.export_graphviz(clf, out_file=report_folder + metric_set + '.tree3.dot', max_depth=3, class_names=['non-parallelizable','parallelizable'], feature_names=ppar.metric_sets[metric_set], filled=True, rounded=True)
        tree.export_graphviz(clf, out_file=report_folder + metric_set + '.tree5.dot', max_depth=5, class_names=['non-parallelizable','parallelizable'], feature_names=ppar.metric_sets[metric_set], filled=True, rounded=True)

        predictions = clf.predict(test_dataset)

        # calculate learning performance
        mispredicted_loops = set()
        predicted_loops = set()
        icc_win_loops = set()
        icc_loss_loops = set()

        tests = len(predictions)
        mispredicts = 0
        safe_mispredicts = 0
        unsafe_mispredicts = 0
        icc_wins = 0
        icc_losses = 0

        case_00_num = 0
        case_01_num = 0
        case_10_num = 0
        case_11_num = 0
                
        case_00_par = 0
        case_01_par = 0
        case_10_par = 0
        case_11_par = 0
                
        case_00_npar = 0
        case_01_npar = 0
        case_10_npar = 0
        case_11_npar = 0

        cases_00_par = set()
        cases_01_par = set()
        cases_10_par = set()
        cases_11_par = set()
                
        cases_00_npar = set()
        cases_01_npar = set()
        cases_10_npar = set()
        cases_11_npar = set()

        icc_par = 0
        icc_npar = 0
        real_par = 0
        real_npar = 0

        for i in range(0,tests):
            pred = predictions[i]
            icc = test_icc_labels[i]
            par = test_par_labels[i]

            if icc == 0:
                if pred == 0:
                    case_00_num += 1
                    if par == 0:
                        case_00_npar += 1
                        cases_00_npar.add(str(test_loop_locations[i]))
                    elif par == 1:
                        case_00_par += 1
                        cases_00_par.add(str(test_loop_locations[i]))
                elif pred == 1:
                    case_01_num += 1
                    if par == 0:
                        case_01_npar += 1
                        cases_01_npar.add(str(test_loop_locations[i]))
                    elif par == 1:
                        case_01_par += 1
                        cases_01_par.add(str(test_loop_locations[i]))
            elif icc == 1:
                if pred == 0:
                    case_10_num += 1
                    if par == 0:
                        case_10_npar += 1
                        cases_10_npar.add(str(test_loop_locations[i]))
                    elif par == 1:
                        case_10_par += 1
                        cases_10_par.add(str(test_loop_locations[i]))
                elif pred == 1:
                    case_11_num += 1
                    if par == 0:
                        case_11_npar += 1
                        cases_11_npar.add(str(test_loop_locations[i]))
                    elif par == 1:
                        case_11_par += 1
                        cases_11_par.add(str(test_loop_locations[i]))
         
            if pred != par:
                # misprediction
                mispredicts += 1
                if pred == 1:
                    unsafe_mispredicts += 1
                    mispredicted_loops.add(str(test_loop_locations[i]) + ":" + str(predictions[i]) + ":" + str(test_par_labels[i]) + ":unsafe")
                else:
                    safe_mispredicts += 1
                    mispredicted_loops.add(str(test_loop_locations[i]) + ":" + str(predictions[i]) + ":" + str(test_par_labels[i]) + ":safe")
            else:
                predicted_loops.add(str(test_loop_locations[i]) + ":" + str(predictions[i]) + ":" + str(test_par_labels[i]))

            if pred != icc:
                if pred == par:
                    icc_wins += 1
                    icc_win_loops.add(str(test_loop_locations[i]) + ":" + str(predictions[i]) + ":" + str(test_icc_labels[i]) + ":win")
                else:
                    icc_losses += 1
                    icc_loss_loops.add(str(test_loop_locations[i]) + ":" + str(predictions[i]) + ":" + str(test_icc_labels[i]) + ":loss")
  
        # Table
        # 0 0
        # 0 1
        # 1 0
        # 1 1
        report_file.write("Table" + "\n")
        report_file.write("========" + "\n")
        report_file.write("=== case [0 / 0] ===" + "\n")
        report_file.write("num: " + "{}".format(case_00_num) + "\n")
        report_file.write("par: " + "{}".format(case_00_par) + "\n")
        report_file.write("non-par: " + "{}".format(case_00_npar) + "\n")
        report_file.write("=== case [0 / 1] ===" + "\n")
        report_file.write("num: " + "{}".format(case_01_num) + "\n")
        report_file.write("par: " + "{}".format(case_01_par) + "\n")
        report_file.write("non-par: " + "{}".format(case_01_npar) + "\n")
        report_file.write("=== case [1 / 0] ===" + "\n")
        report_file.write("num: " + "{}".format(case_10_num) + "\n")
        report_file.write("par: " + "{}".format(case_10_par) + "\n")
        report_file.write("non-par: " + "{}".format(case_10_npar) + "\n")
        report_file.write("=== case [1 / 1] ===" + "\n")
        report_file.write("num: " + "{}".format(case_11_num) + "\n")
        report_file.write("par: " + "{}".format(case_11_par) + "\n")
        report_file.write("non-par: " + "{}".format(case_11_npar) + "\n")
        report_file.write("========" + "\n")
        report_file.write("\n")

        report_file.write("=== case [0 / 0] ===" + "\n")
        report_file.write("= parallelisible =" + "\n")
        for loop in cases_00_par:
            report_file.write(loop + "\n")
        report_file.write("= non-parallelisible =" + "\n")
        for loop in cases_00_npar:
            report_file.write(loop + "\n")
        report_file.write("===================" + "\n")
        report_file.write("\n")

        report_file.write("=== case [0 / 1] ===" + "\n")
        report_file.write("= parallelisible =" + "\n")
        for loop in cases_01_par:
            report_file.write(loop + "\n")
        report_file.write("= non-parallelisible =" + "\n")
        for loop in cases_01_npar:
            report_file.write(loop + "\n")
        report_file.write("===================" + "\n")
        report_file.write("\n")

        report_file.write("=== case [1 / 0] ===" + "\n")
        report_file.write("= parallelisible =" + "\n")
        for loop in cases_10_par:
            report_file.write(loop + "\n")
        report_file.write("= non-parallelisible =" + "\n")
        for loop in cases_10_npar:
            report_file.write(loop + "\n")
        report_file.write("===================" + "\n")
        report_file.write("\n")

        report_file.write("=== case [1 / 1] ===" + "\n")
        report_file.write("= parallelisible =" + "\n")
        for loop in cases_11_par:
            report_file.write(loop + "\n")
        report_file.write("= non-parallelisible =" + "\n")
        for loop in cases_11_npar:
            report_file.write(loop + "\n")
        report_file.write("===================" + "\n")
        report_file.write("\n")

        accuracy_rate = 1 - mispredicts/len(predictions)
        error_rate = mispredicts/len(predictions)

        safe_mispredict_rate = 0
        unsafe_mispredict_rate = 0
        if mispredicts != 0:
            safe_mispredict_rate = safe_mispredicts/mispredicts
            unsafe_mispredict_rate = unsafe_mispredicts/mispredicts

        icc_win_rate = 0
        icc_loss_rate = 0
        icc_disagreements = icc_wins + icc_losses
        if icc_disagreements != 0:
            # wins
            icc_win_rate = icc_wins/icc_disagreements
            # losses
            icc_loss_rate = icc_losses/icc_disagreements

        # calculate prediction accuracy 
        accuracy = accuracy_score(test_par_labels, clf.predict(test_dataset))
            
        report_file.write("Mispredictions: " + str(mispredicts) + "\n")
        report_file.write("\n")

        report_file.write("Accuracy" + "\n")
        report_file.write("========" + "\n")
        report_file.write("tests: " + "{}".format(tests) + "\n")
        report_file.write("mispredicts: " + "{}".format(mispredicts) + "\n")
        report_file.write("accuracy: " + "{0:.2f}".format(accuracy) + "\n")
        report_file.write("accuracy-rate: " + "{0:.2f}".format(accuracy_rate) + "\n")
        report_file.write("error-rate: " + "{0:.2f}".format(error_rate) + "\n")
        report_file.write("safe-mispredicts: " + "{}".format(safe_mispredicts) + "\n")
        report_file.write("unsafe-mispredicts: " + "{}".format(unsafe_mispredicts) + "\n")
        report_file.write("safe-error-rate: " + "{0:.2f}".format(safe_mispredict_rate) + "\n")
        report_file.write("unsafe-error-rate: " + "{0:.2f}".format(unsafe_mispredict_rate) + "\n")
        report_file.write("========" + "\n")
        report_file.write("\n")
                
        report_file.write("ICC competition" + "\n")
        report_file.write("========" + "\n")
        report_file.write("icc-discrepancies: " + "{}".format(icc_disagreements) + "\n")
        report_file.write("icc-wins: " + "{}".format(icc_wins) + "\n")
        report_file.write("icc-losses: " + "{}".format(icc_losses) + "\n")
        report_file.write("win-rate: " + "{0:.2f}".format(icc_win_rate) + "\n")
        report_file.write("loss-rate: " + "{0:.2f}".format(icc_loss_rate) + "\n")
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

        report_file.write("loop:pred:icc:result\n")
        
        report_file.write("Win over ICC loops:" + "\n")
        report_file.write("===================" + "\n")
        for loop_report in icc_win_loops:
            report_file.write(loop_report + "\n")
        report_file.write("===================" + "\n")
      
        report_file.write("\n")
        
        report_file.write("Loss to ICC loops:" + "\n")
        report_file.write("===================" + "\n")
        for loop_report in icc_loss_loops:
            report_file.write(loop_report + "\n")
        report_file.write("===================" + "\n")

        report_file.write("\n")

        report_file.write("==============================================================" + "\n")
        report_file.write("\n\n\n")
        
        report_file.close()
