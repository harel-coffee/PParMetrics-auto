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

    print("=================================================================")
    print("=== Decision Tree (DT) based predictor analysis and debugging ===")
    print("=================================================================")

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

    # loop locations in the benchmark source code
    loop_locations = data['loop-location']
    # prepare loop icc answers
    loop_icc_answers = data['icc']

    # prepare statistical learning features 
    features = data.drop(['loop-location','icc'], axis=1)
    
    # remove outliers from the data
    # indexes of left data
    filtered_idx = features.index
    if std_num != 0:
        filtered_idxs = {}
        for metric in ppar.metric_list:  
            d = features[metric]
            filtered_idxs[metric] = features[abs(d-d.mean()) < std_num*d.std()].index
            filtered_idx &= filtered_idxs[metric]

        loop_locations = loop_locations[filtered_idx]
        loop_icc_answers = loop_icc_answers[filtered_idx]
        idxs_to_drop = features.index.drop(filtered_idx)
        features = features.drop(idxs_to_drop)

        loop_locations = loop_locations.reset_index(drop=True)
        loop_icc_answers = loop_icc_answers.reset_index(drop=True)
        features = features.reset_index(drop=True)

    # prepare data for different metric groups
    metrics_data = {}
    for metric_group in ppar.metric_groups:
        metrics_data[metric_group] = features[ppar.metric_groups[metric_group]]

    # prepare data for single metrics
    metric_data = {}
    for metric in ppar.metric_list:
        metric_data[metric] = features[metric]

    # prepare data for different metric sets
    metric_set_data = {}
    for metric_set in ppar.metric_sets:
        metric_set_data[metric_set] = features[ppar.metric_sets[metric_set]]

    # conduct precision experiments and get debugging data for all of them   
    random_state = 12883823
    for splits_num in [5,10,15,20,30]:
        print("DT for splits num " + str(splits_num))
        # create a folder for debug report
        splits_folder_name = report_folder + "split_" + str(splits_num) + "/"
        create_folder(splits_folder_name)

        rkf = RepeatedKFold(n_splits=splits_num, n_repeats=5, random_state=random_state)

        for metric_set in ppar.metric_sets:
            dataset = metric_set_data[metric_set]
            print("DT with features from " + metric_set + " metric set")

            # create a folder for debug report
            folder_name = splits_folder_name + metric_set + "/"
            create_folder(folder_name)
            report_filename = folder_name + "dt.report"
            report_file = open(report_filename,'w')

            # print the header into the report file
            print('DT.report', file=report_file)

            report_file.write(metric_set + " ") 
            report_file.write("[") 
            for metric in ppar.metric_sets[metric_set]: 
                report_file.write(" " + str(metric))
            report_file.write(" ]\n") 

            accuracy_samples = []
            accuracy_rates = []
            error_rates = []
            safe_mispredict_rates = []
            unsafe_mispredict_rates = []
            icc_win_rates = []
            icc_loss_rates = []
            num = 0
            for train, test in rkf.split(dataset):
                report_file.write("=== Split: " + str(num)+ " ===\n")
                report_file.write("==============================================================" + "\n")

                # prepare label vectors
                training_data = dataset.iloc[train]
                testing_data = dataset.iloc[test]
                training_icc = loop_icc_answers.iloc[train]
                testing_icc = loop_icc_answers.iloc[test]
                training_loops = loop_locations.iloc[train]
                testing_loops = loop_locations.iloc[test]

                testing_icc = testing_icc.reset_index(drop=True)
                testing_loops = testing_loops.reset_index(drop=True)

                report_file.write("Training data size: " + str(len(training_data)) + "\n")
                report_file.write("Testing data size: " + str(len(testing_data)) + "\n")

                # fit DT model to the training dataset
                clf = tree.DecisionTreeClassifier()
                clf.fit(training_data, training_icc)
        
                if num == 0:
                    tree.export_graphviz(clf, out_file=folder_name + metric_set + '.tree3.dot', max_depth=3, class_names=['non-parallelizable','parallelizable'], feature_names=ppar.metric_sets[metric_set], filled=True, rounded=True)
                    tree.export_graphviz(clf, out_file=folder_name + metric_set + '.tree5.dot', max_depth=5, class_names=['non-parallelizable','parallelizable'], feature_names=ppar.metric_sets[metric_set], filled=True, rounded=True)
                
                predictions = clf.predict(testing_data)

                # calculate performance
                mispredicted_loops = set()
                predicted_loops = set()

                mispredicts = 0
                safe_mispredicts = 0
                unsafe_mispredicts = 0
                for i in range(0,len(predictions)):
                    pred = predictions[i]
                    icc = testing_icc[i]

                    if pred != icc:
                        # misprediction
                        mispredicts += 1
                        if pred == 1:
                            unsafe_mispredicts += 1
                            mispredicted_loops.add(str(testing_loops[i]) + ":" + str(predictions[i]) + ":" + str(testing_icc[i]) + ":unsafe")
                        else:
                            safe_mispredicts += 1
                            mispredicted_loops.add(str(testing_loops[i]) + ":" + str(predictions[i]) + ":" + str(testing_icc[i]) + ":safe")
                    else:
                        predicted_loops.add(str(testing_loops[i]) + ":" + str(predictions[i]) + ":" + str(testing_icc[i]))

                rate = 1 - mispredicts/len(predictions)
                accuracy_rates.append(rate)

                rate = mispredicts/len(predictions)
                error_rates.append(rate)

                if mispredicts != 0:
                    rate = safe_mispredicts/mispredicts
                    safe_mispredict_rates.append(rate)

                    rate = unsafe_mispredicts/mispredicts
                    unsafe_mispredict_rates.append(rate)
                else:
                    rate = 0
                    safe_mispredict_rates.append(rate)
                    unsafe_mispredict_rates.append(rate)

                # calculate prediction accuracy 
                accuracy = accuracy_score(testing_icc, clf.predict(testing_data))
                accuracy_samples.append(accuracy)
            
                report_file.write("Mispredictions: " + str(mispredicts) + "\n")
                report_file.write("\n")

                report_file.write("Accuracy" + "\n")
                report_file.write("========" + "\n")
                report_file.write("dt-accuracy: " + "{0:.2f}".format(accuracy) + "\n")
                report_file.write("accuracy: " + "{0:.2f}".format(accuracy_rates[num]) + "\n")
                report_file.write("mispredicts: " + "{0:.2f}".format(error_rates[num]) + "\n")
                report_file.write("safe-mispredicts: " + "{0:.2f}".format(safe_mispredict_rates[num]) + "\n")
                report_file.write("unsafe-mispredicts: " + "{0:.2f}".format(unsafe_mispredict_rates[num]) + "\n")
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
                
                num = num + 1
                # for train, train

            report_file.write("Mean values\n")
            report_file.write("===================" + "\n")
            report_file.write("mean-dt-accuracy:" + "{0:.2f}".format(pd.Series(accuracy_samples).mean()*100) + "\n")
            report_file.write("mean-accuracy:" + "{0:.2f}".format(pd.Series(accuracy_rates).mean()*100) + "\n")
            report_file.write("mean-error:" + "{0:.2f}".format(pd.Series(error_rates).mean()*100) + "\n")
            report_file.write("mean-safe-mispredicts-rate:" + "{0:.2f}".format(pd.Series(safe_mispredict_rates).mean()*100) + "\n")
            report_file.write("mean-unsafe-mispredicts-rate:" + "{0:.2f}".format(pd.Series(unsafe_mispredict_rates).mean()*100) + "\n")
            report_file.write("===================" + "\n")

            report_file.write("\n")
