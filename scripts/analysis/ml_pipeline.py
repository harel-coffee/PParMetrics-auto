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
from sklearn.feature_selection import RFECV
from sklearn.feature_selection import chi2
from sklearn.feature_selection import mutual_info_classif
from sklearn.feature_selection import f_classif

# model selection
from sklearn.model_selection import train_test_split
from sklearn.model_selection import GridSearchCV
from sklearn.model_selection import KFold
from sklearn.model_selection import RepeatedKFold

# models
from sklearn.svm import SVC
from sklearn.tree import DecisionTreeClassifier
from sklearn.dummy import DummyClassifier
from sklearn.ensemble import RandomForestClassifier, AdaBoostClassifier
from sklearn.neural_network import MLPClassifier

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

_rand_state_ = 12883823

CLASS_IDX = 0
PROBABILITY_IDX = 1

def get_random_state():
    return _rand_state_

def preprocess_features(cfg, train_features, test_features,  ):
    
    preprocess_cfg = cfg['preprocess']

    scaler = None
    if preprocess_cfg['scaler'] == "std":
        with_mean=False
        with_std=False
        if preprocess_cfg['scale'] == "true":
            with_std = True
        if self.preprocess_cfg['center'] == "true":
            with_mean = True
        scaler = StandardScaler(with_mean, with_std)

    elif preprocess_cfg['scaler'] == "norm":
        scaler = Normalizer()
    
    elif preprocess_cfg['scaler'] == "min_max":
        scaler = MinMaxScaler()

    if preprocess_cfg['scaler'] != "raw":
        scaler.fit(train_features[:])
        train_features[:] = scaler.transform(train_features[:])
        test_features[:] = scaler.transform(test_features[:])
    else:
        self.report_fd.write("scaler: raw" + "\n")
    
    self.report_fd.write("= =============================== =" + "\n")
    self.report_fd.write("\n")

class MLPipeline:
    
    """ 
        Machine Learning (ML) Pipeline 

        Creates copies of passed training data and labels;
        Runs ML pipeline configured with the passed configuration file;
        Writes its status and reporting messages into the provided report
        file descriptor, as it goes; 
    """
    
    def __init__(self, cfg, report_fd, train_features, test_features, train_labels):
        
        # convinience pointers to all configuration file sections
        self.cfg = cfg
        self.header_cfg = cfg['header']
        self.preprocess_cfg = cfg['preprocess']
        self.select_cfg = cfg['feature_selection']
        self.hp_cfg = cfg['hyper_param_selection']
        self.train_cfg = cfg['model_training']
        self.test_cfg = cfg['model_testing']
        self.report_cfg = cfg['report']

        self.model_name = self.train_cfg['model']

        # file descriptor to write ML pipeline reporting
        self.report_fd = report_fd
        self.report_fd.write("= MLPipeline (" + str(self) + ") =" + "\n")

        # train features
        self.train_features = train_features.copy()
        # test features
        self.test_features = test_features.copy()
        # train labels
        self.train_labels = np.copy(train_labels)

        # data structures to be populated with the pipeline run results
       
        # feature selection result
        self.best_features = None
        # hyper-parameter selection result
        self.best_hparams = None
        # trained models
        self.trained_classifiers = {}
        self.trained_classifiers['baseline'] = {}
        for baseline_name in ml_pipeline_config.available_baselines:
            self.trained_classifiers['baseline'][baseline_name] = None 
        self.trained_classifiers[self.model_name] = None

    def run(self):
        self.preprocess_features()
        self.select_features()
        self.hyper_param_selection()
        self.model_training()

    #
    # predict() - takes in testing set features and returns 
    # prediction vectors
    #
    def predict(self):
        
        verbose = int(self.report_cfg['model_test_verbose'])

        if verbose > 0:
            self.report_fd.write("= MLPipeline (" + str(self) + ") Prediction =" + "\n")
            self.report_fd.write("= ========================== =" + "\n")
            self.report_fd.write("\n")

        # prepare predictions data structure  

        predictions = {}
        predictions['baseline'] = {}
        for baseline_name in ml_pipeline_config.available_baselines:
            predictions['baseline'][baseline_name] = [None, None]
        predictions[self.model_name] = [None, None]

        # perform actual predictions
        
        predictions[self.model_name][CLASS_IDX] = \
                self.trained_classifiers[self.model_name].predict(self.test_features)
        if self.model_name in ['DT','RFC','SVC','AdaBoost','MLP']:
            predictions[self.model_name][PROBABILITY_IDX] = \
                    self.trained_classifiers[self.model_name].predict_proba(self.test_features)
        
        for baseline_name in ml_pipeline_config.available_baselines:
            predictions['baseline'][baseline_name][CLASS_IDX] = \
                    self.trained_classifiers['baseline'][baseline_name].predict(self.test_features)
            if verbose > 0:
                self.report_fd.write(baseline_name + " baseline tested" + "\n")

        if verbose > 0:
            self.report_fd.write("= ========================== =" + "\n")
            self.report_fd.write("\n")
        
        return predictions

    def preprocess_features(self):
        
        self.report_fd.write("= MLPipeline (" + str(self) + "): [1] Feature Preprocessing Stage =" + "\n")
        self.report_fd.write("= =============================== =" + "\n")
        self.report_fd.write("\n")

        scaler = None
        if self.preprocess_cfg['scaler'] == "std":
            
            self.report_fd.write("scaler: StandardScaler( ")
            
            with_mean=False
            with_std=False
            if self.preprocess_cfg['scale'] == "true":
                self.report_fd.write("scale=True ")
                with_std = True
            if self.preprocess_cfg['center'] == "true":
                self.report_fd.write("mean=True ")
                with_mean = True
            self.report_fd.write(")\n")
            scaler = StandardScaler(with_mean, with_std)

        elif self.preprocess_cfg['scaler'] == "norm":
            self.report_fd.write("scaler: Normalizer()" + "\n")
            scaler = Normalizer()
        
        elif self.preprocess_cfg['scaler'] == "min_max":
            self.report_fd.write("scaler: MinMaxScaler()" + "\n")
            scaler = MinMaxScaler()

        if self.preprocess_cfg['scaler'] != "raw":
            self.report_fd.write("features fitted and transformed" + "\n")
            scaler.fit(self.train_features[:])
            self.train_features[:] = scaler.transform(self.train_features[:])
            self.test_features[:] = scaler.transform(self.test_features[:])
        else:
            self.report_fd.write("scaler: raw" + "\n")
        
        self.report_fd.write("= =============================== =" + "\n")
        self.report_fd.write("\n")
   
    def select_features(self):
   
        verbose = int(self.report_cfg['feature_select_verbose'])

        if verbose > 0:
            self.report_fd.write("= [2] Feature Selection Stage =" + "\n")
            self.report_fd.write("= =========================== =" + "\n")

        # perform automatic feature selection
        if self.select_cfg['auto'] != 'true':
            if verbose > 0:
                self.report_fd.write("No automatic feature selection is done!" + "\n")
                self.report_fd.write("= =========================== =" + "\n")
                self.report_fd.write("\n")
            return True
 
        # determine feature selection method
        methods = self.select_cfg['methods'].split(",")
        models = self.select_cfg['models'].split(",")
        medians = self.select_cfg['medians'].split(",")
        thresholds = self.select_cfg['thresholds'].split(",")
        scorings = self.select_cfg['scorings'].split(",")
        tests = self.select_cfg['tests'].split(",")

        if verbose > 0:
            self.report_fd.write("\n")
            self.report_fd.write(str(len(methods)) + " selectors specified" + "\n")
            self.report_fd.write("\n")

        # apply all feature selection methods one after the other
        for i in range(0,len(methods)):
            
            # get feature selection parameters
            method = methods[i]
            model = models[i]
            thd = thresholds[i]
            median = medians[i]
            score = scorings[i]
            
            # pick the right selector
            selector = None
            if method == 'VarianceThreshold':
                if verbose > 0:
                    self.report_fd.write("[ VarianceThreshold ]" + "\n")
                if thd == '':
                    thd=0.16
                else:
                    thd=float(thd)
                if verbose > 1:
                    self.report_fd.write("threshold: " + str(thd) + "\n")
            
                selector = VarianceThreshold(threshold=thd)
           
            elif method == 'SelectFromModel':
                if verbose > 0:
                    self.report_fd.write("[ SelectFromModel ]" + "\n")
                if median == '':
                    median=1.0
                else:
                    median=float(median)
                if verbose > 1:
                    self.report_fd.write("median: " + str(median) + "\n")
           
                clf = None
                if model == 'RFC':
                    n_est = 100
                    max_d = None
                    min_s_leaf = 1
                    min_s_split = 0.2
                    if 'n_estimators' in self.select_cfg:
                        n_est = int(self.select_cfg['n_estimators'])
                    if 'max_depth' in self.select_cfg:
                        max_d = int(self.select_cfg['max_depth'])
                    if 'min_samples_leaf' in self.select_cfg:
                        min_s_leaf = int(self.select_cfg['min_samples_leaf'])
                    if 'min_samples_split' in self.select_cfg:
                        min_s_split = int(self.select_cfg['min_samples_split'])
                 
                    clf = RandomForestClassifier(n_estimators=n_est, 
                            max_depth=max_d, 
                            min_samples_split=min_s_split, 
                            random_state=_rand_state_, 
                            min_samples_leaf=min_s_leaf)

                elif model == 'SVC':
                    clf = SVC(random_state=_rand_state_)
                
                elif model == 'DT':
                    clf = DecisionTreeClassifier(random_state=_rand_state_)
                
                else:
                    sys.exit("error: feature selection: " + "method " + str(i) + "has unrecognised model: " + str(model))

                if verbose > 1:
                    self.report_fd.write("model: " + str(model) + "\n")
                
                selector = SelectFromModel(clf, threshold=str(median)+'*median')

            elif method == 'SelectKBest':
                
                if verbose > 0:
                    self.report_fd.write("[ SelectKBest ]" + "\n")
                
                if median == '':
                    median=1.0
                else:
                    median=float(median)
                
                if verbose > 1:
                    self.report_fd.write("median: " + str(median) + "\n")
          
                feature_num = len(self.train_features.columns)
                k_best = float(feature_num)*median/2
                k_best = int(k_best)

                if model == '':
                    selector = SelectKBest(k=k_best)
                elif model == 'chi2':
                    selector = SelectKBest(score_func=chi2, k=k_best)
                elif model == 'f_classif':
                    selector = SelectKBest(score_func=f_classif, k=k_best)
                elif model == 'mutual_info_classif':
                    selector = SelectKBest(score_func=f_classif, k=k_best)
                else:
                    sys.exit("error: feature selection: " + "method " + str(i) + "has unrecognised model: " + str(model))

                if verbose > 1:
                    self.report_fd.write("model: " + str(model) + "\n")
            
            elif method == 'RFECV':
                
                if verbose > 0:
                    self.report_fd.write("[ RFECV ]" + "\n")
                
                if model == 'DT':
                    estimator = DecisionTreeClassifier()
                elif model == 'RFC':
                    estimator = RandomForestClassifier()
                else:
                    sys.exit("error: feature selection: " + "method " + str(i) + "has unrecognised model: " + str(model))
                estimator.set_params(random_state=_rand_state_)

                if median == '':
                    median=0.9
                else:
                    median=float(median)
                
                if verbose > 1:
                    self.report_fd.write("median: " + str(median) + "\n")
          
                feature_num = len(self.train_features.columns)
                k_best = float(feature_num)*median
                k_best = int(k_best)

                selector = RFECV(estimator, min_features_to_select=k_best, step=1, cv=10, scoring=score)

                if verbose > 1:
                    self.report_fd.write("model: " + str(model) + "\n")

            else:
                sys.exit("error: feature selection: " + "unrecognised selection method: " + method)

            selector.fit(self.train_features, self.train_labels)
            selected_cols = selector.get_support(indices=True)
            dropped_cols = np.invert(selector.get_support())
            dropped_cols = np.where(dropped_cols)[0]

            if verbose > 1:
                self.report_fd.write("\n")

            selected_feature_names = (self.train_features.columns[selected_cols]).tolist()
            dropped_feature_names = (self.train_features.columns[dropped_cols]).tolist()
            if verbose > 2:
                self.report_fd.write("selected features num: " + str(len(selected_cols)) + "\n")
                self.report_fd.write("dropped features num: " + str(len(dropped_cols)) + "\n")
                self.report_fd.write("dropped features [" + "\n")
                for feature in dropped_feature_names:
                    self.report_fd.write("\t" + feature + "\n")
                self.report_fd.write("]" + "\n")
              
            self.train_features.drop(self.train_features.columns[dropped_cols], axis=1, inplace=True)
            self.test_features.drop(self.test_features.columns[dropped_cols], axis=1, inplace=True)

        if verbose > 0:
            self.report_fd.write("= Final feature selection =" + "\n")
        selected_feature_names = (self.train_features.columns).tolist()
        if verbose > 0:
            self.report_fd.write("selected features num: " + str(len(selected_feature_names)) + "\n")
        if verbose > 1:
            self.report_fd.write("selected features [" + "\n")
            for feature in selected_feature_names:
                self.report_fd.write("\t" + feature + "\n")
            self.report_fd.write("]" + "\n")

        if verbose > 0:
            self.report_fd.write("= =========================== =" + "\n")
            self.report_fd.write("\n")

    def hyper_param_selection(self):
     
        verbose = int(self.report_cfg['hyper_param_select_verbose'])

        if verbose > 0:
            self.report_fd.write("= [3] Model Hyper-Parameter Selection Stage =" + "\n")
            self.report_fd.write("= ========================================= =" + "\n")
            self.report_fd.write("\n")

        # perform automatic hyper-parameter selection
        if self.hp_cfg['auto'] != 'true':
            if verbose > 0:
                self.report_fd.write("= No automatic model hyper-parameter selection is done! =" + "\n")
                self.report_fd.write("= ========================================= =" + "\n")
                self.report_fd.write("\n")
            return None
        else:
            if verbose > 0:
                self.report_fd.write("= Automatic model hyper-parameter selection =" + "\n")
       
        if self.hp_cfg['method'] == 'GridSearchCV':
            
            if verbose > 0:
                self.report_fd.write("method: GridSearchCV" + "\n")
            
            gs = None
            hyper_param_grid = []
        
            if self.hp_cfg['model'] == 'SVC':
          
                if verbose > 0:
                    self.report_fd.write("model: SVC" + "\n")
            
                kernels = self.hp_cfg['kernel'].split(",")
                gammas = [ float(x) for x in self.hp_cfg['gamma'].split(",") ] 
                Cs = [ int(x) for x in self.hp_cfg['C'].split(",") ]
           
                if verbose > 1:
                    self.report_fd.write("= hyper-parameter grid search space =\n")
                    self.report_fd.write("kernels: ")
                    for kernel in kernels:
                        self.report_fd.write(kernel)
                        self.report_fd.write(" ")
                    self.report_fd.write("\n")

                    self.report_fd.write("gammas: ")
                    for gamma in gammas:
                        self.report_fd.write(str(gamma))
                        self.report_fd.write(" ")
                    self.report_fd.write("\n")

                    self.report_fd.write("Cs: ")
                    for C in Cs:
                        self.report_fd.write(str(C))
                        self.report_fd.write(" ")
                    self.report_fd.write("\n")
                    self.report_fd.write("= =\n")

                # recover hyper-parameter search space from config file
                for kernel in kernels:
                    if kernel != 'linear':
                        hyper_param_grid.append({'kernel': [kernel], 'gamma': gammas, 'C': Cs})
                    else:
                        hyper_param_grid.append({'kernel': [kernel], 'C': Cs})
        
                gs = GridSearchCV(SVC(random_state=_rand_state_), hyper_param_grid, cv=int(self.hp_cfg['cv_k_folds']), n_jobs=-1, scoring=self.hp_cfg['scoring'])

            elif self.hp_cfg['model'] == 'DT':
                
                if verbose > 0:
                    self.report_fd.write("model: DT" + "\n")
            
                max_depths = [None]
                for x in self.hp_cfg['max_depth'].split(","):
                    max_depths.append(int(x))

                min_samples_splits = [ float(x) for x in self.hp_cfg['min_samples_split'].split(",") ]
                min_samples_leafs = [ int(x) for x in self.hp_cfg['min_samples_leaf'].split(",") ]
                
                features_num = len(self.train_features.columns)
                max_features = []
                for x in self.hp_cfg['max_features'].split(","):
                    if int(x) <= features_num:
                        max_features.append(int(x))

                if verbose > 1:
                    self.report_fd.write("= hyper-parameter grid search space =\n")
                    self.report_fd.write("max_depths: ")
                    for x in max_depths:
                        self.report_fd.write(str(x))
                        self.report_fd.write(" ")
                    self.report_fd.write("\n")

                    self.report_fd.write("min_samples_split: ")
                    for x in min_samples_splits:
                        self.report_fd.write(str(x))
                        self.report_fd.write(" ")
                    self.report_fd.write("\n")

                    self.report_fd.write("min_samples_leaf: ")
                    for x in min_samples_leafs:
                        self.report_fd.write(str(x))
                        self.report_fd.write(" ")
                    self.report_fd.write("\n")
                    self.report_fd.write("= =\n")

                    self.report_fd.write("max_features: ")
                    for x in max_features:
                        self.report_fd.write(str(x))
                        self.report_fd.write(" ")
                    self.report_fd.write("\n")

                # recover hyper-parameter search space from config file
                hyper_param_grid = {
                    'max_depth' : max_depths,
                    'min_samples_split': min_samples_splits,
                    'min_samples_leaf': min_samples_leafs,
                    'max_features' : max_features,
                }
                
                gs = GridSearchCV(DecisionTreeClassifier(random_state=_rand_state_), hyper_param_grid, cv=int(self.hp_cfg['cv_k_folds']), n_jobs=-1, scoring=self.hp_cfg['scoring'])
            
            elif self.hp_cfg['model'] == 'RFC':
                
                if verbose > 0:
                    self.report_fd.write("model: RFC" + "\n")
            
                num_estimators = [ int(x) for x in self.hp_cfg['n_estimators'].split(",") ]
                
                max_depths = [None]
                for x in self.hp_cfg['max_depth'].split(","):
                    max_depths.append(int(x))

                min_samples_splits = [ float(x) for x in self.hp_cfg['min_samples_split'].split(",") ]
                min_samples_leafs = [ int(x) for x in self.hp_cfg['min_samples_leaf'].split(",") ]
                
                features_num = len(self.train_features.columns)
                max_features = []
                for x in self.hp_cfg['max_features'].split(","):
                    max_features.append(str(x))

                if verbose > 1:
                    self.report_fd.write("= hyper-parameter grid search space =\n")
                    self.report_fd.write("n_estimators: ")
                    for x in num_estimators:
                        self.report_fd.write(str(x))
                        self.report_fd.write(" ")
                    self.report_fd.write("\n")

                    self.report_fd.write("max_depths: ")
                    for x in max_depths:
                        self.report_fd.write(str(x))
                        self.report_fd.write(" ")
                    self.report_fd.write("\n")

                    self.report_fd.write("min_samples_split: ")
                    for x in min_samples_splits:
                        self.report_fd.write(str(x))
                        self.report_fd.write(" ")
                    self.report_fd.write("\n")

                    self.report_fd.write("min_samples_leaf: ")
                    for x in min_samples_leafs:
                        self.report_fd.write(str(x))
                        self.report_fd.write(" ")
                    self.report_fd.write("\n")
                    self.report_fd.write("= =\n")

                    self.report_fd.write("max_features: ")
                    for x in max_features:
                        self.report_fd.write(str(x))
                        self.report_fd.write(" ")
                    self.report_fd.write("\n")

                # recover hyper-parameter search space from config file
                hyper_param_grid = {
                    'n_estimators' : num_estimators,
                    'max_depth' : max_depths,
                    'min_samples_split': min_samples_splits,
                    'min_samples_leaf': min_samples_leafs,
                    'max_features' : max_features,
                }
                
                gs = GridSearchCV(RandomForestClassifier(random_state=_rand_state_), hyper_param_grid, cv=int(self.hp_cfg['cv_k_folds']), n_jobs=-1, scoring=self.hp_cfg['scoring'])

            elif self.hp_cfg['model'] == 'MLP':
                
                if verbose > 0:
                    self.report_fd.write("model: MLP" + "\n")
            
                activations = [ str(x) for x in self.hp_cfg['activation'].split(",") ]
                
                hidden_layer_sizes_lst = []
                h_layer_sizes_lst = [ x[1:-1] for x in self.hp_cfg['hidden_layer_sizes'].split(",") ]
                for x in h_layer_sizes_lst:
                    num_lst = [int(num) for num in x.split(';')]
                    hidden_layer_sizes_lst.append(tuple(num_lst))

                solvers_lst = [ str(x) for x in self.hp_cfg['solver'].split(",") ]
                alphas_lst = [ float(x) for x in self.hp_cfg['alpha'].split(",") ]

                if verbose > 1:
                    self.report_fd.write("= hyper-parameter grid search space =\n")
                    self.report_fd.write("activations: ")
                    for x in activations:
                        self.report_fd.write(str(x))
                        self.report_fd.write(" ")
                    self.report_fd.write("\n")

                    self.report_fd.write("hidden_layer_sizes: ")
                    for x in hidden_layer_sizes_lst:
                        self.report_fd.write(str(x))
                        self.report_fd.write(" ")
                    self.report_fd.write("\n")

                    self.report_fd.write("solvers: ")
                    for x in solvers_lst:
                        self.report_fd.write(str(x))
                        self.report_fd.write(" ")
                    self.report_fd.write("\n")

                    self.report_fd.write("alphas: ")
                    for x in alphas_lst:
                        self.report_fd.write(str(x))
                        self.report_fd.write(" ")
                    self.report_fd.write("\n")

                # recover hyper-parameter search space from config file
                hyper_param_grid = {
                    'activation' : activations,
                    'hidden_layer_sizes': hidden_layer_sizes_lst,
                    'solver': solvers_lst,
                    'alpha' : alphas_lst,
                }
                
                gs = GridSearchCV(MLPClassifier(random_state=_rand_state_), hyper_param_grid, cv=int(self.hp_cfg['cv_k_folds']), n_jobs=-1, scoring=self.hp_cfg['scoring'])
     
            elif self.hp_cfg['model'] == 'AdaBoost':
                
                if verbose > 0:
                    self.report_fd.write("model: AdaBoost" + "\n")
            
                num_estimators = [ int(x) for x in self.hp_cfg['n_estimators'].split(",") ]
                
                if verbose > 1:
                    self.report_fd.write("= hyper-parameter grid search space =\n")
                    self.report_fd.write("n_estimators: ")
                    for x in num_estimators:
                        self.report_fd.write(str(x))
                        self.report_fd.write(" ")
                    self.report_fd.write("\n")

                # recover hyper-parameter search space from config file
                hyper_param_grid = {
                    'n_estimators' : num_estimators
                }
                
                gs = GridSearchCV(AdaBoostClassifier(random_state=_rand_state_), hyper_param_grid, cv=int(self.hp_cfg['cv_k_folds']), n_jobs=-1, scoring=self.hp_cfg['scoring'])
               
            self.report_fd.write("\n")

            gs.fit(self.train_features, self.train_labels)
            
            if verbose > 0:
                self.report_fd.write(str(gs.best_params_) + "\n")
                self.report_fd.write("= ========================================= =" + "\n")
                self.report_fd.write("\n")
            
            self.best_hparams = gs.best_params_
   
    def model_training(self):
        
        verbose = int(self.report_cfg['model_train_verbose'])

        if verbose > 0:
            self.report_fd.write("= [4] Model Training Stage =" + "\n")
            self.report_fd.write("= ======================== =" + "\n")
            self.report_fd.write("\n")
       
        # select train features and labels
        train_dataset = self.train_features
        train_labels = self.train_labels

        # trained models
        for baseline_name in ml_pipeline_config.available_baselines:
            dummy_clf = None
            if baseline_name != 'constant':
                dummy_clf = DummyClassifier(strategy=baseline_name, random_state=_rand_state_)
            else:
                dummy_clf = DummyClassifier(strategy=baseline_name, constant=1, random_state=_rand_state_)
            dummy_clf.fit(train_dataset, train_labels)
            self.trained_classifiers['baseline'][baseline_name] = dummy_clf
            
        self.report_fd.write("main model: " + str(self.model_name) + "\n")
        
        clf = None
        if self.best_hparams != None:
            
            self.report_fd.write("[auto hyper-parameters search]" + "\n")
            self.report_fd.write("param: " + str(self.best_hparams) + "\n")
            
            if self.model_name == 'SVC':
                clf = SVC(**self.best_hparams)
                clf.set_params(probability=True)
            elif self.model_name == 'DT':
                clf = DecisionTreeClassifier(**self.best_hparams)
            elif self.model_name == 'RFC':
                clf = RandomForestClassifier(**self.best_hparams)
            elif self.model_name == 'MLP':
                clf = MLPClassifier(**self.best_hparams)
            elif self.model_name == 'AdaBoost':
                clf = AdaBoostClassifier(**self.best_hparams)
        else:
            # use default values

            self.report_fd.write("[default parameters]" + "\n")

            if self.model_name == 'SVC':
                clf = SVC()
                clf.set_params(probability=True)
            elif self.model_name == 'DT':

                max_d = None
                min_s_leaf = 1
                min_s_split = 0.2
                max_f = None
                if 'max_depth' in self.train_cfg:
                    max_d = int(self.train_cfg['max_depth'])
                if 'min_samples_leaf' in self.train_cfg:
                    min_s_leaf = int(self.train_cfg['min_samples_leaf'])
                if 'min_samples_split' in self.train_cfg:
                    min_s_split = int(self.train_cfg['min_samples_split'])
                if 'max_features' in self.train_cfg:
                    max_f = int(self.train_cfg['max_features'])

                clf = DecisionTreeClassifier(max_depth=max_d, 
                        min_samples_split=min_s_split, 
                        random_state=_rand_state_, 
                        min_samples_leaf=min_s_leaf,
                        max_features=max_f)
                self.report_fd.write("param: " + str(clf.get_params()) + "\n")

            elif self.model_name == 'RFC':
                
                n_est = 100
                max_d = None
                min_s_leaf = 1
                min_s_split = 0.2
                if 'n_estimators' in self.train_cfg:
                    n_est = int(self.train_cfg['n_estimators'])
                if 'max_depth' in self.train_cfg:
                    max_d = int(self.train_cfg['max_depth'])
                if 'min_samples_leaf' in self.train_cfg:
                    min_s_leaf = int(self.train_cfg['min_samples_leaf'])
                if 'min_samples_split' in self.train_cfg:
                    min_s_split = float(self.train_cfg['min_samples_split'])
                
                clf = RandomForestClassifier(n_estimators=n_est,
                        max_depth=max_d, 
                        min_samples_split=min_s_split, 
                        random_state=_rand_state_, 
                        min_samples_leaf=min_s_leaf)
                self.report_fd.write("param: " + str(clf.get_params()) + "\n")

            elif self.model_name == 'MLP':
                clf = MLPClassifier()
            elif self.model_name == 'AdaBoost':
                clf = AdaBoostClassifier()

        clf.set_params(random_state=_rand_state_)

        clf.fit(self.train_features, self.train_labels)
        
        self.trained_classifiers[self.model_name] = clf
        
        if verbose > 0:
            self.report_fd.write("= ======================== =" + "\n")
            self.report_fd.write("\n")

def report_results(cfg, report_fds, predictions, test_loop_locations, test_par_labels, test_icc_labels):

    report_cfg = cfg['report']
    test_cfg = cfg['model_testing']

    pl_report_fd = report_fds['ml_pipeline']
    acc_report_fd = report_fds['accuracy']
    oracle_report_fd = report_fds['oracle']

    # accuracy report to be returned 
    accuracy_report = {}
    accuracy_report['baseline'] = {}
    for baseline_name in ml_pipeline_config.available_baselines:
        accuracy_report['baseline'][baseline_name] = {}
    accuracy_report[test_cfg['model']] = {}
    accuracy_report['icc_competition'] = {}
    accuracy_report['oracle'] = {}

    main_report = accuracy_report[test_cfg['model']] 

    verbose = int(report_cfg['report_verbose'])

    if verbose > 0:
        pl_report_fd.write("= [6] Model Report Stage =" + "\n")
        pl_report_fd.write("= ====================== =" + "\n")
        pl_report_fd.write("\n")

    if report_cfg['baseline'] == 'true':
        
        for baseline_name, preds in predictions['baseline'].items():
            
            baseline_report = accuracy_report['baseline'][baseline_name]
            
            acc_report_fd.write("= SciKitLearn Dummy (Baseline) Predictor =" + "\n")
            acc_report_fd.write("= =========================== =" + "\n")
            acc_report_fd.write("type: " + baseline_name + "\n")
        
            accuracy = accuracy_score(test_par_labels, preds[CLASS_IDX])
            acc_report_fd.write("accuracy score: " + "{0:.3f}".format(accuracy) + "\n")
            baseline_report['accuracy'] = accuracy

            accuracy = balanced_accuracy_score(test_par_labels, preds[CLASS_IDX])
            acc_report_fd.write("balanced accuracy score: " + "{0:.3f}".format(accuracy) + "\n")
            baseline_report['balanced_accuracy'] = accuracy

            accuracy = precision_score(test_par_labels, preds[CLASS_IDX])
            acc_report_fd.write("precision score: " + "{0:.3f}".format(accuracy) + "\n")
            baseline_report['precision'] = accuracy

            accuracy = f1_score(test_par_labels, preds[CLASS_IDX])
            acc_report_fd.write("f1 score: " + "{0:.3f}".format(accuracy) + "\n")
            baseline_report['f1_score'] = accuracy

            accuracy = recall_score(test_par_labels, preds[CLASS_IDX])
            acc_report_fd.write("recall score: " + "{0:.3f}".format(accuracy) + "\n")
            baseline_report['recall_score'] = accuracy
            acc_report_fd.write("= =========================== =" + "\n")
            acc_report_fd.write("\n")

    if report_cfg['main'] == 'true':
        preds = predictions[cfg['model_testing']['model']]
    
        acc_report_fd.write("= SciKitLearn Main Score Report =" + "\n")
        acc_report_fd.write("= ============================= =" + "\n")
        accuracy = accuracy_score(test_par_labels, preds[CLASS_IDX])
        print("accuracy score: " + "{0:.3f}".format(accuracy))
        acc_report_fd.write("accuracy score: " + "{0:.3f}".format(accuracy) + "\n")
        main_report['accuracy'] = accuracy

        accuracy = balanced_accuracy_score(test_par_labels, preds[CLASS_IDX])
        print("balanced accuracy score: " + "{0:.3f}".format(accuracy))
        acc_report_fd.write("balanced accuracy score: " + "{0:.3f}".format(accuracy) + "\n")
        main_report['balanced_accuracy'] = accuracy

        accuracy = precision_score(test_par_labels, preds[CLASS_IDX])
        print("precision score: " + "{0:.3f}".format(accuracy))
        acc_report_fd.write("precision score: " + "{0:.3f}".format(accuracy) + "\n")
        main_report['precision'] = accuracy

        accuracy = f1_score(test_par_labels, preds[CLASS_IDX])
        print("f1 score: " + "{0:.3f}".format(accuracy))
        acc_report_fd.write("f1 score: " + "{0:.3f}".format(accuracy) + "\n")
        main_report['f1_score'] = accuracy

        accuracy = recall_score(test_par_labels, preds[CLASS_IDX])
        print("recall score: " + "{0:.3f}".format(accuracy))
        acc_report_fd.write("recall score: " + "{0:.3f}".format(accuracy) + "\n")
        main_report['recall_score'] = accuracy
        acc_report_fd.write("= ======================== =" + "\n")
        acc_report_fd.write("\n")
    
    if report_cfg['icc_competition'] == 'true':
        preds = predictions[cfg['model_testing']['model']]

        probs = None
        if test_cfg['model'] in ['DT','RFC','SVC','AdaBoost','MLP']:
            probs = preds[PROBABILITY_IDX]
        
        preds = preds[CLASS_IDX]

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
            prob_0 = probs[i][0]
            prob_1 = probs[i][1]
            icc = test_icc_labels[i]
            par = test_par_labels[i]

            if icc == 0:
                if pred == 0:
                    case_00_num += 1
                    if par == 0:
                        case_00_npar += 1
                        cases_00_npar.add(str(test_loop_locations[i]) + ": 0 [" + str(prob_0) + "]" + ": 1 [" + str(prob_1) + "]")
                    elif par == 1:
                        case_00_par += 1
                        cases_00_par.add(str(test_loop_locations[i]) + ": 0 [" + str(prob_0) + "]" + ": 1 [" + str(prob_1) + "]")
                elif pred == 1:
                    case_01_num += 1
                    if par == 0:
                        case_01_npar += 1
                        cases_01_npar.add(str(test_loop_locations[i]) + ": 0 [" + str(prob_0) + "]" + ": 1 [" + str(prob_1) + "]")
                    elif par == 1:
                        case_01_par += 1
                        cases_01_par.add(str(test_loop_locations[i]) + ": 0 [" + str(prob_0) + "]" + ": 1 [" + str(prob_1) + "]")
            elif icc == 1:
                if pred == 0:
                    case_10_num += 1
                    if par == 0:
                        case_10_npar += 1
                        cases_10_npar.add(str(test_loop_locations[i]) + ": 0 [" + str(prob_0) + "]" + ": 1 [" + str(prob_1) + "]")
                    elif par == 1:
                        case_10_par += 1
                        cases_10_par.add(str(test_loop_locations[i]) + ": 0 [" + str(prob_0) + "]" + ": 1 [" + str(prob_1) + "]")
                elif pred == 1:
                    case_11_num += 1
                    if par == 0:
                        case_11_npar += 1
                        cases_11_npar.add(str(test_loop_locations[i]) + ": 0 [" + str(prob_0) + "]" + ": 1 [" + str(prob_1) + "]")
                    elif par == 1:
                        case_11_par += 1
                        cases_11_par.add(str(test_loop_locations[i]) + ": 0 [" + str(prob_0) + "]" + ": 1 [" + str(prob_1) + "]")
         
            if pred != par:
                # misprediction
                mispredicts += 1
                if pred == 1:
                    unsafe_mispredicts += 1
                    mispredicted_loops.add(str(test_loop_locations[i]) + ":" + str(preds[i]) + ":" + str(test_par_labels[i]) + ":unsafe" + ": 0 [" + str(prob_0) + "]" + ": 1 [" + str(prob_1) + "]")
                else:
                    safe_mispredicts += 1
                    mispredicted_loops.add(str(test_loop_locations[i]) + ":" + str(preds[i]) + ":" + str(test_par_labels[i]) + ":safe" + ": 0 [" + str(prob_0) + "]" + ": 1 [" + str(prob_1) + "]")
            else:
                predicted_loops.add(str(test_loop_locations[i]) + ":" + str(preds[i]) + ":" + str(test_par_labels[i]) + ": 0 [" + str(prob_0) + "]" + ": 1 [" + str(prob_1) + "]")

            if pred != icc:
                if pred == par:
                    icc_wins += 1
                    icc_win_loops.add(str(test_loop_locations[i]) + ":" + str(preds[i]) + ":" + str(test_icc_labels[i]) + ":win" + ": 0 [" + str(prob_0) + "]" + ": 1 [" + str(prob_1) + "]")
                else:
                    icc_losses += 1
                    icc_loss_loops.add(str(test_loop_locations[i]) + ":" + str(preds[i]) + ":" + str(test_icc_labels[i]) + ":loss" + ": 0 [" + str(prob_0) + "]" + ": 1 [" + str(prob_1) + "]")
    
        # Table
        # 0 0
        # 0 1
        # 1 0
        # 1 1
        oracle_report_fd.write("Table" + "\n")
        oracle_report_fd.write("========" + "\n")
        oracle_report_fd.write("=== case [0 / 0] ===" + "\n")
        oracle_report_fd.write("num: " + "{}".format(case_00_num) + "\n")
        oracle_report_fd.write("par: " + "{}".format(case_00_par) + "\n")
        oracle_report_fd.write("non-par: " + "{}".format(case_00_npar) + "\n")
        oracle_report_fd.write("=== case [0 / 1] ===" + "\n")
        oracle_report_fd.write("num: " + "{}".format(case_01_num) + "\n")
        oracle_report_fd.write("par: " + "{}".format(case_01_par) + "\n")
        oracle_report_fd.write("non-par: " + "{}".format(case_01_npar) + "\n")
        oracle_report_fd.write("=== case [1 / 0] ===" + "\n")
        oracle_report_fd.write("num: " + "{}".format(case_10_num) + "\n")
        oracle_report_fd.write("par: " + "{}".format(case_10_par) + "\n")
        oracle_report_fd.write("non-par: " + "{}".format(case_10_npar) + "\n")
        oracle_report_fd.write("=== case [1 / 1] ===" + "\n")
        oracle_report_fd.write("num: " + "{}".format(case_11_num) + "\n")
        oracle_report_fd.write("par: " + "{}".format(case_11_par) + "\n")
        oracle_report_fd.write("non-par: " + "{}".format(case_11_npar) + "\n")
        oracle_report_fd.write("========" + "\n")
        oracle_report_fd.write("\n")

        if report_cfg['loop_locations'] == 'true':
            oracle_report_fd.write("=== case [0 / 0] ===" + "\n")
            oracle_report_fd.write("= parallelisible =" + "\n")
            for loop in cases_00_par:
                oracle_report_fd.write(loop + "\n")
            oracle_report_fd.write("= non-parallelisible =" + "\n")
            for loop in cases_00_npar:
                oracle_report_fd.write(loop + "\n")
            oracle_report_fd.write("===================" + "\n")
            oracle_report_fd.write("\n")

            oracle_report_fd.write("=== case [0 / 1] ===" + "\n")
            oracle_report_fd.write("= parallelisible =" + "\n")
            for loop in cases_01_par:
                oracle_report_fd.write(loop + "\n")
            oracle_report_fd.write("= non-parallelisible =" + "\n")
            for loop in cases_01_npar:
                oracle_report_fd.write(loop + "\n")
            oracle_report_fd.write("===================" + "\n")
            oracle_report_fd.write("\n")

            oracle_report_fd.write("=== case [1 / 0] ===" + "\n")
            oracle_report_fd.write("= parallelisible =" + "\n")
            for loop in cases_10_par:
                oracle_report_fd.write(loop + "\n")
            oracle_report_fd.write("= non-parallelisible =" + "\n")
            for loop in cases_10_npar:
                oracle_report_fd.write(loop + "\n")
            oracle_report_fd.write("===================" + "\n")
            oracle_report_fd.write("\n")

            oracle_report_fd.write("=== case [1 / 1] ===" + "\n")
            oracle_report_fd.write("= parallelisible =" + "\n")
            for loop in cases_11_par:
                oracle_report_fd.write(loop + "\n")
            oracle_report_fd.write("= non-parallelisible =" + "\n")
            for loop in cases_11_npar:
                oracle_report_fd.write(loop + "\n")
            oracle_report_fd.write("===================" + "\n")
            oracle_report_fd.write("\n")

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

        oracle_report_fd.write("Mispredictions: " + str(mispredicts) + "\n")
        oracle_report_fd.write("\n")

        oracle_report_fd.write("Accuracy" + "\n")
        oracle_report_fd.write("========" + "\n")
        oracle_report_fd.write("tests: " + "{}".format(tests) + "\n")
        oracle_report_fd.write("mispredicts: " + "{}".format(mispredicts) + "\n")
        oracle_report_fd.write("accuracy-rate: " + "{0:.2f}".format(accuracy_rate) + "\n")
        oracle_report_fd.write("error-rate: " + "{0:.2f}".format(error_rate) + "\n")
        oracle_report_fd.write("safe-mispredicts: " + "{}".format(safe_mispredicts) + "\n")
        oracle_report_fd.write("unsafe-mispredicts: " + "{}".format(unsafe_mispredicts) + "\n")
        oracle_report_fd.write("safe-error-rate: " + "{0:.2f}".format(safe_mispredict_rate) + "\n")
        oracle_report_fd.write("unsafe-error-rate: " + "{0:.2f}".format(unsafe_mispredict_rate) + "\n")
        oracle_report_fd.write("========" + "\n")
        oracle_report_fd.write("\n")
        
        accuracy_report['safe'] = safe_mispredict_rate
        accuracy_report['unsafe'] = unsafe_mispredict_rate
            
        oracle_report_fd.write("ICC competition" + "\n")
        oracle_report_fd.write("========" + "\n")
        oracle_report_fd.write("icc-discrepancies: " + "{}".format(icc_disagreements) + "\n")
        oracle_report_fd.write("icc-wins: " + "{}".format(icc_wins) + "\n")
        oracle_report_fd.write("icc-losses: " + "{}".format(icc_losses) + "\n")
        oracle_report_fd.write("win-rate: " + "{0:.2f}".format(icc_win_rate) + "\n")
        oracle_report_fd.write("loss-rate: " + "{0:.2f}".format(icc_loss_rate) + "\n")
        oracle_report_fd.write("========" + "\n")
        oracle_report_fd.write("\n")

        accuracy_report['icc-win'] = icc_win_rate
        accuracy_report['icc-loss'] = icc_loss_rate

        if report_cfg['loop_locations'] == 'true':
            oracle_report_fd.write("loop:pred:label:safe\n")
            
            oracle_report_fd.write("Mispredicted loops:" + "\n")
            oracle_report_fd.write("===================" + "\n")
            for loop_report in mispredicted_loops:
                oracle_report_fd.write(loop_report + "\n")
            oracle_report_fd.write("===================" + "\n")
          
            oracle_report_fd.write("\n")
            
            oracle_report_fd.write("Predicted loops:" + "\n")
            oracle_report_fd.write("===================" + "\n")
            for loop_report in predicted_loops:
                oracle_report_fd.write(loop_report + "\n")
            oracle_report_fd.write("===================" + "\n")

            oracle_report_fd.write("\n")

            oracle_report_fd.write("loop:pred:icc:result\n")
            
            oracle_report_fd.write("Win over ICC loops:" + "\n")
            oracle_report_fd.write("===================" + "\n")
            for loop_report in icc_win_loops:
                oracle_report_fd.write(loop_report + "\n")
            oracle_report_fd.write("===================" + "\n")
          
            oracle_report_fd.write("\n")
            
            oracle_report_fd.write("Loss to ICC loops:" + "\n")
            oracle_report_fd.write("===================" + "\n")
            for loop_report in icc_loss_loops:
                oracle_report_fd.write(loop_report + "\n")
            oracle_report_fd.write("===================" + "\n")

            oracle_report_fd.write("\n")

    if report_cfg['oracle_guide'] == 'true':
        preds = predictions[cfg['model_testing']['model']]

        probs = None
        if test_cfg['model'] in ['DT','RFC','SVC','AdaBoost','MLP']:
            probs = preds[PROBABILITY_IDX]
        
        preds = preds[CLASS_IDX]

        # put loops into different buckets dependent on oracle predictions

        nparallel_npar = set()
        nparallel_par = set()

        parallel_npar = set()
        parallel_par = set()

        nparallel_npar_prob0 = []
        nparallel_npar_prob1 = []

        nparallel_par_prob0 = []
        nparallel_par_prob1 = []

        parallel_npar_prob0 = []
        parallel_npar_prob1 = []

        parallel_par_prob0 = []
        parallel_par_prob1 = []

        for i in range(0,tests):
            pred = preds[i]
            prob_0 = probs[i][0]
            prob_1 = probs[i][1]
            par = test_par_labels[i]

            if pred == 0:
                if par == 0:
                    nparallel_npar.add(str(test_loop_locations[i]) + ":" + str(prob_0) + ":" + str(prob_1))
                    nparallel_npar_prob0.append(prob_0)
                    nparallel_npar_prob1.append(prob_1)
                else:
                    nparallel_par.add(str(test_loop_locations[i]) + ":" + str(prob_0) + ":" + str(prob_1))
                    nparallel_par_prob0.append(prob_0)
                    nparallel_par_prob1.append(prob_1)
            else:
                if par == 0:
                    parallel_npar.add(str(test_loop_locations[i]) + ":" + str(prob_0) + ":" + str(prob_1))
                    parallel_npar_prob0.append(prob_0)
                    parallel_npar_prob1.append(prob_1)
                else:
                    parallel_par.add(str(test_loop_locations[i]) + ":" + str(prob_0) + ":" + str(prob_1))
                    parallel_par_prob0.append(prob_0)
                    parallel_par_prob1.append(prob_1)
   
        # oracle guidance
        total_cases = len(nparallel_npar) + len(nparallel_par) + len(parallel_npar) + len(parallel_par)
        oracle_report_fd.write("Table" + "\n")
        oracle_report_fd.write("total-num: " + "{}".format(total_cases) + "\n")
        oracle_report_fd.write("========" + "\n")
        oracle_report_fd.write("=== case [nparallel / npar] ===" + "\n")
        oracle_report_fd.write("num: " + "{}".format(len(nparallel_npar)) + "\n")
        oracle_report_fd.write("mean-prob-0: " + "{0:.3f}".format(pd.Series(nparallel_npar_prob0).mean()*100) + "\n")
        oracle_report_fd.write("mean-prob-1: " + "{0:.3f}".format(pd.Series(nparallel_npar_prob1).mean()*100) + "\n")
        oracle_report_fd.write("=== case [nparallel / par] ===" + "\n")
        oracle_report_fd.write("num: " + "{}".format(len(nparallel_par)) + "\n")
        oracle_report_fd.write("mean-prob-0: " + "{0:.3f}".format(pd.Series(nparallel_par_prob0).mean()*100) + "\n")
        oracle_report_fd.write("mean-prob-1: " + "{0:.3f}".format(pd.Series(nparallel_par_prob1).mean()*100) + "\n")
        oracle_report_fd.write("=== case [parallel / npar] ===" + "\n")
        oracle_report_fd.write("num: " + "{}".format(len(parallel_npar)) + "\n")
        oracle_report_fd.write("mean-prob-0: " + "{0:.3f}".format(pd.Series(parallel_npar_prob0).mean()*100) + "\n")
        oracle_report_fd.write("mean-prob-1: " + "{0:.3f}".format(pd.Series(parallel_npar_prob1).mean()*100) + "\n")
        oracle_report_fd.write("=== case [parallel / par] ===" + "\n")
        oracle_report_fd.write("num: " + "{}".format(len(parallel_par)) + "\n")
        oracle_report_fd.write("mean-prob-0: " + "{0:.3f}".format(pd.Series(parallel_par_prob0).mean()*100) + "\n")
        oracle_report_fd.write("mean-prob-1: " + "{0:.3f}".format(pd.Series(parallel_par_prob1).mean()*100) + "\n")
        oracle_report_fd.write("========" + "\n")
        oracle_report_fd.write("\n")

        if report_cfg['loop_locations'] == 'true':
            oracle_report_fd.write("=== case [nparallel / npar] ===" + "\n")
            oracle_report_fd.write("loop-location:prob_0:prob_1" + "\n")
            for loop in nparallel_npar:
                oracle_report_fd.write(loop + "\n")
            oracle_report_fd.write("===================" + "\n")
            oracle_report_fd.write("\n")

            oracle_report_fd.write("=== case [nparallel / par] ===" + "\n")
            oracle_report_fd.write("loop-location:prob_0:prob_1" + "\n")
            for loop in nparallel_par:
                oracle_report_fd.write(loop + "\n")
            oracle_report_fd.write("===================" + "\n")
            oracle_report_fd.write("\n")

            oracle_report_fd.write("=== case [parallel / npar] ===" + "\n")
            oracle_report_fd.write("loop-location:prob_0:prob_1" + "\n")
            for loop in parallel_npar:
                oracle_report_fd.write(loop + "\n")
            oracle_report_fd.write("===================" + "\n")
            oracle_report_fd.write("\n")

            oracle_report_fd.write("=== case [parallel / par] ===" + "\n")
            oracle_report_fd.write("loop-location:prob_0:prob_1" + "\n")
            for loop in parallel_par:
                oracle_report_fd.write(loop + "\n")
            oracle_report_fd.write("===================" + "\n")
            oracle_report_fd.write("\n")

    if verbose > 0:
        pl_report_fd.write("==============================================================" + "\n")
        pl_report_fd.write("\n\n\n")
    
    return accuracy_report

if __name__ == "__main__":

    sys.exit("error: ./ml_pipeline.py script cannot be launched as a main script and exists to be imported")
