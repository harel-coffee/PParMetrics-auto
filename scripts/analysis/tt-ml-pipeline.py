#!/usr/bin/python3

import sys
import os

import configparser

import pandas as pd
import numpy as np

# preprocessing
from sklearn import preprocessing
from sklearn.preprocessing import StandardScaler
from sklearn.preprocessing import MinMaxScaler
from sklearn.preprocessing import Normalizer

# feature selection
from sklearn.feature_selection import VarianceThreshold
from sklearn.feature_selection import SelectKBest
from sklearn.feature_selection import SelectFromModel
from sklearn.feature_selection import chi2

# model selection
from sklearn.model_selection import train_test_split
from sklearn.model_selection import GridSearchCV

# models
from sklearn import svm
from sklearn.dummy import DummyClassifier
from sklearn.ensemble import RandomForestClassifier

# scoring metrics
from sklearn.metrics import accuracy_score
from sklearn.metrics import balanced_accuracy_score
from sklearn.metrics import precision_score
from sklearn.metrics import recall_score
from sklearn.metrics import f1_score

# auxiliary
from scipy.stats import pearsonr

import ppar
import ml_pipeline_config 

def preprocess_features(cfg, report_fd, train_features, test_features):

    preprocess_cfg = cfg['preprocess']
    report_cfg = cfg['report']

    report_fd.write("= [1] Feature Preprocessing Stage =" + "\n")
    report_fd.write("= =============================== =" + "\n")

    scaler = None
    if preprocess_cfg['scaler'] == "std":
        report_fd.write("scaler: StandardScaler( ")
        
        with_mean=False
        with_std=False
        if preprocess_cfg['scale'] == "true":
            report_fd.write("scale=True ")
            with_std = True
        if preprocess_cfg['center'] == "true":
            report_fd.write("mean=True ")
            with_mean = True
        report_fd.write(")\n")
        scaler = StandardScaler(with_mean, with_std)

    elif preprocess_cfg['scaler'] == "norm":
        scaler = Normalizer()
    elif preprocess_cfg['scaler'] == "min_max":
        report_fd.write("scaler: MinMaxScaler()" + "\n")
        scaler = MinMaxScaler()

    if preprocess_cfg['scaler'] != "raw":
        scaler.fit(train_features[:])
        train_features[:] = scaler.transform(train_features[:])
        test_features[:] = scaler.transform(test_features[:])
    else:
        report_fd.write("scaler: raw" + "\n")
    
    report_fd.write("= =============================== =" + "\n")

def select_features(cfg, report_fd, train_features, test_features, train_labels):

    select_cfg = cfg['feature_selection']
    report_cfg = cfg['report']
   
    verbose = int(report_cfg['feature_select_verbose'])

    if verbose > 0:
        report_fd.write("= [2] Feature Selection Stage =" + "\n")
        report_fd.write("= =========================== =" + "\n")

    # perform automatic feature selection
    if select_cfg['auto'] != 'true':
        if verbose > 0:
            report_fd.write("No automatic feature selection is done!" + "\n")
            report_fd.write("= =========================== =" + "\n")
        return True
 
    # determine feature selection method
    methods = select_cfg['methods'].split(",")
    models = select_cfg['models'].split(",")
    medians = select_cfg['medians'].split(",")
    thresholds = select_cfg['thresholds'].split(",")

    if verbose > 0:
        report_fd.write(str(len(methods)) + " selectors specified" + "\n")

    # apply all feature selection methods one after the other
    for i in range(0,len(methods)):
        # get feature selection parameters
        method = methods[i]
        model = models[i]
        thd = thresholds[i]
        median = medians[i]
        # pick the right selector
        selector = None
        if method == 'VarianceThreshold':
            if verbose > 0:
                report_fd.write("[ VarianceThreshold ]" + "\n")
            if thd == '':
                thd=0.16
            else:
                thd=float(thd)
            if verbose > 1:
                report_fd.write("threshold: " + str(thd) + "\n")
        
            selector = VarianceThreshold(threshold=thd)
       
        elif method == 'SelectFromModel':
            if verbose > 0:
                report_fd.write("[ SelectFromModel ]" + "\n")
            if median == '':
                median=1.0
            else:
                median=float(median)
            if verbose > 1:
                report_fd.write("median: " + str(median) + "\n")
       
            clf = None
            if model == 'RFC':
                clf = RandomForestClassifier(n_estimators=100)
            elif model == 'SVC':
                clf = SVC()
            else:
                sys.exit("error: feature selection: " + "method " + str(i) + "has unrecognised model: " + str(model))

            if verbose > 1:
                report_fd.write("model: " + str(model) + "\n")
            
            selector = SelectFromModel(clf, threshold=str(median)+'*median')

        elif method == 'SelectKBest':
            if verbose > 0:
                report_fd.write("[ SelectKBest ]" + "\n")
            if median == '':
                median=1.0
            else:
                median=float(median)
            if verbose > 1:
                report_fd.write("median: " + str(median) + "\n")
      
            feature_num = len(train_features.columns)
            k_best = float(feature_num)*median/2
            k_best = int(k_best)

            if model == '':
                selector = SelectKBest(k=k_best)
            else:
                sys.exit("error: feature selection: " + "method " + str(i) + "has unrecognised model: " + str(model))

            if verbose > 1:
                report_fd.write("model: " + str(model) + "\n")

        else:
            sys.exit("error: feature selection: " + "unrecognised selection method: " + method)

        selector.fit(train_features, train_labels)
        selected_cols = selector.get_support(indices=True)
        dropped_cols = np.invert(selector.get_support())
        dropped_cols = np.where(dropped_cols)[0]
            
        selected_feature_names = (train_features.columns[selected_cols]).tolist()
        dropped_feature_names = (train_features.columns[dropped_cols]).tolist()
        if verbose > 2:
            report_fd.write("selected features num: " + str(len(selected_cols)) + "\n")
            report_fd.write("dropped features num: " + str(len(dropped_cols)) + "\n")
            report_fd.write("dropped features [" + "\n")
            for feature in dropped_feature_names:
                report_fd.write("\t" + feature + "\n")
            report_fd.write("]" + "\n")
          
        train_features.drop(train_features.columns[dropped_cols],axis=1,inplace=True)
        test_features.drop(test_features.columns[dropped_cols],axis=1,inplace=True)

    if verbose > 0:
        report_fd.write("= Final feature selection =" + "\n")
    selected_feature_names = (train_features.columns).tolist()
    if verbose > 0:
        report_fd.write("selected features num: " + str(len(selected_feature_names)) + "\n")
    if verbose > 1:
        report_fd.write("selected features [" + "\n")
        for feature in selected_feature_names:
            report_fd.write("\t" + feature + "\n")
        report_fd.write("]" + "\n")

    if verbose > 0:
        report_fd.write("= =========================== =" + "\n")

def hyper_param_selection(cfg, report_fd, train_features, test_features, train_labels):

    hp_cfg = cfg['hyper_param_selection']
    report_cfg = cfg['report']

    verbose = int(report_cfg['hyper_param_select_verbose'])

    if verbose > 0:
        report_fd.write("= [3] Model Hyper-Parameter Selection Stage =" + "\n")
        report_fd.write("= ========================================= =" + "\n")

    # perform automatic hyper-parameter selection
    if hp_cfg['auto'] != 'true':
        if verbose > 0:
            report_fd.write("= No automatic model hyper-parameter selection is done! =" + "\n")
            report_fd.write("= ========================================= =" + "\n")
        return None
    
    if hp_cfg['model'] == 'SVC':
        gs = None
      
        if verbose > 1:
            report_fd.write("model: " + hp_cfg['model'] + "\n")
        
        kernels = hp_cfg['kernel'].split(",")
        gammas = [ float(x) for x in hp_cfg['gamma'].split(",") ] 
        Cs = [ int(x) for x in hp_cfg['C'].split(",") ]
       
        if verbose > 1:
            report_fd.write("= hyper-parameter grid search space =\n")
            report_fd.write("kernels: ")
            for kernel in kernels:
                report_fd.write(kernel)
                report_fd.write(" ")
            report_fd.write("\n")

            report_fd.write("gammas: ")
            for gamma in gammas:
                report_fd.write(str(gamma))
                report_fd.write(" ")
            report_fd.write("\n")

            report_fd.write("Cs: ")
            for C in Cs:
                report_fd.write(str(C))
                report_fd.write(" ")
            report_fd.write("\n")
            report_fd.write("= =\n")

        # recover hyper-parameter search space from config file
        hyper_param_grid = []
        for kernel in kernels:
            if kernel != 'linear':
                hyper_param_grid.append({'kernel': [kernel], 'gamma': gammas, 'C': Cs})
            else:
                hyper_param_grid.append({'kernel': [kernel], 'C': Cs})
    
        gs = GridSearchCV(svm.SVC(), hyper_param_grid, cv=4, n_jobs=4, scoring='balanced_accuracy')
        gs.fit(train_features, train_labels)
        if verbose > 0:
            report_fd.write(str(gs.best_params_) + "\n")
        return gs.best_params_
    
    if verbose > 0:
        report_fd.write("= ========================================= =" + "\n")

def model_training(cfg, report_fd, hparams, train_features, train_labels):

    train_cfg = cfg['model_training']
    report_cfg = cfg['report']

    verbose = int(report_cfg['model_train_verbose'])

    if verbose > 0:
        report_fd.write("= [4] Model Training Stage =" + "\n")
        report_fd.write("= ======================== =" + "\n")
   
    # select train and test datasets
    train_dataset = train_features

    trained_models = {}
    trained_models['baseline'] = None
    trained_models[train_cfg['model']] = None

    # first, deploy random predictors and measure their accuracy
    dummy_clf = DummyClassifier(strategy='most_frequent')
    dummy_clf.fit(train_dataset, train_labels)
    trained_models['baseline'] = dummy_clf

    clf = None
    if train_cfg['model'] == 'SVC':
        clf = svm.SVC(**hparams)
    
    clf.fit(train_dataset, train_labels)
    trained_models[train_cfg['model']] = clf
    
    if verbose > 0:
        report_fd.write("= ======================== =" + "\n")

    return trained_models

def model_testing(cfg, report_fd, clfs, test_features):

    test_cfg = cfg['model_testing']
    report_cfg = cfg['report']

    verbose = int(report_cfg['model_test_verbose'])

    if verbose > 0:
        report_fd.write("= [5] Model Testing Stage =" + "\n")
        report_fd.write("= ========================== =" + "\n")

    predictions = {}

    predictions['baseline'] = clfs['baseline'].predict(test_features)
    if verbose > 0:
        report_fd.write("baseline tested" + "\n")

    predictions[test_cfg['model']] = clfs[test_cfg['model']].predict(test_features)
    if verbose > 0:
        report_fd.write(test_cfg['model'] + " tested" + "\n")
    
    if verbose > 0:
        report_fd.write("= ========================== =" + "\n")
    
    return predictions

def report_results(cfg, report_fd, predictions, test_loop_locations, test_par_labels, test_icc_labels):

    report_cfg = cfg['report']
    test_cfg = cfg['model_testing']

    verbose = int(report_cfg['report_verbose'])

    if verbose > 0:
        report_fd.write("= [6] Model Report Stage =" + "\n")
        report_fd.write("= ====================== =" + "\n")

    if report_cfg['baseline'] == 'true':
        preds = predictions['baseline']

        report_fd.write("= SciKitLearn Dummy (Baseline) Predictor =" + "\n")
        report_fd.write("= =========================== =" + "\n")
        
        report_fd.write("type: " + "most frequent" + "\n")
        
        accuracy = accuracy_score(test_par_labels, preds)
        report_fd.write("accuracy score: " + "{0:.3f}".format(accuracy) + "\n")

        accuracy = balanced_accuracy_score(test_par_labels, preds)
        report_fd.write("balanced accuracy score: " + "{0:.3f}".format(accuracy) + "\n")

        accuracy = precision_score(test_par_labels, preds)
        report_fd.write("precision score: " + "{0:.3f}".format(accuracy) + "\n")

        accuracy = f1_score(test_par_labels, preds)
        report_fd.write("f1 score: " + "{0:.3f}".format(accuracy) + "\n")

        accuracy = recall_score(test_par_labels, preds)
        report_fd.write("recall score: " + "{0:.3f}".format(accuracy) + "\n")
        report_fd.write("= =========================== =" + "\n")
        report_fd.write("\n")

    if report_cfg['main'] == 'true':
        preds = predictions[cfg['model_testing']['model']]
    
        report_fd.write("= SciKitLearn Main Score Report =" + "\n")
        report_fd.write("= ============================= =" + "\n")
        accuracy = accuracy_score(test_par_labels, preds)
        print("accuracy score: " + "{0:.3f}".format(accuracy))
        report_fd.write("accuracy score: " + "{0:.3f}".format(accuracy) + "\n")

        accuracy = balanced_accuracy_score(test_par_labels, preds)
        print("balanced accuracy score: " + "{0:.3f}".format(accuracy))
        report_fd.write("balanced accuracy score: " + "{0:.3f}".format(accuracy) + "\n")

        accuracy = precision_score(test_par_labels, preds)
        print("precision score: " + "{0:.3f}".format(accuracy))
        report_fd.write("precision score: " + "{0:.3f}".format(accuracy) + "\n")

        accuracy = f1_score(test_par_labels, preds)
        print("f1 score: " + "{0:.3f}".format(accuracy))
        report_fd.write("f1 score: " + "{0:.3f}".format(accuracy) + "\n")

        accuracy = recall_score(test_par_labels, preds)
        print("recall score: " + "{0:.3f}".format(accuracy))
        report_fd.write("recall score: " + "{0:.3f}".format(accuracy) + "\n")
        report_fd.write("= ======================== =" + "\n")
        report_fd.write("\n")
    
    if report_cfg['icc_competition'] == 'true':
        preds = predictions[cfg['model_testing']['model']]

        # calculate feedback scheme performance
        mispredicted_loops = set()
        predicted_loops = set()
        icc_win_loops = set()
        icc_loss_loops = set()

        tests = len(preds)
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
            pred = preds[i]
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
                    mispredicted_loops.add(str(test_loop_locations[i]) + ":" + str(preds[i]) + ":" + str(test_par_labels[i]) + ":unsafe")
                else:
                    safe_mispredicts += 1
                    mispredicted_loops.add(str(test_loop_locations[i]) + ":" + str(preds[i]) + ":" + str(test_par_labels[i]) + ":safe")
            else:
                predicted_loops.add(str(test_loop_locations[i]) + ":" + str(preds[i]) + ":" + str(test_par_labels[i]))

            if pred != icc:
                if pred == par:
                    icc_wins += 1
                    icc_win_loops.add(str(test_loop_locations[i]) + ":" + str(preds[i]) + ":" + str(test_icc_labels[i]) + ":win")
                else:
                    icc_losses += 1
                    icc_loss_loops.add(str(test_loop_locations[i]) + ":" + str(preds[i]) + ":" + str(test_icc_labels[i]) + ":loss")
     
        # Table
        # 0 0
        # 0 1
        # 1 0
        # 1 1
        report_fd.write("Table" + "\n")
        report_fd.write("========" + "\n")
        report_fd.write("=== case [0 / 0] ===" + "\n")
        report_fd.write("num: " + "{}".format(case_00_num) + "\n")
        report_fd.write("par: " + "{}".format(case_00_par) + "\n")
        report_fd.write("non-par: " + "{}".format(case_00_npar) + "\n")
        report_fd.write("=== case [0 / 1] ===" + "\n")
        report_fd.write("num: " + "{}".format(case_01_num) + "\n")
        report_fd.write("par: " + "{}".format(case_01_par) + "\n")
        report_fd.write("non-par: " + "{}".format(case_01_npar) + "\n")
        report_fd.write("=== case [1 / 0] ===" + "\n")
        report_fd.write("num: " + "{}".format(case_10_num) + "\n")
        report_fd.write("par: " + "{}".format(case_10_par) + "\n")
        report_fd.write("non-par: " + "{}".format(case_10_npar) + "\n")
        report_fd.write("=== case [1 / 1] ===" + "\n")
        report_fd.write("num: " + "{}".format(case_11_num) + "\n")
        report_fd.write("par: " + "{}".format(case_11_par) + "\n")
        report_fd.write("non-par: " + "{}".format(case_11_npar) + "\n")
        report_fd.write("========" + "\n")
        report_fd.write("\n")

        if report_cfg['loop_locations'] == 'true':
            report_fd.write("=== case [0 / 0] ===" + "\n")
            report_fd.write("= parallelisible =" + "\n")
            for loop in cases_00_par:
                report_fd.write(loop + "\n")
            report_fd.write("= non-parallelisible =" + "\n")
            for loop in cases_00_npar:
                report_fd.write(loop + "\n")
            report_fd.write("===================" + "\n")
            report_fd.write("\n")

            report_fd.write("=== case [0 / 1] ===" + "\n")
            report_fd.write("= parallelisible =" + "\n")
            for loop in cases_01_par:
                report_fd.write(loop + "\n")
            report_fd.write("= non-parallelisible =" + "\n")
            for loop in cases_01_npar:
                report_fd.write(loop + "\n")
            report_fd.write("===================" + "\n")
            report_fd.write("\n")

            report_fd.write("=== case [1 / 0] ===" + "\n")
            report_fd.write("= parallelisible =" + "\n")
            for loop in cases_10_par:
                report_fd.write(loop + "\n")
            report_fd.write("= non-parallelisible =" + "\n")
            for loop in cases_10_npar:
                report_fd.write(loop + "\n")
            report_fd.write("===================" + "\n")
            report_fd.write("\n")

            report_fd.write("=== case [1 / 1] ===" + "\n")
            report_fd.write("= parallelisible =" + "\n")
            for loop in cases_11_par:
                report_fd.write(loop + "\n")
            report_fd.write("= non-parallelisible =" + "\n")
            for loop in cases_11_npar:
                report_fd.write(loop + "\n")
            report_fd.write("===================" + "\n")
            report_fd.write("\n")

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

        report_fd.write("Mispredictions: " + str(mispredicts) + "\n")
        report_fd.write("\n")

        report_fd.write("Accuracy" + "\n")
        report_fd.write("========" + "\n")
        report_fd.write("tests: " + "{}".format(tests) + "\n")
        report_fd.write("mispredicts: " + "{}".format(mispredicts) + "\n")
        report_fd.write("accuracy-rate: " + "{0:.2f}".format(accuracy_rate) + "\n")
        report_fd.write("error-rate: " + "{0:.2f}".format(error_rate) + "\n")
        report_fd.write("safe-mispredicts: " + "{}".format(safe_mispredicts) + "\n")
        report_fd.write("unsafe-mispredicts: " + "{}".format(unsafe_mispredicts) + "\n")
        report_fd.write("safe-error-rate: " + "{0:.2f}".format(safe_mispredict_rate) + "\n")
        report_fd.write("unsafe-error-rate: " + "{0:.2f}".format(unsafe_mispredict_rate) + "\n")
        report_fd.write("========" + "\n")
        report_fd.write("\n")
            
        report_fd.write("ICC competition" + "\n")
        report_fd.write("========" + "\n")
        report_fd.write("icc-discrepancies: " + "{}".format(icc_disagreements) + "\n")
        report_fd.write("icc-wins: " + "{}".format(icc_wins) + "\n")
        report_fd.write("icc-losses: " + "{}".format(icc_losses) + "\n")
        report_fd.write("win-rate: " + "{0:.2f}".format(icc_win_rate) + "\n")
        report_fd.write("loss-rate: " + "{0:.2f}".format(icc_loss_rate) + "\n")
        report_fd.write("========" + "\n")
        report_fd.write("\n")

        if report_cfg['loop_locations'] == 'true':
            report_fd.write("loop:pred:label:safe\n")
            
            report_fd.write("Mispredicted loops:" + "\n")
            report_fd.write("===================" + "\n")
            for loop_report in mispredicted_loops:
                report_fd.write(loop_report + "\n")
            report_fd.write("===================" + "\n")
          
            report_fd.write("\n")
            
            report_fd.write("Predicted loops:" + "\n")
            report_fd.write("===================" + "\n")
            for loop_report in predicted_loops:
                report_fd.write(loop_report + "\n")
            report_fd.write("===================" + "\n")

            report_fd.write("\n")

            report_fd.write("loop:pred:icc:result\n")
            
            report_fd.write("Win over ICC loops:" + "\n")
            report_fd.write("===================" + "\n")
            for loop_report in icc_win_loops:
                report_fd.write(loop_report + "\n")
            report_fd.write("===================" + "\n")
          
            report_fd.write("\n")
            
            report_fd.write("Loss to ICC loops:" + "\n")
            report_fd.write("===================" + "\n")
            for loop_report in icc_loss_loops:
                report_fd.write(loop_report + "\n")
            report_fd.write("===================" + "\n")

            report_fd.write("\n")

    if verbose > 0:
        report_fd.write("==============================================================" + "\n")
        report_fd.write("\n\n\n")


if __name__ == "__main__":

    print("================================================")
    print("=== Machine Learning Train Test Pipeline Run ===")
    print("================================================")

    if len(sys.argv) != 5:
        sys.exit("error: use ./train_test_pipeline.py " +
                 "[TRAIN data file]" + 
                 "[TEST data file]" + 
                 "[Pipeline Configuration INI file]" + 
                 "[REPORT folder]")

    # get command line arguments and check that provided files exist
    train_data_filename = sys.argv[1]
    test_data_filename = sys.argv[2]
    pipeline_cfg_filename = sys.argv[3]
    report_folder = sys.argv[4] + "/"

    if not os.path.exists(train_data_filename):
        sys.exit("error: " + train_data_filename + " TRAIN data file does not exist!")

    if not os.path.exists(test_data_filename):
        sys.exit("error: " + test_data_filename + " TEST data file does not exist!")

    if not os.path.exists(pipeline_cfg_filename):
        sys.exit("error: " + pipeline_cfg_filename + " Pipeline Configuration file does not exist!")

    if not os.path.exists(report_folder):
        sys.exit("error: " + report_folder + " REPORT folder does not exist!")

    print("TRAIN data: " + train_data_filename)
    print("TEST data: " + test_data_filename)
    print("ML Pipeline Config: " + pipeline_cfg_filename)
    print("REPORT folder: " + report_folder)
    print("================================================")

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
  
    ###
    # Parse ML Pipeline Configuration INI file
    ###
    pl_cfg = configparser.ConfigParser()
    pl_cfg.read(pipeline_cfg_filename)
    if ml_pipeline_config.check_ml_pipeline_config(pl_cfg) != True:
        sys.exit("error: " + pipeline_cfg_filename + " has a wrong format!")

    ###
    # Prepare and open Report file to write
    ###
    # create and open report file
    report_filename = report_folder + "ml_pipeline" + ".report"
    report_fd = open(report_filename,'w')
    # print the header into the report file
    report_fd.write("= ML Pipeline Report =\n") 
    report_fd.write("= ================== =\n") 
    report_fd.write("\n") 

    ###
    # Run ML Pipeline
    ###
    # basic feature preprocessing (scaling, normalizaion, etc.) stage 
    preprocess_features(pl_cfg, report_fd, train_features, test_features)
    # feature selection stage
    select_features(pl_cfg, report_fd, train_features, test_features, train_par_labels)
    # model hyper-parameter selection stage
    best_hparams = hyper_param_selection(pl_cfg, report_fd, train_features, test_features, train_par_labels)
    # train ML model
    trained_clfs = model_training(pl_cfg, report_fd, best_hparams, train_features, train_par_labels)
    # test ML model
    predictions = model_testing(pl_cfg, report_fd, trained_clfs, test_features)
    # report results
    report_results(pl_cfg, report_fd, predictions, test_loop_locations, test_par_labels, test_icc_labels)
    
    # close report file
    report_fd.close()

    sys.exit(0)
