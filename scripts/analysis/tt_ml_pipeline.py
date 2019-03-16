#!/usr/bin/python3

import sys
import os
import configparser

import pandas as pd
import numpy as np

import ppar
import ml_pipeline_config 
import ml_pipeline 

if __name__ == "__main__":

    print("================================================")
    print("=== Machine Learning Train/Test Pipeline Run ===")
    print("================================================")

    if len(sys.argv) != 5:
        sys.exit("error: use ./tt-ml-pipeline.py " +
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
        sys.exit("error: " + pipeline_cfg_filename + " pipeline configuration file does not exist!")

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
    # prepare loop omp labels
    train_omp_labels = train_data['omp']
    # loop application fraction times
    train_loop_times = train_data['loop-app-frac-time']
    # prepare statistical learning features 
    train_features = train_data.drop(['loop-location','parallel','icc','omp'], axis=1)
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
    # prepare loop omp labels
    test_omp_labels = test_data['omp']
    # loop application fraction times
    test_loop_times = test_data['loop-app-frac-time']
    test_loop_times /= 100
    # prepare statistical learning features 
    test_features = test_data.drop(['loop-location','parallel','icc','omp'], axis=1)
    # cast all integer features to float
    test_features = test_features.astype('float64')
  
    ###
    # Parse ML Pipeline Configuration INI file
    ###
    pl_cfg = configparser.ConfigParser()
    pl_cfg.read(pipeline_cfg_filename)
    if ml_pipeline_config.check_ml_pipeline_config(pl_cfg) != True:
        sys.exit("error: " + pipeline_cfg_filename + " has a wrong format!")

    header_cfg = pl_cfg['header']
    if header_cfg['feature_type'] == 'static':
        # drop all dymanic features
        test_features = test_features.drop(ppar.metric_groups['dynamic'], axis=1)
        train_features = train_features.drop(ppar.metric_groups['dynamic'], axis=1)
    elif header_cfg['feature_type'] == 'dynamic':
        # drop all dymanic features
        test_features = test_features[ppar.metric_groups['dynamic']]
        train_features = train_features[ppar.metric_groups['dynamic']]
    elif header_cfg['feature_type'] != 'all':
        sys.exit("error: configuration file " + pipeline_cfg_filename + " has a wrong feature_type in its header!")

    ###
    # Prepare and open all report files to write
    ###
    report_fds = {}
    # create and open report files
    report_filename = report_folder + "tt_ml_pipeline" + ".report"
    report_fd = open(report_filename,'w')
    report_fds['ml_pipeline'] = report_fd

    # create and open report file
    report_filename = report_folder + "accuracy" + ".report"
    report_fd = open(report_filename,'w')
    report_fds['accuracy'] = report_fd

    # create and open report file
    report_filename = report_folder + "oracle" + ".report"
    report_fd = open(report_filename,'w')
    report_fds['oracle'] = report_fd

    # run ML Pipeline and train the model specified in the configuration file 
    # with parallel labels
    ml_pl = ml_pipeline.MLPipeline(pl_cfg, report_fds['ml_pipeline'], train_features, test_features, train_par_labels)
    ml_pl.run()
    preds = ml_pl.predict()

    acc = ml_pipeline.report_results(pl_cfg, report_fds, preds, test_loop_locations, test_par_labels, test_icc_labels, test_omp_labels, test_loop_times)

    report_fd = report_fds['accuracy']

    report_fd.write("Final Test/Train Mean Accuracy\n")
    report_fd.write("===================" + "\n")
    report_fd.write("\n")
    report_fd.write("= SciKitLearn Dummy (Baseline) Predictor =" + "\n")
    report_fd.write("= =========================== =" + "\n")

    for baseline_name, baseline_acc in acc['baseline'].items():
        report_fd.write("type: " + baseline_name + "\n")
        report_fd.write("mean-accuracy:" + "{0:.3f}".format(baseline_acc['accuracy']) + "\n")
        report_fd.write("mean-balanced-accuracy:" + "{0:.3f}".format(baseline_acc['balanced_accuracy']) + "\n")
        report_fd.write("mean-precision:" + "{0:.3f}".format(baseline_acc['precision']) + "\n")
        report_fd.write("mean-recall:" + "{0:.3f}".format(baseline_acc['recall_score']) + "\n")
        report_fd.write("mean-f1-score:" + "{0:.3f}".format(baseline_acc['f1_score']) + "\n")
        report_fd.write("\n")
        report_fd.write("\n")
    report_fd.write("= ============================= =" + "\n")

    main_acc = acc[pl_cfg['model_testing']['model']]
   
    report_fd.write("= SciKitLearn Main Score Report =" + "\n")
    report_fd.write("= ============================= =" + "\n")
    report_fd.write("mean-accuracy:" + "{0:.3f}".format(main_acc['accuracy']) + "\n")
    report_fd.write("mean-balanced-accuracy:" + "{0:.3f}".format(main_acc['balanced_accuracy']) + "\n")
    report_fd.write("mean-precision:" + "{0:.3f}".format(main_acc['precision']) + "\n")
    report_fd.write("mean-recall:" + "{0:.3f}".format(main_acc['recall_score']) + "\n")
    report_fd.write("mean-f1-score:" + "{0:.3f}".format(main_acc['f1_score']) + "\n")
    report_fd.write("= =========================== =" + "\n")
    report_fd.write("\n")

    report_fd.write("===================" + "\n")
    report_fd.write("\n")

    # close all report file descriptors
    for fd_name, report_fd in report_fds.items():
        report_fd.close()

    sys.exit(0)
