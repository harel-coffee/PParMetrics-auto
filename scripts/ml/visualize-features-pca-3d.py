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
    report_folder = sys.argv[2]
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

    # remove outliers from the data
    if std_num != 0:
        filtered_idxs = {}
        for metric in ppar.metric_list:  
            d = features[metric]
            filtered_idxs[metric] = features[abs(d-d.mean()) < std_num*d.std()].index
    
        filtered_idx = features.index
        for metric in ppar.metric_list:  
            filtered_idx &= filtered_idxs[metric]

        loop_icc_classifications = loop_icc_classifications[filtered_idx]
        idxs_to_drop = features.index.drop(filtered_idx)
        features = features.drop(idxs_to_drop)

        loop_icc_classifications = loop_icc_classifications.reset_index(drop=True)
        features = features.reset_index(drop=True)

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

    pca = PCA(n_components=3)
    pca.fit(features)
    features_3d = pca.transform(features)
 
    fig = plt.figure()
    ax3d = fig.add_subplot(111, projection='3d')

    colors = []
    for i in range(0,len(loop_icc_classifications)):
        if loop_icc_classifications[i] == 1:
            colors.append('green')
        else:
            colors.append('red')

    #ax3d.scatter((features_3d.transpose())[0], (features_3d.transpose())[1], (features_3d.transpose())[2], c=colors, marker='o')
    
    kmeans = KMeans(n_clusters=3)
    kmeans.fit(features_3d)
    labels = kmeans.labels_

    colors_0 = loop_icc_classifications[labels == 0].tolist()
    colors_1 = loop_icc_classifications[labels == 1].tolist()
    colors_2 = loop_icc_classifications[labels == 2].tolist()

    for i in range(0,len(colors_0)):
        if colors_0[i] == 0:
            colors_0[i] = 'red'
        else:
            colors_0[i] = 'green'
    
    for i in range(0,len(colors_1)):
        if colors_1[i] == 0:
            colors_1[i] = 'red'
        else:
            colors_1[i] = 'green'

    for i in range(0,len(colors_2)):
        if colors_2[i] == 0:
            colors_2[i] = 'red'
        else:
            colors_2[i] = 'green'

    ax3d.scatter((features_3d[labels == 0].transpose())[0], (features_3d[labels == 0].transpose())[1], (features_3d[labels == 0].transpose())[2], c=colors_0, marker='v')
    ax3d.scatter((features_3d[labels == 1].transpose())[0], (features_3d[labels == 1].transpose())[1], (features_3d[labels == 1].transpose())[2], c=colors_1, marker='|')
    ax3d.scatter((features_3d[labels == 2].transpose())[0], (features_3d[labels == 2].transpose())[1], (features_3d[labels == 2].transpose())[2], c=colors_2, marker='.')
   
    plt.show()
