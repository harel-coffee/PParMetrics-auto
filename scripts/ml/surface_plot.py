#! /usr/bin/python3

from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
import numpy as np
import math

def sigmoid(x):
    return 1/(1 + np.exp(7*(-x+0.5)))

fig = plt.figure()
ax = fig.gca(projection='3d')

# Make data.
prob = np.arange(0, 1, 0.05)
rt = np.arange(0, 1, 0.05)
prob, rt = np.meshgrid(prob, rt)
Z = np.power(rt,1.0)*sigmoid(prob)

# Plot the surface.
surf = ax.plot_surface(prob, rt, Z, cmap=cm.coolwarm, linewidth=0, antialiased=False)

plt.ylabel('rt')
plt.xlabel('prob')

# Customize the z axis.
ax.set_zlim(-1.01, 1.01)
ax.zaxis.set_major_locator(LinearLocator(10))
ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))

# Add a color bar which maps values to colors.
fig.colorbar(surf, shrink=0.5, aspect=5)

plt.show()
