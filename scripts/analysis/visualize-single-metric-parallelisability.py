#! /usr/bin/python3

import sys

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from mpl_toolkits.mplot3d import Axes3D
from sklearn.decomposition import PCA
from sklearn.cluster import KMeans
from sklearn.datasets.samples_generator import make_blobs

def reject_outliers(data, m=2):
    return data[abs(data - np.mean(data)) < m * np.std(data)]

import ppar

if __name__ == "__main__":

    print("=== Single metric vs loop parallelisability visualization ===")
    raw_data_filename = sys.argv[1]
    report_folder = sys.argv[2] # outliers screening parameter
    std_num = int(sys.argv[3]) # outliers screening parameter
    print("Raw data input: " + raw_data_filename)
    
    # load raw data file
    data = pd.read_csv(raw_data_filename)
    # loop locations in benchmark source code
    loop_locations = data['loop-location']
    # prepare statistical learning labels 
    loop_icc_classifications = data['icc-parallel']
    # prepare statistical learning features 
    features = data.drop(['loop-location','icc-parallel'], axis=1)

    # normalize the data
    for feature in ppar.metric_list:
        features[feature] = (features[feature] - features[feature].min())/(features[feature].max() - features[feature].min())

    # prepare data for different metric groups
    metric_group_data = {}
    for metric_group in ppar.metric_groups:
        metric_group_data[metric_group] = features[ppar.metric_groups[metric_group]]

    metric_data = {}
    for metric in ppar.metric_list:
        metric_data[metric] = features[metric]

    # visualise parallelisability vs single metrics
    for metric in ppar.metric_list:
        
        fig, ax = plt.subplots()
        
        ax.set_ylabel(metric)
        ax.set_xlabel("benchmarks")
        
        # remove outliers out of the set
        d = metric_data[metric]
        if std_num != 0:
            x = d[abs(d-d.mean()) < std_num*d.std()]
            y = loop_icc_classifications[abs(d-d.mean()) < std_num*d.std()]
        else:
            x = d
            y = loop_icc_classifications

        # scatter non-parallelisible points over the figure 
        ax.scatter((x[y==0]).index, x[y==0], s=1, c='red', marker=".", alpha=1.0)
        m = (x[y==0]).mean()
        disp = (x.max()-x.min())/100 # 1% of x range
        ax.axhline(m, c='red', linestyle='--', linewidth=0.5)
        ax.text((y.index.min()+y.index.max())/2-100, m+disp, "np ~= "+"{0:.2f}".format((x[y==0]).mean()), fontsize=6, color='red')

        # scatter parallelisible points over the figure 
        ax.scatter((x[y==1]).index, x[y==1], s=1, c='green', marker="+", alpha=1.0)
        ax.axhline((x[y==1]).mean(), c='green', linestyle='--', linewidth=0.5, label=x[y==1].mean())
        m = (x[y==1]).mean()
        ax.text((y.index.min()+y.index.max())/2+100, m+disp, "p ~= "+"{0:.2f}".format((x[y==1]).mean()), fontsize=6, color='green')

        fig.savefig(report_folder+metric, transparent=False, dpi=320, bbox_inches="tight")
