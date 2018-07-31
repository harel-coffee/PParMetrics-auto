#! /usr/bin/python3

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from mpl_toolkits.mplot3d import Axes3D
from sklearn.decomposition import PCA as sklearnPCA
from sklearn.cluster import KMeans
from sklearn.datasets.samples_generator import make_blobs

def reject_outliers(data, m=2):
    return data[abs(data - np.mean(data)) < m * np.std(data)]

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

for metric in metrics_list:

    kwargs = dict(histtype='stepfilled', alpha=0.3, normed=True, bins=100)

    d = reject_outliers(metric_data[metric],2)
    plt.title(metric)
    plt.hist(d[loop_icc_classifications == 0], **kwargs, color='red')
    plt.hist(d[loop_icc_classifications == 1], **kwargs, color='green')
    plt.show()
