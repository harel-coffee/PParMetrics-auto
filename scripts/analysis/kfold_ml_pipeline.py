#!/usr/bin/python3

import sys
import os

import configparser

import pandas as pd
import numpy as np

from sklearn.model_selection import KFold
from sklearn.model_selection import RepeatedKFold

import ppar
import ml_pipeline_config 
import ml_pipeline 

if __name__ == "__main__":

    print("==============================================")
    print("==== Machine Learning K-Fold Pipeline Run ====")
    print("==============================================")

    if len(sys.argv) != 4:
        sys.exit("error: use ./train_test_pipeline.py " +
                 "[TRAIN/TEST data file]" + 
                 "[Pipeline Configuration INI file]" + 
                 "[REPORT folder]")

    # get command line arguments and check that provided files exist
    raw_data_filename = sys.argv[1]
    pipeline_cfg_filename = sys.argv[2]
    report_folder = sys.argv[3] + "/"

    if not os.path.exists(raw_data_filename):
        sys.exit("error: " + raw_data_filename + " TRAIN/TEST data file does not exist!")

    if not os.path.exists(pipeline_cfg_filename):
        sys.exit("error: " + pipeline_cfg_filename + " Pipeline Configuration file does not exist!")

    if not os.path.exists(report_folder):
        sys.exit("error: " + report_folder + " REPORT folder does not exist!")

    print("Raw data file: " + raw_data_filename)
    print("ML Pipeline Config: " + pipeline_cfg_filename)
    print("REPORT folder: " + report_folder)
    print("================================================")

    ###
    # Prepare Training/Testing Data
    ###
    # load raw data file
    raw_data = pd.read_csv(raw_data_filename)
    # loop locations in the benchmark source code
    loop_locations = raw_data['loop-location']
    # prepare real parallel labels 
    par_labels = raw_data['parallel']
    # prepare loop icc labels
    icc_labels = raw_data['icc']
    # prepare loop omp labels
    omp_labels = raw_data['omp']
    # loop application fraction times
    loop_times = raw_data['loop-app-frac-time']
    # prepare statistical learning features 
    features = raw_data.drop(['loop-location','parallel','icc','omp'], axis=1)
    # cast all integer features to float
    features = features.astype('float64')

    # parse ML pipeline configuration INI file
    pl_cfg = configparser.ConfigParser()
    pl_cfg.read(pipeline_cfg_filename)
    if ml_pipeline_config.check_ml_pipeline_config(pl_cfg) != True:
        sys.exit("error: " + pipeline_cfg_filename + " has a wrong format!")

    header_cfg = pl_cfg['header']
    if header_cfg['feature_type'] == 'static':
        # drop all dymanic features
        features = features.drop(ppar.metric_groups['dynamic'], axis=1)
    elif header_cfg['feature_type'] == 'dynamic':
        # drop all dymanic features
        features = features[ppar.metric_groups['dynamic']]
    elif header_cfg['feature_type'] != 'all':
        sys.exit("error: configuration file " + pipeline_cfg_filename + " has a wrong feature_type in its header!")

    # create and open report file
    report_fds = {}
    report_filename = report_folder + "ml_pipeline" + ".report"
    report_fd = open(report_filename,'w')
    report_fds['ml_pipeline'] = report_fd
    report_fds['accuracy'] = report_fd
    report_fds['oracle'] = report_fd

    # prepare K-fold cross validation
    K = 30
    N = 1
    kf = KFold(n_splits=K)
    rkf = RepeatedKFold(n_splits=K, n_repeats=N, random_state=ml_pipeline.get_random_state())

    train_size = (len(features)/K)*(K-1)
    test_size = len(features)/K

    baseline_accuracies = {}
    main_accuracies = []
    baseline_balanced_accuracies = {}
    main_balanced_accuracies = []
    baseline_precisions = {}
    main_precisions = []
    baseline_recalls = {}
    main_recalls = []
    baseline_f1_scores = {}
    main_f1_scores = []

    for baseline_name in ml_pipeline_config.available_baselines:
        baseline_accuracies[baseline_name] = []
        baseline_balanced_accuracies[baseline_name] = []
        baseline_precisions[baseline_name] = []
        baseline_recalls[baseline_name] = []
        baseline_f1_scores[baseline_name] = []

    case_nums = {}
    case_pars = {}
    case_npars = {}
    case_probs = {}
    for case in ['00','01','10','11']:
        case_nums[case] = []
        case_pars[case] = []
        case_npars[case] = []
        case_probs[case] = []
    safe_nums = []
    unsafe_nums = []
    icc_win_nums = []
    icc_loss_nums = []

    real_case_probs = {}
    for case in ['n_npar','p_npar','n_par','p_par']:
        real_case_probs[case] = []

    split_num = 0
    for train, test in rkf.split(raw_data):
        
        report_fd.write("= Split " + str(split_num) + " =\n") 
        report_fd.write("= =========" + " =\n") 

        split_num = split_num + 1

        # prepare vectors
        train_features = features.iloc[train]
        test_features = features.iloc[test]
        train_icc_labels = icc_labels.iloc[train]
        test_icc_labels = icc_labels.iloc[test]
        train_par_labels = par_labels.iloc[train]
        test_par_labels = par_labels.iloc[test]
        train_omp_labels = omp_labels.iloc[train]
        test_omp_labels = omp_labels.iloc[test]
        train_loop_locations = loop_locations.iloc[train]
        test_loop_locations = loop_locations.iloc[test]
        train_loop_times = loop_times.iloc[train]
        test_loop_times = loop_times.iloc[test]

        train_features = train_features.reset_index(drop=True)
        test_features = test_features.reset_index(drop=True)
        
        train_loop_locations = train_loop_locations.reset_index(drop=True)
        test_loop_locations = test_loop_locations.reset_index(drop=True)

        train_icc_labels = train_icc_labels.reset_index(drop=True)
        test_icc_labels = test_icc_labels.reset_index(drop=True)

        train_omp_labels = train_omp_labels.reset_index(drop=True)
        test_omp_labels = test_omp_labels.reset_index(drop=True)

        train_par_labels = train_par_labels.reset_index(drop=True)
        test_par_labels = test_par_labels.reset_index(drop=True)

        train_loop_times = train_loop_times.reset_index(drop=True)
        test_loop_times = test_loop_times.reset_index(drop=True)

        report_fd.write("Training data size: " + str(len(train_features)) + "\n")
        report_fd.write("Testing data size: " + str(len(test_features)) + "\n")
        report_fd.write("\n")

        # run ML Pipeline and train the model specified in the configuration file 
        # with parallel labels
        ml_pl = ml_pipeline.MLPipeline(pl_cfg, report_fd, train_features, test_features, train_par_labels)
        ml_pl.run()
        preds = ml_pl.predict()

        acc = ml_pipeline.report_results(pl_cfg, report_fds, preds, test_loop_locations, test_par_labels, test_icc_labels, test_omp_labels, test_loop_times)
 
        for baseline_name, baseline_acc in acc['baseline'].items():
            baseline_accuracies[baseline_name].append(baseline_acc['accuracy'])
            baseline_balanced_accuracies[baseline_name].append(baseline_acc['balanced_accuracy'])
            baseline_precisions[baseline_name].append(baseline_acc['precision'])
            baseline_recalls[baseline_name].append(baseline_acc['recall_score'])
            baseline_f1_scores[baseline_name].append(baseline_acc['f1_score'])
        
        main_acc = acc[pl_cfg['model_testing']['model']]
        main_accuracies.append(main_acc['accuracy'])
        main_balanced_accuracies.append(main_acc['balanced_accuracy'])
        main_precisions.append(main_acc['precision'])
        main_recalls.append(main_acc['recall_score'])
        main_f1_scores.append(main_acc['f1_score'])

        for case, case_stats in acc['icc_competition'].items():
            if case in ['00','01','10','11']:
                case_nums[case].append(case_stats['num'])
                case_pars[case].append(case_stats['par'])
                case_npars[case].append(case_stats['npar'])
                case_probs[case].append(case_stats['prob'])

        safe_nums.append(acc['icc_competition']['safe'])
        unsafe_nums.append(acc['icc_competition']['unsafe'])
        icc_win_nums.append(acc['icc_competition']['icc-win'])
        icc_loss_nums.append(acc['icc_competition']['icc-loss'])

        for case, case_prob in acc['oracle'].items():
            if case in ['n_npar','p_npar','n_par','p_par']:
                real_case_probs[case].append(case_prob)

    report_fd.write("Final K-Fold (K=" + str(K) +") Cross Validation Mean Accuracy\n")
    report_fd.write("===================" + "\n")
    report_fd.write("\n")
    report_fd.write("= SciKitLearn Dummy (Baseline) Predictor =" + "\n")
    report_fd.write("= =========================== =" + "\n")

    for baseline_name in ml_pipeline_config.available_baselines:
        report_fd.write("type: " + baseline_name + "\n")
        report_fd.write("mean-accuracy:" + "{0:.3f}".format(pd.Series(baseline_accuracies[baseline_name]).mean()*100) + "\n")
        report_fd.write("mean-balanced-accuracy:" + "{0:.3f}".format(pd.Series(baseline_balanced_accuracies[baseline_name]).mean()*100) + "\n")
        report_fd.write("mean-precision:" + "{0:.3f}".format(pd.Series(baseline_precisions[baseline_name]).mean()*100) + "\n")
        report_fd.write("mean-recall:" + "{0:.3f}".format(pd.Series(baseline_recalls[baseline_name]).mean()*100) + "\n")
        report_fd.write("mean-f1-score:" + "{0:.3f}".format(pd.Series(baseline_f1_scores[baseline_name]).mean()*100) + "\n")
        report_fd.write("= =========================== =" + "\n")
        report_fd.write("\n")

    report_fd.write("= SciKitLearn Main Score Report =" + "\n")
    report_fd.write("= ============================= =" + "\n")
    report_fd.write("mean-accuracy:" + "{0:.3f}".format(pd.Series(main_accuracies).mean()*100) + "\n")
    report_fd.write("mean-balanced-accuracy:" + "{0:.3f}".format(pd.Series(main_balanced_accuracies).mean()*100) + "\n")
    report_fd.write("mean-precision:" + "{0:.3f}".format(pd.Series(main_precisions).mean()*100) + "\n")
    report_fd.write("mean-recall:" + "{0:.3f}".format(pd.Series(main_recalls).mean()*100) + "\n")
    report_fd.write("mean-f1-score:" + "{0:.3f}".format(pd.Series(main_f1_scores).mean()*100) + "\n")
    report_fd.write("= =========================== =" + "\n")
    report_fd.write("\n")

    report_fd.write("= ICC Competition Main Report =" + "\n")
    report_fd.write("= ============================= =" + "\n\n")
    report_fd.write("train-size: " + "{0:.3f}".format(train_size) + "\n")
    report_fd.write("test-size: " + "{0:.3f}".format(test_size) + "\n")

    par_avg_num = 0.0
    for case in ['00','01','10','11']:
        par_avg_num += pd.Series(case_pars[case]).mean()
    report_fd.write("par-avg-num: " + "{0:.3f}".format(par_avg_num) + "\n")
    
    p_pred_avg_num = 0.0
    for case in ['01','11']:
        p_pred_avg_num += pd.Series(case_nums[case]).mean()
    report_fd.write("p-pred-avg-num: " + "{0:.3f}".format(p_pred_avg_num) + "\n\n")

    p_icc_avg_num = 0.0
    for case in ['10','11']:
        p_icc_avg_num += pd.Series(case_nums[case]).mean()
    report_fd.write("p-icc-avg-num: " + "{0:.3f}".format(p_icc_avg_num) + "\n\n")

    for case in ['00','01','10','11']:
        report_fd.write("=== case [" + case + "] ===" + "\n")
        report_fd.write("avg-num: " + "{0:.3f}".format(pd.Series(case_nums[case]).mean()) + "\n")
        report_fd.write("avg-par: " + "{0:.3f}".format(pd.Series(case_pars[case]).mean()) + "\n")
        report_fd.write("avg-npar: " + "{0:.3f}".format(pd.Series(case_npars[case]).mean()) + "\n")
        report_fd.write("avg-prob: " + "{0:.3f}".format(pd.Series(case_probs[case]).mean()) + "\n")
    report_fd.write("\n")
    report_fd.write("=== Stats  ===" + "\n")
    report_fd.write("avg-safe-num: " + "{0:.3f}".format(pd.Series(safe_nums).mean()*100) + "\n")
    report_fd.write("avg-unsafe-num: " + "{0:.3f}".format(pd.Series(unsafe_nums).mean()*100) + "\n")
    report_fd.write("avg-icc-win-num: " + "{0:.3f}".format(pd.Series(icc_win_nums).mean()*100) + "\n")
    report_fd.write("avg-icc-loss-num: " + "{0:.3f}".format(pd.Series(icc_loss_nums).mean()*100) + "\n")
    report_fd.write("= ============================= =" + "\n")
    report_fd.write("\n")
    report_fd.write("=== Probs ===" + "\n")
    for case in ['n_npar','p_npar','n_par','p_par']:
        report_fd.write("=== case [" + case + "] ===" + "\n")
        report_fd.write("avg-prob-1: " + "{0:.3f}".format(pd.Series(real_case_probs[case]).mean()) + "\n")
    report_fd.write("===================" + "\n")
    report_fd.write("\n")

    # close report file
    report_fd.close()

    sys.exit(0)
