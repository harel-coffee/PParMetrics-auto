#!/usr/bin/python3

import sys
import os

import pandas as pd
from sklearn import preprocessing

from sklearn.preprocessing import StandardScaler
from sklearn.preprocessing import MinMaxScaler
from sklearn.preprocessing import Normalizer

from sklearn import svm
from sklearn.metrics import accuracy_score

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

    gs = None
    # model hyper-parameter search
    if hp_opt == "opt":
        #X_train, X_valid, y_train, y_valid = train_test_split(train_features, train_par_labels, test_size=0.2, random_state=0)
        # hyper-parameter space to search
        param_grid = [ {'kernel': ['rbf'], 'gamma': [1e-3, 1e-4],'C': [1, 10, 100, 1000]},
                       {'kernel': ['linear'], 'C': [1, 10, 100, 1000]} ]
        
        gs = GridSearchCV(svm.SVC(), param_grid, cv=4, n_jobs=4)
        gs.fit(train_features, train_par_labels)
        print(gs.best_params_)

    # do the ICC competition experiment for all metric sets
    for metric_set in ppar.metric_sets:
        # create and open report file
        report_filename = report_folder + metric_set + ".report"
        report_file = open(report_filename,'w')
        # print the header into the report file
        print(metric_set + '.report', file=report_file)
        print("SVC [ " + metric_set + " ]")
        report_file.write(metric_set + " ") 
        report_file.write("[") 
        for metric in ppar.metric_sets[metric_set]: 
            report_file.write(" " + str(metric))
        report_file.write(" ]\n") 
        # select train and test datasets
        train_dataset = train_features[ppar.metric_sets[metric_set]]
        test_dataset = test_features[ppar.metric_sets[metric_set]]

        # fit SVM model to the training dataset
        clf = svm.SVC(**gs.best_params_)
        print(clf.get_params())
        clf.fit(train_dataset, train_par_labels)

        predictions = clf.predict(test_dataset)
        accuracy = accuracy_score(test_par_labels, clf.predict(test_dataset))
        print(str(accuracy))
        report_file.write("accuracy: " + "{0:.2f}".format(accuracy) + "\n")
        report_file.close()
