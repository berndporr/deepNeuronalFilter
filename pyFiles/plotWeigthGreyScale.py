#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jul  4 12:43:52 2019

@author: Sama
"""
import numpy as np
import matplotlib.pylab as plt

plt.rcParams['font.family'] = 'serif'
plt.rcParams.update({'font.size': 9})
plt.close("all")

myData = 12
fileName = ['', '1',  '2',  '3', '4', '5', '6', '7', '8', '9', '10', '11', '12']
print('running from cppData', fileName[myData])

subject = 14
trial = ['closed', 'open']
total_layers = 2

for tri in trial:
    for layerIndex in range(1, total_layers+1, 1):
        data = np.loadtxt('../../cppData{}/subject{}/grayLayer{}_{}_subject{}.csv'
                          .format(fileName[myData], subject, layerIndex, tri, subject), dtype=float)
        numNeurons = data.shape[0]
        numInputs = data.shape[1]
        # dataNormTemp = data - data.min()
        dataNorm = data / abs(data).max()
        fig1 = plt.figure('x subject{}, eyes {}, layer {}'.format(subject, tri, layerIndex))
        ax1 = fig1.add_subplot(111)
        for i in range(dataNorm.shape[0]):
            ax1.plot(dataNorm[i, :])
            plt.title('subject{}, eyes {}'.format(subject, tri))
        plt.show()
        fig = plt.figure('subject{}, eyes {}, layer {}'.format(subject, tri, layerIndex))
        ax = fig.add_subplot(111)
        myImage = ax.imshow(dataNorm, cmap='bone', interpolation='none')
        fig.colorbar(myImage, ax=ax)
        plt.gca().set_yticks(np.arange(0, numNeurons, 2))
        plt.gca().set_xticks(np.arange(0, numInputs, 5))
        ax.set_aspect(aspect=2)
        plt.title('subject{}, eyes {}'.format(subject, tri))
        plt.show()
        fig.savefig('../../cppData' + fileName[myData] + '/subject' + str(subject)
                    + '/py_layer' + str(layerIndex) + '_gray_' + str(tri) + 'subject' + str(subject),
                    quality=10, format='eps', bbox_inches='tight')
        fig1.savefig('../../cppData' + fileName[myData] + '/subject' + str(subject)
                     + '/py_layer' + str(layerIndex) + '_x_' + str(tri) + 'subject' + str(subject),
                     quality=10, format='eps', bbox_inches='tight')



