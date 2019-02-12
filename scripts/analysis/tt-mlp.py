#!/usr/bin/python3

import sys
import os

import pandas as pd
from sklearn import preprocessing

from sklearn.preprocessing import StandardScaler
from sklearn.preprocessing import MinMaxScaler
from sklearn.preprocessing import Normalizer

from sklearn.neural_network import MLPClassifier
from sklearn.metrics import accuracy_score
from sklearn.metrics import balanced_accuracy_score
from sklearn.metrics import precision_score
from sklearn.metrics import recall_score
from sklearn.metrics import f1_score

from sklearn.model_selection import train_test_split
from sklearn.model_selection import GridSearchCV

import ppar


def preprocess_features(scale, train_features, test_features):
    
    scaler = None
    if scale == "std":
        scaler = StandardScaler()
    elif scale == "norm":
        scaler = Normalizer()
    elif scale == "minmax":
        scaler = MinMaxScaler()

    if scale != "raw":
        scaler.fit(train_features[:])
        train_features[:] = scaler.transform(train_features[:])
        test_features[:] = scaler.transform(test_features[:])

if __name__ == "__main__":

    print("==============================================================================")
    print("=== Machine Learning vs. Intel C/C++ Compiler (ICC) competition experiment ===")
    print("===               Multi-layer Perceptron Neural Network Model              ===")
    print("==============================================================================")

    if len(sys.argv) != 6:
        sys.exit("error: use tt.py [TRAIN dataset filename] [TEST dataset filename] [REPORT folder] [SCALE parameter] [HP search] as arguments!")

    # get command line arguments
    train_data_filename = sys.argv[1]
    test_data_filename = sys.argv[2]
    report_folder = sys.argv[3] + "/"
    scale = sys.argv[4]
    hp_opt = sys.argv[5]

    # check if command line passed files exist
    if not os.path.exists(train_data_filename):
        sys.exit("error: " + train_data_filename + " TRAIN data file does not exist!")
    if not os.path.exists(test_data_filename):
        sys.exit("error: " + test_data_filename + " TEST data file does not exist!")
    if not os.path.exists(report_folder):
        sys.exit("error: " + report_folder + " REPORT folder does not exist!")

    print("TRAIN data: " + train_data_filename)
    print("TEST data: " + test_data_filename)
    print("REPORT folder: " + report_folder)

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
    # cast all integer features to float
    train_features = train_features.astype('float64')

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
    # cast all integer features to float
    test_features = test_features.astype('float64')
   
    # feature preprocessing (scaling, normalizaion, etc.) 
    preprocess_features(scale, train_features, test_features)

    # do the ICC competition experiment for all metric sets
    for metric_set in ppar.metric_sets:
        
        # create and open report file
        report_filename = report_folder + metric_set + ".report"
        report_file = open(report_filename,'w')
        
        # print the header into the report file
        #print(metric_set + '.report', file=report_file)
        print("SVC [ " + metric_set + " ]")
        report_file.write("\n") 
        report_file.write(metric_set + " ") 
        report_file.write("[\n") 
        for metric in ppar.metric_sets[metric_set]: 
            report_file.write("\t" + str(metric) + "\n")
        report_file.write("]\n") 
        report_file.write("\n") 
        
        # select train and test datasets
        train_dataset = train_features[ppar.metric_sets[metric_set]]
        test_dataset = test_features[ppar.metric_sets[metric_set]]

        gs = None
        # model hyper-parameter search
        if hp_opt == "opt":
            #X_train, X_valid, y_train, y_valid = train_test_split(train_features, train_par_labels, test_size=0.2, random_state=0)
            # hyper-parameter space to search
            param_grid = [ {'activation': ['logistic','tanh'], 
                            'hidden_layer_sizes': [(5,5), (10,), (10,10), (10,5)], 
                            'solver': ['lbfgs'], 
                            #'solver': ['lbfgs', 'sgd', 'adam'], 
                            'alpha': [1e-2, 1e-1, 1, 10, 100], 
                            'random_state': [12883823]} ]
        
            gs = GridSearchCV(MLPClassifier(), param_grid, cv=4, n_jobs=4, scoring='balanced_accuracy')
            gs.fit(train_dataset, train_par_labels)
            print(gs.best_params_, file=report_file)
            print(gs.best_params_)

        report_file.write("\n") 
        # fit SVM model to the training dataset
        clf = None
        if hp_opt == "opt":
            clf = MLPClassifier(**gs.best_params_)
        else:
            random_state = 12883823
            clf = MLPClassifier(activation='logistic',solver='lbfgs', alpha=1e-3, hidden_layer_sizes=(10,10), random_state=random_state)
            #clf = MLPClassifier(activation='tanh',solver='lbfgs', alpha=1e-1, hidden_layer_sizes=(10,10), random_state=random_state)
            #clf = MLPClassifier(activation='tanh',solver='sgd', alpha=1e-3, hidden_layer_sizes=(5,5), random_state=random_state)
            #clf = MLPClassifier(activation='tanh',solver='adam', alpha=1e-3, hidden_layer_sizes=(5,5), random_state=random_state)
        clf.fit(train_dataset, train_par_labels)

        predictions = clf.predict(test_dataset)
        
        report_file.write("= SciKitLearn Score Report =" + "\n")
        report_file.write("= ======================== =" + "\n")
        accuracy = accuracy_score(test_par_labels, predictions)
        print("accuracy score: " + "{0:.3f}".format(accuracy))
        report_file.write("accuracy score: " + "{0:.3f}".format(accuracy) + "\n")

        accuracy = balanced_accuracy_score(test_par_labels, predictions)
        print("balanced accuracy score: " + "{0:.3f}".format(accuracy))
        report_file.write("balanced accuracy score: " + "{0:.3f}".format(accuracy) + "\n")

        accuracy = precision_score(test_par_labels, predictions)
        print("precision score: " + "{0:.3f}".format(accuracy))
        report_file.write("precision score: " + "{0:.3f}".format(accuracy) + "\n")

        accuracy = f1_score(test_par_labels, predictions)
        print("f1 score: " + "{0:.3f}".format(accuracy))
        report_file.write("f1 score: " + "{0:.3f}".format(accuracy) + "\n")

        accuracy = recall_score(test_par_labels, predictions)
        print("recall score: " + "{0:.3f}".format(accuracy))
        report_file.write("recall score: " + "{0:.3f}".format(accuracy) + "\n")
        report_file.write("= ======================== =" + "\n")
        report_file.write("\n")

        # calculate feedback scheme performance
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
