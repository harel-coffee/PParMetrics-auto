#! /usr/bin/python3

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from mpl_toolkits.mplot3d import Axes3D
from sklearn.decomposition import PCA as sklearnPCA
from sklearn.cluster import KMeans
from sklearn.datasets.samples_generator import make_blobs

# prepare a set of metrics for further reference
metrics = {}
metrics['loop-proportion'] = ['loop-absolute-size', 
                              'loop-payload-fraction', 
                              'loop-proper-sccs-number']
metrics['iterator-payload-cohesion'] = ['iterator-payload-total-cohesion', 
                                        'iterator-payload-non-cf-cohesion', 
                                        'iterator-payload-mem-cohesion']
metrics['critical-payload-cohesion'] = ['critical-payload-total-cohesion', 
                                        'critical-payload-non-cf-cohesion', 
                                        'critical-payload-mem-cohesion']
metrics['payload-dependencies-number'] = ['payload-total-dependencies-number', 
                                          'payload-true-dependencies-number', 
                                          'payload-anti-dependencies-number', 
                                          'payload-output-dependencies-number']
metrics['critical-payload-dependencies-number'] = ['critical-payload-total-dependencies-number', 
                                                   'critical-payload-true-dependencies-number', 
                                                   'critical-payload-anti-dependencies-number'] 
metrics['all'] = []
for metrics_list in metrics:
    if metrics_list != 'all':
        metrics['all'].extend(metrics[metrics_list])
metrics_list = metrics['all'] 
        
# load original metrics+icc report file
data = pd.read_csv('/home/s1736883/Work/PParMetrics/analysis/raw_data.csv')
loop_locations = data['loop-location']
loop_icc_classifications = data['ICC-parallel']
data_kmeans = data.drop(['loop-location','ICC-parallel'], axis=1)

training_set_size = 1000

# normalize the data
#data_kmeans = (data_kmeans-data_kmeans.mean())/data_kmeans.std()

# prepare data for different metric groups
metrics_data = {}
for metric_group in metrics:
    metrics_data[metric_group] = data_kmeans[metrics[metric_group]]

# prepare data for single metrics
metric_data = {}
for metric in metrics_list:
    metric_data[metric] = data_kmeans[metric]

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.scatter(metric_data['loop-absolute-size'][loop_icc_classifications == 0],
           metric_data['loop-payload-fraction'][loop_icc_classifications == 0],
           metric_data['loop-proper-sccs-number'][loop_icc_classifications == 0],
           c='red',marker=".")
ax.scatter(metric_data['loop-absolute-size'][loop_icc_classifications == 1],
           metric_data['loop-payload-fraction'][loop_icc_classifications == 1],
           metric_data['loop-proper-sccs-number'][loop_icc_classifications == 1],
           c='green',marker=".")
ax.set_xlabel('loop-absolute-size')
ax.set_ylabel('loop-payload-fraction')
ax.set_zlabel('loop-proper-sccs-number')
plt.show()
  
'''
for metric in metrics_list:
    plt.xlabel(metric)
    plt.yticks([0,1])
    plt.scatter(metric_data[metric][loop_icc_classifications == 0], np.zeros((1,len(metric_data[metric][loop_icc_classifications == 0]))), c='red',marker=".")
    plt.scatter(metric_data[metric][loop_icc_classifications == 1], np.ones((1,len(metric_data[metric][loop_icc_classifications == 1]))), c='green')
    plt.show()
'''
'''

for metric_group in metrics:
    print("")
    print("doing 2-mean clustering for " + metric_group + " metric group")
    kmeans = KMeans(n_clusters=2)
    # fitting the input data
    kmeans.fit(metrics_data[metric_group])
    # centroid values
    centroids = kmeans.cluster_centers_
    print("centroinds:")
    print(centroids)
    print("labels:")
    labels = kmeans.labels_
    print(labels)
    error=0

    for i in range(len(loop_icc_classifications)-1):
        for j in range(i+1,len(loop_icc_classifications)-1):
            if loop_icc_classifications[i] != loop_icc_classifications[j]:
                if labels[i] == labels[j]:
                    error += 1
            else:
                if labels[i] != labels[j]:
                    error += 1
                
    print("error is " + str((2*error/(len(loop_icc_classifications)*len(loop_icc_classifications)))*100) + "%")
    print("")
    '''
'''
for metric in metrics_list:
    print("")
    print("doing 2-mean clustering for " + metric + " metric")
    kmeans = KMeans(n_clusters=2)
    # fitting the input data
    kmeans.fit(metric_data[metric].values.reshape(-1, 1))
    # centroid values
    centroids = kmeans.cluster_centers_
    print("centroinds:")
    print(centroids)
    print("labels:")
    labels = kmeans.labels_
    print(labels)
    error=0
    for i in range(len(loop_icc_classifications)-1):
        for j in range(i+1,len(loop_icc_classifications)-1):
            if loop_icc_classifications[i] != loop_icc_classifications[j]:
                if labels[i] == labels[j]:
                    error += 1
            else:
                if labels[i] != labels[j]:
                    error += 1
                
    print("error is " + str((2*error/(len(loop_icc_classifications)*len(loop_icc_classifications)))*100) + "%")
    print("")
'''
#training_data = (metrics_data[metric_group])[:training_set_size]
#testing_data = (metrics_data[metric_group])[training_set_size:]

# Number of clusters
#kmeans = KMeans(n_clusters=2)
# Fitting the input data
#kmeans.fit(training_data)
# Centroid values
#centroids = kmeans.cluster_centers_

# Getting the cluster labels
#labels = kmeans.predict(testing_data)
# Centroid values
#centroids = kmeans.cluster_centers_

#print(centroids)

#pca = sklearnPCA(n_components=3) # 3-dimensional PCA
#transformed = pd.DataFrame(pca.fit_transform(data_kmeans))

#plt.scatter(transformed[0], transformed[1], transformed[2], label='non-parallel', c='red')

#plt.legend()
#plt.show()
