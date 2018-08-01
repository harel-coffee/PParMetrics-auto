#! /usr/bin/python3

import sys

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from mpl_toolkits.mplot3d import Axes3D
from sklearn.decomposition import PCA as sklearnPCA
from sklearn.cluster import KMeans
from sklearn.datasets.samples_generator import make_blobs

def reject_outliers(data, m=2):
    return data[abs(data - np.mean(data)) < m * np.std(data)]

import ppar

if __name__ == "__main__":

    print("=== Single metric vs loop parallelisability visualization ===")
    raw_data_filename = sys.argv[1]
    print("Raw data input: " + raw_data_filename)
    
    # load raw data file
    data = pd.read_csv(raw_data_filename)
    # loop locations in benchmark source code
    loop_locations = data['loop-location']
    # prepare statistical learning labels 
    loop_icc_classifications = data['icc-parallel']
    # prepare statistical learning features 
    features = data.drop(['loop-location','icc-parallel'], axis=1)

    # prepare data for different metric groups
    metric_group_data = {}
    for metric_group in ppar.metric_groups:
        metric_group_data[metric_group] = features[ppar.metric_groups[metric_group]]

    metric_data = {}
    for metric in ppar.metric_list:
        metric_data[metric] = features[metric]

    for metric in ppar.metric_list:
        
        fig, ax = plt.subplots()
        
        ax.set_ylabel(metric)
        ax.set_xlabel("benchmarks")
        
        # remove outliers out of the set
        std_num = 1 # number of standard deviations
        d = metric_data[metric]
        x = d[abs(d-d.mean()) < std_num*d.std()]
        y = loop_icc_classifications[abs(d-d.mean()) < std_num*d.std()]
        
        # scatter non-parallelisible points over the figure 
        ax.scatter((x[y==0]).index, x[y==0], s=1, c='red', marker=".", alpha=1.0)
        ax.axhline((x[y==0]).mean(), c='red', linestyle='--', linewidth=0.5)

        # scatter parallelisible points over the figure 
        ax.scatter((x[y==1]).index, x[y==1], s=1, c='green', marker="+", alpha=1.0)
        ax.axhline((x[y==1]).mean(), c='green', linestyle='--', linewidth=0.5)

        fig.savefig(metric, transparent=False, dpi=320, bbox_inches="tight")
