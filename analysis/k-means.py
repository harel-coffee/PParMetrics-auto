#! /usr/bin/python3

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from sklearn.decomposition import PCA as sklearnPCA
from sklearn.cluster import KMeans
from sklearn.datasets.samples_generator import make_blobs

# load original metrics+icc report file
print("Loading metrics+icc.csv file")
data_report = pd.read_csv('/home/s1736883/Work/PParMetrics/analysis/data.csv')
print("Data loading is done!")

loop_locations = data_report['loop-location']
loop_classifications = data_report['ICC-parallel']

data_kmeans = data_report.drop(['loop-location','ICC-parallel'], axis=1)

# Number of clusters
kmeans = KMeans(n_clusters=2)
# Fitting the input data
kmeans.fit(data_kmeans)
# Getting the cluster labels
#labels = kmeans.predict(X)
# Centroid values
#centroids = kmeans.cluster_centers_

#print(centroids)

#pca = sklearnPCA(n_components=3) # 3-dimensional PCA
#transformed = pd.DataFrame(pca.fit_transform(data_kmeans))

#plt.scatter(transformed[0], transformed[1], transformed[2], label='non-parallel', c='red')

#plt.legend()
#plt.show()
