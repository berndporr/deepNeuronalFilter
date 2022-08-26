# -*- coding: utf-8 -*-
import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as signal
import math as math
import scipy.stats as stats
from numpy.polynomial import Polynomial

class ParalysedEEGFromWhithamEtAl:
    """
    (C) 2022 Bernd Porr <bernd.porr@glasgow.ac.uk>

    GNU GENERAL PUBLIC LICENSE
    Version 3, 29 June 2007

    Log Power spectra of pure EEG from two temporarily paralysed
    subjects. Data adapted from the figures in:

    Scalp electrical recording during paralysis: Quantitative evidence that
    EEG frequencies above 20 Hz are contaminated by EMG
    Emma M. Whitham a , Kenneth J. Pope b , Sean P. Fitzgibbon c , Trent Lewis b ,
    C. Richard Clark c , Stephen Loveless d , Marita Broberg e , Angus Wallace e ,
    Dylan DeLosAngeles e , Peter Lillie f , Andrew Hardy f , Rik

    by having manually clicked on the datapoints of the 6 PSD plots
    in the paper and then done a polynomial fit to smooth out the errors
    from the manual clicking.
    """

    f_signal_min = 1
    f_signal_max = 95

    def __init__(self,datasetIndex = -1,degree = 15):
        """
        Provides the power spectrum from Figure `datasetIndex` from Whitham et al
        using a polynomial fit. The default degreee is 15. If no datasetIndex is
        given or is negative then average from all 6 figures from the paper is calculated.
        """
        if datasetIndex in range(len(self.allsubjectdata)):
            a = self.allsubjectdata[datasetIndex]
            f = a[:,0]
            p = a[:,1]
            self.psd_coeff = Polynomial.fit(f, p, degree, domain = [self.f_signal_min,self.f_signal_max])
        else:
            a = self.allsubjectdata[0]
            f = a[:,0]
            p = a[:,1]
            tmpcoeff = Polynomial.fit(f, p, degree, domain = [self.f_signal_min,self.f_signal_max])
            for i in range(len(self.allsubjectdata)-1):
                a = self.allsubjectdata[i+1]
                f = a[:,0]
                p = a[:,1]
                c = Polynomial.fit(f, p, degree, domain = [self.f_signal_min,self.f_signal_max])
                tmpcoeff = tmpcoeff + c
            self.psd_coeff = tmpcoeff / len(self.allsubjectdata)

    def EEGVariance(self,frequency):
        """
        Returns the EEG variance (i.e. power spectral density) at the specified frequency
        """
        return self.psd_coeff(frequency)

    def totalEEGPower(self):
        """
        Calculates the total power of the EEG.
        """
        totalpower = 0
        for f in np.arange(self.f_signal_min,self.f_signal_max,1.0):
            psd = self.EEGVariance(f)
            totalpower = totalpower + 10**psd
        return totalpower

    sub1a = np.array([[  0.688863, -10.456   ],
       [  1.836969, -10.808   ],
       [  2.06659 , -10.976   ],
       [  2.296211, -11.144   ],
       [  2.640643, -11.336   ],
       [  3.329506, -11.568   ],
       [  4.592423, -11.728   ],
       [  5.281286, -11.808   ],
       [  5.855339, -11.72    ],
       [  6.659013, -11.56    ],
       [  7.692308, -11.4     ],
       [  8.266361, -11.176   ],
       [  8.381171, -11.      ],
       [  8.725603, -10.728   ],
       [  9.414466, -10.632   ],
       [  9.873708, -10.448   ],
       [ 11.366246, -10.84    ],
       [ 11.940299, -11.416   ],
       [ 12.055109, -11.224   ],
       [ 13.892078, -11.504   ],
       [ 15.154994, -11.496   ],
       [ 16.877153, -11.544   ],
       [ 17.795637, -11.68    ],
       [ 18.599311, -11.776   ],
       [ 20.665901, -11.944   ],
       [ 21.010333, -12.008   ],
       [ 23.536165, -12.16    ],
       [ 24.913892, -12.288   ],
       [ 26.865672, -12.376   ],
       [ 30.884041, -12.8     ],
       [ 33.754305, -13.008   ],
       [ 37.083812, -13.2     ],
       [ 40.528129, -13.32    ],
       [ 42.250287, -13.432   ],
       [ 45.809414, -13.624   ],
       [ 53.04248 , -13.8     ],
       [ 55.338691, -13.848   ],
       [ 58.553387, -13.92    ],
       [ 60.505166, -13.952   ],
       [ 63.26062 , -14.04    ],
       [ 65.556831, -14.048   ],
       [ 68.197474, -14.064   ],
       [ 70.264064, -14.104   ],
       [ 72.330654, -14.136   ],
       [ 74.626866, -14.192   ],
       [ 77.152698, -14.2     ],
       [ 78.415614, -14.216   ],
       [ 80.252583, -14.224   ],
       [ 81.974742, -14.224   ],
       [ 84.156142, -14.272   ],
       [ 85.993111, -14.32    ],
       [ 88.059701, -14.344   ],
       [ 94.144661, -14.376   ],
       [ 96.096441, -14.44    ],
       [ 97.588978, -14.48    ]])


    sub1b = np.array([[  0.      , -10.472   ],
       [  1.033295, -10.552   ],
       [  1.148106, -10.648   ],
       [  1.607348, -10.76    ],
       [  1.95178 , -10.88    ],
       [  2.181401, -11.064   ],
       [  2.411022, -11.328   ],
       [  2.755454, -11.392   ],
       [  2.870264, -11.472   ],
       [  3.329506, -11.536   ],
       [  4.13318 , -11.592   ],
       [  5.396096, -11.72    ],
       [  6.773823, -11.656   ],
       [  7.003444, -11.584   ],
       [  7.577497, -11.528   ],
       [  7.692308, -11.464   ],
       [  7.921929, -11.4     ],
       [  8.15155 , -11.272   ],
       [  8.495982, -11.24    ],
       [  8.840413, -11.136   ],
       [  9.184845, -11.048   ],
       [  9.873708, -10.968   ],
       [ 10.332951, -11.024   ],
       [ 10.447761, -11.08    ],
       [ 11.021814, -11.232   ],
       [ 11.366246, -11.416   ],
       [ 11.595867, -11.36    ],
       [ 12.28473 , -11.696   ],
       [ 12.514351, -11.776   ],
       [ 14.006889, -11.728   ],
       [ 14.810563, -11.672   ],
       [ 15.729047, -11.672   ],
       [ 15.843858, -11.696   ],
       [ 18.36969 , -11.776   ],
       [ 20.895522, -11.896   ],
       [ 22.273249, -12.144   ],
       [ 22.50287 , -12.248   ],
       [ 23.650976, -12.4     ],
       [ 25.258324, -12.48    ],
       [ 25.832377, -12.552   ],
       [ 27.324914, -12.664   ],
       [ 28.128588, -12.76    ],
       [ 30.884041, -12.928   ],
       [ 32.261768, -13.032   ],
       [ 34.098737, -13.184   ],
       [ 36.280138, -13.328   ],
       [ 39.265212, -13.424   ],
       [ 39.724455, -13.504   ],
       [ 43.283582, -13.608   ],
       [ 46.039036, -13.648   ],
       [ 53.731343, -13.832   ],
       [ 60.734788, -14.      ],
       [ 64.408726, -14.088   ],
       [ 70.034443, -14.136   ],
       [ 74.856487, -14.232   ],
       [ 80.597015, -14.336   ],
       [ 85.64868 , -14.352   ],
       [ 90.241102, -14.4     ],
       [ 93.455798, -14.464   ],
       [ 96.326062, -14.512   ],
       [ 98.163031, -14.504   ]])


    sub1c = np.array([[  0.344432, -10.592   ],
       [  0.688863, -10.632   ],
       [  1.377727, -10.76    ],
       [  1.836969, -10.872   ],
       [  2.06659 , -11.216   ],
       [  2.870264, -11.344   ],
       [  3.099885, -11.424   ],
       [  3.673938, -11.608   ],
       [  5.051665, -11.896   ],
       [  6.429392, -12.024   ],
       [  8.15155 , -11.904   ],
       [  8.381171, -11.856   ],
       [  8.840413, -11.832   ],
       [  9.873708, -11.792   ],
       [ 10.677382, -11.792   ],
       [ 12.28473 , -11.824   ],
       [ 13.203215, -11.856   ],
       [ 14.810563, -11.944   ],
       [ 18.36969 , -11.888   ],
       [ 19.862227, -12.024   ],
       [ 21.928817, -12.16    ],
       [ 22.50287 , -12.232   ],
       [ 24.56946 , -12.432   ],
       [ 26.406429, -12.624   ],
       [ 27.784156, -12.76    ],
       [ 29.391504, -12.96    ],
       [ 31.228473, -13.168   ],
       [ 32.72101 , -13.392   ],
       [ 37.543054, -13.52    ],
       [ 40.987371, -13.608   ],
       [ 44.316877, -13.688   ],
       [ 53.846154, -13.904   ],
       [ 56.601607, -14.024   ],
       [ 59.471871, -14.064   ],
       [ 62.571757, -14.104   ],
       [ 64.982778, -14.2     ],
       [ 68.771527, -14.208   ],
       [ 70.838117, -14.232   ],
       [ 74.167623, -14.272   ],
       [ 76.808266, -14.312   ],
       [ 78.874856, -14.328   ],
       [ 80.941447, -14.36    ],
       [ 82.433984, -14.368   ],
       [ 87.83008 , -14.464   ],
       [ 90.470723, -14.512   ],
       [ 93.226177, -14.528   ],
       [ 96.326062, -14.544   ],
       [ 97.703789, -14.536   ]])


    sub2a = np.array([[  0.571429, -10.2     ],
       [  1.028571, -10.352   ],
       [  1.257143, -10.736   ],
       [  1.714286, -10.872   ],
       [  2.057143, -10.504   ],
       [  2.285714, -10.984   ],
       [  2.857143, -11.232   ],
       [  3.771429, -11.528   ],
       [  4.571429, -11.704   ],
       [  5.714286, -12.072   ],
       [  7.885714, -12.032   ],
       [  8.571429, -12.056   ],
       [  9.257143, -12.016   ],
       [  9.714286, -11.896   ],
       [ 10.285714, -11.704   ],
       [ 10.742857, -11.52    ],
       [ 11.771429, -11.344   ],
       [ 12.571429, -11.264   ],
       [ 13.257143, -11.336   ],
       [ 13.714286, -11.712   ],
       [ 13.942857, -11.832   ],
       [ 14.285714, -11.952   ],
       [ 15.771429, -12.264   ],
       [ 16.228571, -12.36    ],
       [ 16.342857, -12.376   ],
       [ 17.371429, -12.368   ],
       [ 19.428571, -12.312   ],
       [ 20.114286, -12.288   ],
       [ 21.6     , -12.272   ],
       [ 22.628571, -12.272   ],
       [ 24.457143, -12.16    ],
       [ 27.085714, -12.16    ],
       [ 26.628571, -12.352   ],
       [ 27.885714, -12.72    ],
       [ 28.685714, -12.848   ],
       [ 29.828571, -13.024   ],
       [ 32.114286, -13.104   ],
       [ 34.057143, -13.184   ],
       [ 35.2     , -13.256   ],
       [ 37.942857, -13.296   ],
       [ 39.428571, -13.32    ],
       [ 42.514286, -13.344   ],
       [ 46.285714, -13.424   ],
       [ 49.6     , -13.408   ],
       [ 51.657143, -13.448   ],
       [ 55.085714, -13.52    ],
       [ 57.828571, -13.632   ],
       [ 62.171429, -13.712   ],
       [ 65.028571, -13.792   ],
       [ 66.285714, -13.864   ],
       [ 68.914286, -14.      ],
       [ 73.6     , -14.176   ],
       [ 78.971429, -14.184   ],
       [ 82.171429, -14.256   ],
       [ 85.371429, -14.328   ],
       [ 90.057143, -14.408   ],
       [ 92.114286, -14.424   ],
       [ 94.171429, -14.432   ],
       [ 96.8     , -14.44    ],
       [ 97.6     , -14.448   ],
       [ 97.828571, -14.456   ]])


    sub2b = np.array([[  0.571429, -10.288   ],
       [  1.142857, -10.336   ],
       [  1.828571, -10.472   ],
       [  2.057143, -10.952   ],
       [  2.171429, -10.912   ],
       [  2.514286, -11.216   ],
       [  2.628571, -11.416   ],
       [  3.885714, -11.688   ],
       [  4.228571, -11.92    ],
       [  5.028571, -12.008   ],
       [  5.371429, -12.032   ],
       [  6.285714, -12.048   ],
       [  6.628571, -12.056   ],
       [  7.2     , -12.056   ],
       [  8.      , -12.056   ],
       [  8.8     , -12.072   ],
       [  9.257143, -12.032   ],
       [ 10.285714, -11.92    ],
       [ 11.314286, -11.736   ],
       [ 12.114286, -11.64    ],
       [ 12.228571, -11.536   ],
       [ 13.028571, -11.56    ],
       [ 13.714286, -11.664   ],
       [ 13.942857, -11.816   ],
       [ 14.742857, -12.048   ],
       [ 15.314286, -12.296   ],
       [ 16.114286, -12.568   ],
       [ 16.8     , -12.656   ],
       [ 19.428571, -12.584   ],
       [ 19.542857, -12.568   ],
       [ 20.685714, -12.488   ],
       [ 22.285714, -12.416   ],
       [ 22.628571, -12.36    ],
       [ 25.485714, -12.432   ],
       [ 26.628571, -12.6     ],
       [ 27.771429, -12.704   ],
       [ 29.257143, -12.824   ],
       [ 30.971429, -13.      ],
       [ 34.514286, -13.2     ],
       [ 38.742857, -13.352   ],
       [ 43.885714, -13.4     ],
       [ 45.371429, -13.408   ],
       [ 50.4     , -13.488   ],
       [ 57.028571, -13.528   ],
       [ 60.228571, -13.608   ],
       [ 63.2     , -13.736   ],
       [ 70.171429, -14.      ],
       [ 73.142857, -14.16    ],
       [ 76.342857, -14.192   ],
       [ 81.942857, -14.296   ],
       [ 83.885714, -14.304   ],
       [ 86.285714, -14.312   ],
       [ 90.857143, -14.376   ],
       [ 94.514286, -14.424   ],
       [ 98.057143, -14.456   ]])


    sub2c = np.array([[  0.457143, -10.84    ],
       [  1.371429, -10.976   ],
       [  2.971429, -11.24    ],
       [  2.514286, -11.368   ],
       [  3.885714, -11.808   ],
       [  5.257143, -11.952   ],
       [  6.285714, -12.088   ],
       [  7.657143, -12.112   ],
       [  8.914286, -12.104   ],
       [  9.6     , -12.04    ],
       [ 11.085714, -11.912   ],
       [ 12.      , -11.76    ],
       [ 12.914286, -11.76    ],
       [ 13.714286, -11.784   ],
       [ 14.628571, -11.944   ],
       [ 15.428571, -12.16    ],
       [ 15.657143, -12.328   ],
       [ 16.8     , -12.552   ],
       [ 16.914286, -12.64    ],
       [ 17.142857, -12.704   ],
       [ 17.828571, -12.744   ],
       [ 18.171429, -12.808   ],
       [ 19.2     , -12.824   ],
       [ 20.914286, -12.76    ],
       [ 22.514286, -12.576   ],
       [ 23.314286, -12.568   ],
       [ 24.342857, -12.544   ],
       [ 24.685714, -12.528   ],
       [ 27.2     , -12.664   ],
       [ 27.771429, -12.776   ],
       [ 29.714286, -12.912   ],
       [ 33.028571, -13.168   ],
       [ 34.057143, -13.312   ],
       [ 41.828571, -13.376   ],
       [ 48.      , -13.336   ],
       [ 54.057143, -13.504   ],
       [ 56.228571, -13.568   ],
       [ 58.857143, -13.624   ],
       [ 60.      , -13.672   ],
       [ 64.685714, -13.84    ],
       [ 66.514286, -13.872   ],
       [ 69.142857, -13.968   ],
       [ 73.6     , -14.096   ],
       [ 76.228571, -14.2     ],
       [ 81.257143, -14.256   ],
       [ 83.314286, -14.296   ],
       [ 87.657143, -14.328   ],
       [ 90.514286, -14.368   ],
       [ 95.085714, -14.424   ],
       [ 97.6     , -14.44    ]])

    allsubjectdata = [sub1a, sub1b, sub1c, sub2a, sub2b, sub2c]
    allsubjectdesciption = ['subject1 1st PSD', 'subject1 2nd PSD', 'subject1 3rd PSD',
                            'subject2 1st PSD', 'subject2 2nd PSD', 'subject2 3rd PSD']
