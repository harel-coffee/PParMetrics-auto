#! /usr/bin/python3

import sys
import math

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

def euclid_distance_3d(pointA, pointB):
    return math.sqrt(math.pow(pointA[0]-pointB[0],2) +
                     math.pow(pointA[1]-pointB[1],2) +
                     math.pow(pointA[2]-pointB[2],2))

if __name__ == "__main__":

    print("=== Single metric vs loop parallelisability visualization ===")
    raw_data_filename = sys.argv[1]
    report_folder = sys.argv[2]
    std_num = int(sys.argv[3]) # outliers screening parameter
    print("Raw data input: " + raw_data_filename)
    print("Report folder: " + report_folder)
    
    # load raw data file
    data = pd.read_csv(raw_data_filename)
    # loop locations in benchmark source code
    loop_locations = data['loop-location']
    # prepare statistical learning labels 
    loop_icc_classifications = data['icc-parallel']
    # prepare statistical learning features 
    features = data.drop(['loop-location','icc-parallel'], axis=1)
    features_abs = features.copy()

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
        loop_locations = loop_locations[filtered_idx]
        idxs_to_drop = features.index.drop(filtered_idx)
        features = features.drop(idxs_to_drop)
        features_abs = features_abs.drop(idxs_to_drop)

        loop_icc_classifications = loop_icc_classifications.reset_index(drop=True)
        loop_locations = loop_locations.reset_index(drop=True)
        features = features.reset_index(drop=True)
        features_abs = features_abs.reset_index(drop=True)
    
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

    # map features onto 3d and 2d with the help of PCA
    pca = PCA(n_components=3)
    pca.fit(features)
    features_3d = pca.transform(features)

    pca = PCA(n_components=2)
    pca.fit(features)
    features_2d = pca.transform(features)

    # plot the 3d PCA-mapped dataset 
    fig1 = plt.figure(1)
    ax3d = fig1.add_subplot(2, 2, 1, projection='3d')
    ax3d_xy = fig1.add_subplot(2, 2, 2)
    ax3d_xz = fig1.add_subplot(2, 2, 3)
    ax3d_yz = fig1.add_subplot(2, 2, 4)

    ax3d.set_xlabel('x')
    ax3d.set_ylabel('y')
    ax3d.set_zlabel('z')

    ax3d_xy.set_xlabel('x')
    ax3d_xy.set_ylabel('y')

    ax3d_xz.set_xlabel('x')
    ax3d_xz.set_ylabel('z')

    ax3d_yz.set_xlabel('y')
    ax3d_yz.set_ylabel('z')

    ax3d.scatter((features_3d.transpose())[0], (features_3d.transpose())[1], (features_3d.transpose())[2], marker='.')
    ax3d_xy.scatter((features_3d.transpose())[0], (features_3d.transpose())[1], marker='.')
    ax3d_xz.scatter((features_3d.transpose())[0], (features_3d.transpose())[2], marker='.')
    ax3d_yz.scatter((features_3d.transpose())[1], (features_3d.transpose())[2], marker='.')

    fig1.show()

    # plot the 2d PCA-mapped dataset 
    fig2 = plt.figure(2)
    ax2d = fig2.add_subplot(1, 1, 1)
    ax2d.set_xlabel('x')
    ax2d.set_ylabel('y')

    ax2d.scatter((features_2d.transpose())[0], (features_2d.transpose())[1], marker='.')

    fig2.show()

    # plot parallelisability labels onto projections 
    colors = []
    for i in range(0,len(loop_icc_classifications)):
        if loop_icc_classifications[i] == 1:
            colors.append('green')
        else:
            colors.append('red')

    # plot the 3d PCA-mapped dataset 
    fig3 = plt.figure(3)
    ax3d = fig3.add_subplot(1, 2, 1, projection='3d')
    ax2d = fig3.add_subplot(1, 2, 2)

    ax3d.set_xlabel('x')
    ax3d.set_ylabel('y')
    ax3d.set_zlabel('z')

    ax2d.set_xlabel('x')
    ax2d.set_ylabel('y')

    ax3d.scatter((features_3d.transpose())[0], (features_3d.transpose())[1], (features_3d.transpose())[2], c=colors, marker='.')
    
    ax2d.scatter((features_2d.transpose())[0], (features_2d.transpose())[1], c=colors, marker='.')

    fig3.show()

    # compute clusters for 3d PCA projection
    
    kmeans3d = KMeans(n_clusters=4)
    kmeans3d.fit(features_3d)
    labels3d = kmeans3d.labels_
    centers3d = kmeans3d.cluster_centers_

    # plot the 3d PCA-mapped dataset 
    fig4 = plt.figure(4)
    ax3d = fig4.add_subplot(1, 2, 1, projection='3d')
    ax2d = fig4.add_subplot(1, 2, 2)

    ax3d.set_xlabel('x')
    ax3d.set_ylabel('y')
    ax3d.set_zlabel('z')

    ax2d.set_xlabel('x')
    ax2d.set_ylabel('y')

    ax3d.scatter((features_3d[labels3d == 0].transpose())[0], (features_3d[labels3d == 0].transpose())[1], (features_3d[labels3d == 0].transpose())[2], c='red', marker='.')
    ax3d.scatter((features_3d[labels3d == 1].transpose())[0], (features_3d[labels3d == 1].transpose())[1], (features_3d[labels3d == 1].transpose())[2], c='green', marker='.')
    ax3d.scatter((features_3d[labels3d == 2].transpose())[0], (features_3d[labels3d == 2].transpose())[1], (features_3d[labels3d == 2].transpose())[2], c='blue', marker='.')
    ax3d.scatter((features_3d[labels3d == 3].transpose())[0], (features_3d[labels3d == 3].transpose())[1], (features_3d[labels3d == 3].transpose())[2], c='yellow', marker='.')
    
    ax3d.scatter(centers3d[0][0], centers3d[0][1], centers3d[0][2], c='red', marker='v')
    ax3d.scatter(centers3d[1][0], centers3d[1][1], centers3d[1][2], c='green', marker='v')
    ax3d.scatter(centers3d[2][0], centers3d[2][1], centers3d[2][2], c='blue', marker='v')
    ax3d.scatter(centers3d[3][0], centers3d[3][1], centers3d[3][2], c='yellow', marker='v')

    ax2d.scatter((features_3d[labels3d == 0].transpose())[0], (features_3d[labels3d == 0].transpose())[1], c='red', marker='.')
    ax2d.scatter((features_3d[labels3d == 1].transpose())[0], (features_3d[labels3d == 1].transpose())[1], c='green', marker='.')
    ax2d.scatter((features_3d[labels3d == 2].transpose())[0], (features_3d[labels3d == 2].transpose())[1], c='blue', marker='.')
    ax2d.scatter((features_3d[labels3d == 3].transpose())[0], (features_3d[labels3d == 3].transpose())[1], c='yellow', marker='.')

    ax2d.scatter(centers3d[0][0], centers3d[0][1], c='red', marker='v')
    ax2d.scatter(centers3d[1][0], centers3d[1][1], c='green', marker='v')
    ax2d.scatter(centers3d[2][0], centers3d[2][1], c='blue', marker='v')
    ax2d.scatter(centers3d[3][0], centers3d[3][1], c='yellow', marker='v')

    fig4.show()
   
#    plt.show()

    # save all plots into reports directory
    fig1.savefig(report_folder + "/" + "metrics-pca-13d-to-3d")
    fig2.savefig(report_folder + "/" + "metrics-pca-13d-to-2d")
    fig3.savefig(report_folder + "/" + "metrics-3d-parallelisability")
    fig4.savefig(report_folder + "/" + "metrics-clustering")

    # report sets of loops in different clusters
    orig_stdout = sys.stdout
    f = open(report_folder + '/' + 'loop-clusters.report', 'w')
    sys.stdout = f

    print("loop-location:", "cluster:", "cluster-center-distance:", end="")
    for metric in ppar.metric_list:
        print(metric + ":", end="")
    print("")
    
    loops = [None]*4
    loop_features = [None]*4
    loop_metrics = [None]*4
    
    for i in [0,1,2,3]:
        loops[i] = loop_locations[labels3d == i]
        loop_features[i] = pd.Series(features_3d.tolist())
        loop_features[i] = loop_features[i][labels3d == i]
        loop_metrics[i] = features_abs[labels3d == i]
        loops[i] = loops[i].reset_index(drop=True)
        loop_features[i] = loop_features[i].reset_index(drop=True)
        loop_metrics[i] = loop_metrics[i].reset_index(drop=True)
       
        for j in range(0,len(loops[i])):
            dist = euclid_distance_3d(loop_features[i][j], centers3d[i])
            print(loops[i][j] + ":" + str(i) + ":" + "{0:.2f}".format(dist) + ":", end="")
            for metric in ppar.metric_list:
                m = loop_metrics[i][metric][j]
                print("{0:.2f}".format(m) + ":", end="")
            print("")
    sys.stdout = orig_stdout
    f.close()
    
    cluster_color = ['red','green','blue','yellow']
    # visualise parallelisability vs single metrics
    for metric in ppar.metric_list:
        fig, ax = plt.subplots()
        ax.set_ylabel(metric)
        ax.set_xlabel("benchmarks")
        
        for i in [0,1,2,3]:
            d = loop_metrics[i]
            d = d[metric]

            ax.scatter(d.index, d, s=1, c=cluster_color[i], marker=".", alpha=1.0)
            m = d.mean()
            disp = (d.max()-d.min())/100 # 1% of x range
            ax.axhline(m, c=cluster_color[i], linestyle='--', linewidth=0.5)
            ax.text((d.index.min()+d.index.max())/2, m+disp, "{0:.2f}".format(m), fontsize=6, color=cluster_color[i])

        fig.savefig(report_folder + "/" + metric + "-clusters" , transparent=False, dpi=320, bbox_inches="tight")
    
    input()
