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
    # prepare statistical learning labels 
    loop_classifications = data['labels']
    # prepare loop icc answers
    loop_icc_answers = data['icc']

    # prepare statistical learning features 
    features = data.drop(['loop-location','labels','icc'], axis=1)
    
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
        loop_classifications = loop_classifications[filtered_idx]
        loop_icc_answers = loop_icc_answers[filtered_idx]
        idxs_to_drop = features.index.drop(filtered_idx)
        features = features.drop(idxs_to_drop)

        loop_locations = loop_locations.reset_index(drop=True)
        loop_classifications = loop_classifications.reset_index(drop=True)
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

            mean_case_00_num = []
            mean_case_01_num = []
            mean_case_10_num = []
            mean_case_11_num = []
                
            mean_case_00_par = []
            mean_case_01_par = []
            mean_case_10_par = []
            mean_case_11_par = []
                
            mean_case_00_npar = []
            mean_case_01_npar = []
            mean_case_10_npar = []
            mean_case_11_npar = []

            for train, test in rkf.split(dataset):
                report_file.write("=== Split: " + str(num)+ " ===\n")
                report_file.write("==============================================================" + "\n")

                # prepare label vectors
                training_data = dataset.iloc[train]
                testing_data = dataset.iloc[test]
                training_labels = loop_classifications.iloc[train]
                testing_labels = loop_classifications.iloc[test]
                training_loops = loop_locations.iloc[train]
                testing_loops = loop_locations.iloc[test]
                training_icc = loop_icc_answers.iloc[train]
                testing_icc = loop_icc_answers.iloc[test]

                testing_labels = testing_labels.reset_index(drop=True)
                testing_loops = testing_loops.reset_index(drop=True)
                testing_icc = testing_icc.reset_index(drop=True)

                report_file.write("Training data size: " + str(len(training_data)) + "\n")
                report_file.write("Testing data size: " + str(len(testing_data)) + "\n")

                # fit DT model to the training dataset
                clf = tree.DecisionTreeClassifier()
                clf.fit(training_data, training_labels)
        
                if num == 0:
                    tree.export_graphviz(clf, out_file=folder_name + metric_set + '.tree3.dot', max_depth=3, class_names=['non-parallelizable','parallelizable'], feature_names=ppar.metric_sets[metric_set], filled=True, rounded=True)
                    tree.export_graphviz(clf, out_file=folder_name + metric_set + '.tree5.dot', max_depth=5, class_names=['non-parallelizable','parallelizable'], feature_names=ppar.metric_sets[metric_set], filled=True, rounded=True)
                
                predictions = clf.predict(testing_data)

                # calculate performance
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
                    test = testing_labels[i]
                    icc = testing_icc[i]

                    if icc == 0:
                        if pred == 0:
                            case_00_num += 1
                            if test == 0:
                                case_00_npar += 1
                                cases_00_npar.add(str(testing_loops[i]))
                            elif test == 1:
                                case_00_par += 1
                                cases_00_par.add(str(testing_loops[i]))
                        elif pred == 1:
                            case_01_num += 1
                            if test == 0:
                                case_01_npar += 1
                                cases_01_npar.add(str(testing_loops[i]))
                            elif test == 1:
                                case_01_par += 1
                                cases_01_par.add(str(testing_loops[i]))
                    elif icc == 1:
                        if pred == 0:
                            case_10_num += 1
                            if test == 0:
                                case_10_npar += 1
                                cases_10_npar.add(str(testing_loops[i]))
                            elif test == 1:
                                case_10_par += 1
                                cases_10_par.add(str(testing_loops[i]))
                        elif pred == 1:
                            case_11_num += 1
                            if test == 0:
                                case_11_npar += 1
                                cases_11_npar.add(str(testing_loops[i]))
                            elif test == 1:
                                case_11_par += 1
                                cases_11_par.add(str(testing_loops[i]))
 

                    if pred != test:
                        # misprediction
                        mispredicts += 1
                        if pred == 1:
                            unsafe_mispredicts += 1
                            mispredicted_loops.add(str(testing_loops[i]) + ":" + str(predictions[i]) + ":" + str(testing_labels[i]) + ":unsafe")
                        else:
                            safe_mispredicts += 1
                            mispredicted_loops.add(str(testing_loops[i]) + ":" + str(predictions[i]) + ":" + str(testing_labels[i]) + ":safe")
                    else:
                        predicted_loops.add(str(testing_loops[i]) + ":" + str(predictions[i]) + ":" + str(testing_labels[i]))

                    if pred != icc:
                        if pred == test:
                            icc_wins += 1
                            icc_win_loops.add(str(testing_loops[i]) + ":" + str(predictions[i]) + ":" + str(testing_icc[i]) + ":win")
                        else:
                            icc_losses += 1
                            icc_loss_loops.add(str(testing_loops[i]) + ":" + str(predictions[i]) + ":" + str(testing_icc[i]) + ":loss")

                mean_case_00_num.append(case_00_num)
                mean_case_01_num.append(case_01_num)
                mean_case_10_num.append(case_10_num)
                mean_case_11_num.append(case_11_num)

                mean_case_00_par.append(case_00_par)
                mean_case_01_par.append(case_01_par)
                mean_case_10_par.append(case_10_par)
                mean_case_11_par.append(case_11_par)

                mean_case_00_npar.append(case_00_npar)
                mean_case_01_npar.append(case_01_npar)
                mean_case_10_npar.append(case_10_npar)
                mean_case_11_npar.append(case_11_npar)

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

                icc_disagreements = icc_wins + icc_losses
                if icc_disagreements != 0:
                    # wins
                    rate = icc_wins/icc_disagreements
                    icc_win_rates.append(rate)
                    # losses
                    rate = icc_losses/icc_disagreements
                    icc_loss_rates.append(rate)
                else:
                    rate = 0
                    icc_win_rates.append(rate)
                    icc_loss_rates.append(rate)

                # calculate prediction accuracy 
                accuracy = accuracy_score(testing_labels, clf.predict(testing_data))
                accuracy_samples.append(accuracy)
            
                report_file.write("Mispredictions: " + str(mispredicts) + "\n")
                report_file.write("\n")

                report_file.write("Accuracy" + "\n")
                report_file.write("========" + "\n")
                report_file.write("tests: " + "{}".format(tests) + "\n")
                report_file.write("mispredicts: " + "{}".format(mispredicts) + "\n")
                report_file.write("accuracy: " + "{0:.2f}".format(accuracy) + "\n")
                report_file.write("accuracy-rate: " + "{0:.2f}".format(accuracy_rates[num]) + "\n")
                report_file.write("error-rate: " + "{0:.2f}".format(error_rates[num]) + "\n")
                report_file.write("safe-mispredicts: " + "{}".format(safe_mispredicts) + "\n")
                report_file.write("unsafe-mispredicts: " + "{}".format(unsafe_mispredicts) + "\n")
                report_file.write("safe-error-rate: " + "{0:.2f}".format(safe_mispredict_rates[num]) + "\n")
                report_file.write("unsafe-error-rate: " + "{0:.2f}".format(unsafe_mispredict_rates[num]) + "\n")
                report_file.write("========" + "\n")
                report_file.write("\n")
                
                report_file.write("ICC competition" + "\n")
                report_file.write("========" + "\n")
                report_file.write("icc-discrepancies: " + "{}".format(icc_disagreements) + "\n")
                report_file.write("icc-wins: " + "{}".format(icc_wins) + "\n")
                report_file.write("icc-losses: " + "{}".format(icc_losses) + "\n")
                report_file.write("win-rate: " + "{0:.2f}".format(icc_win_rates[num]) + "\n")
                report_file.write("loss-rate: " + "{0:.2f}".format(icc_loss_rates[num]) + "\n")
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
                
                num = num + 1
                # for train, train

            report_file.write("Mean values\n")
            report_file.write("===================" + "\n")
            report_file.write("mean-dt-accuracy:" + "{0:.2f}".format(pd.Series(accuracy_samples).mean()*100) + "\n")
            report_file.write("accuracy:" + "{0:.2f}".format(pd.Series(accuracy_rates).mean()*100) + "\n")
            report_file.write("error:" + "{0:.2f}".format(pd.Series(error_rates).mean()*100) + "\n")
            report_file.write("mean-safe-mispredicts-rate:" + "{0:.2f}".format(pd.Series(safe_mispredict_rates).mean()*100) + "\n")
            report_file.write("mean-unsafe-mispredicts-rate:" + "{0:.2f}".format(pd.Series(unsafe_mispredict_rates).mean()*100) + "\n")
            report_file.write("mean-icc-win-rate:" + "{0:.2f}".format(pd.Series(icc_win_rates).mean()*100) + "\n")
            report_file.write("mean-icc-loss-rate:" + "{0:.2f}".format(pd.Series(icc_loss_rates).mean()*100) + "\n")
            report_file.write("===================" + "\n")
            report_file.write("mean-case-00-num:" + "{}".format(pd.Series(mean_case_00_num).mean()) + "\n")
            report_file.write("mean-case-00-par:" + "{}".format(pd.Series(mean_case_00_par).mean()) + "\n")
            report_file.write("mean-case-00-npar:" + "{}".format(pd.Series(mean_case_00_npar).mean()) + "\n")
            report_file.write("mean-case-01-num:" + "{}".format(pd.Series(mean_case_01_num).mean()) + "\n")
            report_file.write("mean-case-01-par:" + "{}".format(pd.Series(mean_case_01_par).mean()) + "\n")
            report_file.write("mean-case-01-npar:" + "{}".format(pd.Series(mean_case_01_npar).mean()) + "\n")
            report_file.write("mean-case-10-num:" + "{}".format(pd.Series(mean_case_10_num).mean()) + "\n")
            report_file.write("mean-case-10-par:" + "{}".format(pd.Series(mean_case_10_par).mean()) + "\n")
            report_file.write("mean-case-10-npar:" + "{}".format(pd.Series(mean_case_10_npar).mean()) + "\n")
            report_file.write("mean-case-11-num:" + "{}".format(pd.Series(mean_case_11_num).mean()) + "\n")
            report_file.write("mean-case-11-par:" + "{}".format(pd.Series(mean_case_11_par).mean()) + "\n")
            report_file.write("mean-case-11-npar:" + "{}".format(pd.Series(mean_case_11_npar).mean()) + "\n")
            report_file.write("===================" + "\n")

            report_file.write("\n")
