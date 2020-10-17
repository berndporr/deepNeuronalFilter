#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jul  4 12:43:52 2019

@author: Sama
"""
import numpy as np
import matplotlib.pylab as plt
import pandas as pd

plt.rcParams['font.family'] = 'serif'
plt.rcParams.update({'font.size': 9})

plt.close("all")
            
myData = 2 # 0 for DFN, 1 for LMS, 2 for Laplace
doParamSweep = 1
closePlots = 0
doPlot = 0
DoMovingSweep = 0


if DoMovingSweep == 1:
    sweepFrom = 1
    sweepTo = 50
    big_alpha_data = np.zeros((sweepTo, 15))
    big_delta_data = np.zeros((sweepTo, 15))
else:
    sweepFrom = 0
    sweepTo = 1
    

for sweeper in range(sweepFrom,sweepTo):
    if DoMovingSweep == 1:
        moving_average_alpha = np.ones(15) * sweeper
        moving_average_delta = np.ones(15) * sweeper
    else:
        moving_average_alpha = np.ones(15) *  5 # best window length
        moving_average_delta = np.ones(15) *  5
        
    print("moving average: alpha: ", moving_average_alpha[1], " delta: ", moving_average_delta[1])
    
    fileName = ['', '', '']
    cpp_params = [0, 500, 500, 500]
    
    if doParamSweep == 1:
        #print("doing a parameter sweep")
        iteration = 3 # has to be 3 always
        subject_first = 1
        subject_last = 12
        num_subject = (subject_last - subject_first) + 1
        subject_array = np.arange(0, num_subject, 1)
    else:
        print("saving SNRs")
        iteration = 1
        correction_a = np.loadtxt('../cppData{}/py_alpha_coefficient.txt'.format(fileName[myData]), int)
        correction_d = np.loadtxt('../cppData{}/py_delta_coefficient.txt'.format(fileName[myData]), int)
        goodOnes = np.loadtxt('../cppData{}/py_goodOnes.txt'.format(fileName[myData]), int)
        num_subject = len(goodOnes)
        subject_array = np.arange(0, num_subject, 1)
    
    alpha_matrix_orig = np.zeros((num_subject + 1) * iteration)
    delta_matrix_orig = np.zeros((num_subject + 1) * iteration)
    scores_orig = np.zeros((num_subject + 1) * iteration)
    alpha_matrix = alpha_matrix_orig.reshape((num_subject + 1), iteration)
    delta_matrix = delta_matrix_orig.reshape((num_subject + 1), iteration)
    scores = scores_orig.reshape((num_subject + 1), iteration)
    
    alpha_all_included_orig = np.zeros((num_subject + 1) * iteration)
    delta_all_included_orig = np.zeros((num_subject + 1) * iteration)
    alpha_all_included = alpha_all_included_orig.reshape((num_subject + 1), iteration)
    delta_all_included = delta_all_included_orig.reshape((num_subject + 1), iteration)
    
    for j in range(iteration):
        if doParamSweep == 1:
            correction_a = np.ones(num_subject + 1) * j
            correction_d = np.ones(num_subject + 1) * j
    
        s = 0
        for k in subject_array:
            if closePlots == 1:
                plt.close("all")
    
            if doParamSweep == 1:
                subject = k + subject_first
            else:
                subject = goodOnes[s]
                s += 1
                print(subject, ', ', end='')
    
            filterDelay = cpp_params[myData]
            fs = 250
    
            nn_2d_data = np.loadtxt('../cppData{}/subject{}/nn_subject{}.tsv'.format(fileName[myData], subject, subject),
                                    dtype=float)
            closed_nn_data = nn_2d_data[:, 0]
            opened_nn_data = nn_2d_data[:, 1]
            closed_nn = closed_nn_data[filterDelay::]
            opened_nn = opened_nn_data[filterDelay::]
            dataLength = len(closed_nn)
            timeX = np.linspace(0, dataLength / 250, dataLength)
            freqCutS_D = 1
            freCutE_D = 5
            freqCutS_Start_D = int((freqCutS_D / fs) * dataLength)
            freqCutEnd_D = int((freCutE_D / fs) * dataLength)
            freqCutS_A = 8
            freCutE_A = 12
            freqCutS_Start_A = int((freqCutS_A / fs) * dataLength)
            freqCutEnd_A = int((freCutE_A / fs) * dataLength)
            inner_2d_data = np.loadtxt(
                '../cppData{}/subject{}/inner_subject{}.tsv'.format(fileName[myData], subject, subject), dtype=float)
            inner_closed_data = inner_2d_data[:, 0]
            inner_opened_data = inner_2d_data[:, 1]
            inner_closed = inner_closed_data[filterDelay::]
            inner_opened = inner_opened_data[filterDelay::]
            outer_2d_data = np.loadtxt(
                '../cppData{}/subject{}/outer_subject{}.tsv'.format(fileName[myData], subject, subject), dtype=float)
            outer_closed_data = outer_2d_data[:, 0]
            outer_opened_data = outer_2d_data[:, 1]
            outer_closed = outer_closed_data[filterDelay::]
            outer_opened = outer_opened_data[filterDelay::]
            remover_2d_data = np.loadtxt(
                '../cppData{}/subject{}/remover_subject{}.tsv'.format(fileName[myData], subject, subject), dtype=float)
            remover_closed_data = remover_2d_data[:, 0]
            remover_opened_data = remover_2d_data[:, 1]
            remover_closed = remover_closed_data[filterDelay::]
            remover_opened = remover_opened_data[filterDelay::]
            
            freqX = np.linspace(0, fs / 2, int(dataLength / 2))
            fft_outer_closed = np.abs(np.fft.fft(outer_closed)[0:np.int(dataLength / 2)])
            fft_inner_closed = np.abs(np.fft.fft(inner_closed)[0:np.int(dataLength / 2)])
            fft_closed_nn = np.abs(np.fft.fft(closed_nn)[0:np.int(dataLength / 2)])
            fft_outer_opened = np.abs(np.fft.fft(outer_opened)[0:np.int(dataLength / 2)])
            fft_inner_opened = np.abs(np.fft.fft(inner_opened)[0:np.int(dataLength / 2)])
            fft_opened_nn = np.abs(np.fft.fft(opened_nn)[0:np.int(dataLength / 2)])
            
            # moving average:
            df_inner_opened = pd.DataFrame({"x": freqX, "y": fft_inner_opened})
            df_inner_closed = pd.DataFrame({"x": freqX, "y": fft_inner_closed})
            df_opened_nn = pd.DataFrame({"x": freqX, "y": fft_opened_nn})
            df_closed_nn = pd.DataFrame({"x": freqX, "y": fft_closed_nn})
            fft_inner_opened_smooth_alpha = df_inner_opened.y.rolling(window=int(moving_average_alpha[subject])).mean()
            fft_inner_closed_smooth_alpha = df_inner_closed.y.rolling(window=int(moving_average_alpha[subject])).mean()
            fft_opened_nn_smooth_alpha = df_opened_nn.y.rolling(window=int(moving_average_alpha[subject])).mean()
            fft_closed_nn_smooth_alpha = df_closed_nn.y.rolling(window=int(moving_average_alpha[subject])).mean()
            fft_inner_opened_smooth_delta = df_inner_opened.y.rolling(window=int(moving_average_delta[subject])).mean()
            fft_inner_closed_smooth_delta = df_inner_closed.y.rolling(window=int(moving_average_delta[subject])).mean()
            fft_opened_nn_smooth_delta = df_opened_nn.y.rolling(window=int(moving_average_delta[subject])).mean()
            fft_closed_nn_smooth_delta = df_closed_nn.y.rolling(window=int(moving_average_delta[subject])).mean()
    
            # ALPHA SNR
            aS = 8
            aE = 12
            alphaStart = int((aS / fs) * dataLength)
            alphaEnd = int((aE / fs) * dataLength)
            diff_noise_alpha = alphaEnd - alphaStart
            # RAW
            left_alpha_noise = fft_inner_opened_smooth_alpha[alphaStart]
            right_alpha_noise = fft_inner_opened_smooth_alpha[alphaEnd]
            left_alpha_total = fft_inner_closed_smooth_alpha[alphaStart]
            right_alpha_total = fft_inner_closed_smooth_alpha[alphaEnd]
            correct_alpha_left = left_alpha_noise / left_alpha_total
            correct_alpha_right = right_alpha_noise / right_alpha_total
    
            correct_alpha = 0
            if correction_a[subject] == 0:
                correct_alpha = 1
            else:
                if correction_a[subject] == 1:
                    correct_alpha = correct_alpha_left
                else:
                    if correction_a[subject] == 2:
                        correct_alpha = correct_alpha_right
    
            fft_inner_closed_scaled_alpha = correct_alpha * fft_inner_closed
            fft_inner_closed_smooth_scaled_alpha = correct_alpha * fft_inner_closed_smooth_alpha
    
            noise_alpha = np.sum(fft_inner_opened[alphaStart:alphaEnd]) / diff_noise_alpha
            integral_alpha = np.sum(fft_inner_closed_scaled_alpha[alphaStart:alphaEnd]) / diff_noise_alpha
            signal_alpha = integral_alpha - noise_alpha
            snr_alpha = signal_alpha / noise_alpha
            #
            # NN
            left_alpha_noise_nn = fft_opened_nn_smooth_alpha[alphaStart]
            right_alpha_noise_nn = fft_opened_nn_smooth_alpha[alphaEnd]
            left_alpha_total_nn = fft_closed_nn_smooth_alpha[alphaStart]
            right_alpha_total_nn = fft_closed_nn_smooth_alpha[alphaEnd]
            correct_alpha_left_nn = left_alpha_noise_nn / left_alpha_total_nn
            correct_alpha_right_nn = right_alpha_noise_nn / right_alpha_total_nn
    
            correct_alpha_nn = 0
            if correction_a[subject] == 0:
                correct_alpha_nn = 1
            else:
                if correction_a[subject] == 1:
                    correct_alpha_nn = correct_alpha_left_nn
                else:
                    if correction_a[subject] == 2:
                        correct_alpha_nn = correct_alpha_right_nn
    
            fft_closed_nn_scaled_alpha = correct_alpha_nn * fft_closed_nn
            fft_closed_nn_smooth_scaled_alpha = correct_alpha_nn * fft_closed_nn_smooth_alpha
    
            noise_alpha_nn = np.sum(fft_opened_nn[alphaStart:alphaEnd]) / diff_noise_alpha
            integral_alpha_nn = np.sum(fft_closed_nn_scaled_alpha[alphaStart: alphaEnd]) / diff_noise_alpha
            signal_alpha_nn = integral_alpha_nn - noise_alpha_nn
            snr_alpha_nn = signal_alpha_nn / noise_alpha_nn
            snr_alpha_ratio = snr_alpha_nn / snr_alpha
    
            # delta SNR
            bS = 1
            bE = 5
            deltaStart = int((bS / fs) * dataLength)
            deltaEnd = int((bE / fs) * dataLength)
            diff_noise_delta = deltaEnd - deltaStart
    
            left_delta_signal = fft_inner_closed_smooth_delta[deltaStart]
            right_delta_signal = fft_inner_closed_smooth_delta[deltaEnd]
            left_delta_total = fft_inner_opened_smooth_delta[deltaStart]
            right_delta_total = fft_inner_opened_smooth_delta[deltaEnd]
            correct_delta_left = left_delta_total / left_delta_signal
            correct_delta_right = right_delta_total / right_delta_signal
    
            correct_delta = 0
            if correction_d[subject] == 0:
                correct_delta = 1
            else:
                if correction_d[subject] == 1:
                    correct_delta = correct_delta_left
                else:
                    if correction_d[subject] == 2:
                        correct_delta = correct_delta_right
    
            fft_inner_closed_scaled_delta = correct_delta * fft_inner_closed
            fft_inner_closed_smooth_scaled_delta = correct_delta * fft_inner_closed_smooth_delta
    
            signal_delta = np.sum(fft_inner_closed_scaled_delta[deltaStart:deltaEnd]) / diff_noise_delta
            integral_delta = np.sum(fft_inner_opened[deltaStart:deltaEnd]) / diff_noise_delta
            noise_delta = integral_delta - signal_delta
            snr_delta = signal_delta / noise_delta
            #
    
            left_delta_signal_nn = fft_closed_nn_smooth_delta[deltaStart]
            right_delta_signal_nn = fft_closed_nn_smooth_delta[deltaEnd]
            left_delta_total_nn = fft_opened_nn_smooth_delta[deltaStart]
            right_delta_total_nn = fft_opened_nn_smooth_delta[deltaEnd]
            correct_delta_left_nn = left_delta_total_nn / left_delta_signal_nn
            correct_delta_right_nn = right_delta_total_nn / right_delta_signal_nn
    
            correct_delta_nn = 0
            if correction_d[subject] == 0:
                correct_delta_nn = 1
            else:
                if correction_d[subject] == 1:
                    correct_delta_nn = correct_delta_left_nn
                else:
                    if correction_d[subject] == 2:
                        correct_delta_nn = correct_delta_right_nn
    
            fft_closed_nn_scaled_delta = correct_delta_nn * fft_closed_nn
            fft_closed_nn_smooth_scaled_delta = correct_delta_nn * fft_closed_nn_smooth_delta
    
            signal_delta_nn = np.sum(fft_closed_nn_scaled_delta[deltaStart:deltaEnd]) / diff_noise_delta
            integral_delta_nn = np.sum(fft_opened_nn[deltaStart:deltaEnd]) / diff_noise_delta
            noise_delta_nn = integral_delta_nn - signal_delta_nn
            snr_delta_nn = signal_delta_nn / noise_delta_nn
    
            snr_delta_ratio = snr_delta_nn / snr_delta
    
            if (snr_alpha_ratio > 1) & (snr_alpha > 0):
                alpha_matrix[k + 1, j] = snr_alpha_ratio
                scores[k + 1, j] = 1
            if (snr_delta_ratio > 1) & (snr_delta > 0):
                delta_matrix[k + 1, j] = snr_delta_ratio
                scores[k + 1, j] = -1
            if (snr_alpha_ratio > 1) & (snr_delta_ratio > 1) & (snr_alpha > 0) & (snr_delta > 0):
                scores[k + 1, j] = 2
            
            alpha_all_included[k + 1, j] = snr_alpha_ratio
            delta_all_included[k + 1, j] = snr_delta_ratio
    
    
            if doParamSweep == 0:
                SNRarray = np.array([snr_alpha, snr_alpha_nn, snr_alpha_ratio, snr_delta, snr_delta_nn, snr_delta_ratio])
                np.savetxt('../cppData' + fileName[myData] + '/subject' + str(subject) + '/py_subject_'
                           + str(subject) + '_SNRs.txt', SNRarray, fmt='%1.4f')
    
            vl = 10
            height = 0.05
            vertical = np.linspace(0, height, vl)
            yLimit = height
    
            fStart = int((0 / fs) * dataLength)
            fEnd = int((25 / fs) * dataLength)
            
            row = 3
            col = 2
    
            if doPlot == 1:
                myFig1 = plt.figure(subject)
                plt.subplot(row,col,1)
                plt.plot(outer_opened, linewidth=0.4, color='black')
                plt.title('outer_opened')
                plt.subplot(row,col,1)
                plt.plot(inner_opened, linewidth=0.4, color='blue')
                plt.title('inner_opened')
                plt.subplot(row,col,1)
                plt.plot(remover_opened, linewidth=0.4, color='green')
                plt.title('remover_opened')
                plt.subplot(row,col,3)
                plt.plot(opened_nn, linewidth=0.4, color='gray')
                plt.title('opened_nn')
                
                plt.subplot(row,col,2)
                plt.plot(outer_closed, linewidth=0.4, color='black')
                plt.title('outer_closed')
                plt.subplot(row,col,2)
                plt.plot(inner_closed, linewidth=0.4, color='blue')
                plt.title('inner_closed')
                plt.subplot(row,col,2)
                plt.plot(remover_closed, linewidth=0.4, color='green')
                plt.title('remover_closed')
                plt.subplot(row,col,4)
                plt.plot(closed_nn, linewidth=0.4, color='gray')
                plt.title('closed_nn')
    
                plt.subplot(row,col,5)
                plt.plot(freqX[freqCutS_Start_D:freqCutEnd_D], fft_inner_opened[freqCutS_Start_D:freqCutEnd_D],
                         linewidth=0.4, color='gray')
                plt.plot(freqX[freqCutS_Start_D:freqCutEnd_D]+6, fft_inner_closed_scaled_delta[freqCutS_Start_D:freqCutEnd_D],
                         linewidth=0.4, color='gray')
    #            plt.plot(freqX[freqCutS_Start_D:freqCutEnd_D], fft_inner_opened_smooth_delta[freqCutS_Start_D:freqCutEnd_D],
    #                     linewidth=0.4, color='blue')
    #            plt.plot(freqX[freqCutS_Start_A:freqCutEnd_A], fft_inner_opened_smooth_alpha[freqCutS_Start_A:freqCutEnd_A],
    #                     linewidth=0.4, color='blue')
                
                plt.plot(freqX[freqCutS_Start_D:freqCutEnd_D], fft_opened_nn[freqCutS_Start_D:freqCutEnd_D],
                         linewidth=0.4, color='green')
                plt.plot(freqX[freqCutS_Start_D:freqCutEnd_D]+6, fft_closed_nn_scaled_delta[freqCutS_Start_D:freqCutEnd_D],
                         linewidth=0.4, color='green')
    #            plt.plot(freqX[freqCutS_Start_D:freqCutEnd_D], fft_opened_nn_smooth_delta[freqCutS_Start_D:freqCutEnd_D],
    #                     linewidth=0.4, color='green')
    #            plt.plot(freqX[freqCutS_Start_A:freqCutEnd_A], fft_opened_nn_smooth_alpha[freqCutS_Start_A:freqCutEnd_A],
    #                     linewidth=0.4, color='green')
                plt.xticks([1, 5, 8, 12])
                plt.subplot(row,col,6)
                plt.plot(freqX[freqCutS_Start_A:freqCutEnd_A]-6, fft_inner_opened[freqCutS_Start_A:freqCutEnd_A],
                         linewidth=0.4, color='gray')
                plt.plot(freqX[freqCutS_Start_A:freqCutEnd_A], fft_inner_closed_scaled_alpha[freqCutS_Start_A:freqCutEnd_A],
                         linewidth=0.4, color='gray')
    #            plt.plot(freqX[freqCutS_Start_D:freqCutEnd_D], fft_inner_closed_smooth_scaled_delta[freqCutS_Start_D:freqCutEnd_D],
    #                     linewidth=0.4, color='orange')
    #            plt.plot(freqX[freqCutS_Start_A:freqCutEnd_A], fft_inner_closed_smooth_scaled_alpha[freqCutS_Start_A:freqCutEnd_A],
    #                     linewidth=0.4, color='orange')
                plt.plot(freqX[freqCutS_Start_A:freqCutEnd_A]-6, fft_opened_nn[freqCutS_Start_A:freqCutEnd_A],
                         linewidth=0.4, color='green')
                plt.plot(freqX[freqCutS_Start_A:freqCutEnd_A], fft_closed_nn_scaled_alpha[freqCutS_Start_A:freqCutEnd_A],
                         linewidth=0.4, color='green')
    #            plt.plot(freqX[freqCutS_Start_D:freqCutEnd_D], fft_closed_nn_smooth_scaled_delta[freqCutS_Start_D:freqCutEnd_D],
    #                     linewidth=0.4, color='red')
    #            plt.plot(freqX[freqCutS_Start_A:freqCutEnd_A], fft_closed_nn_smooth_scaled_alpha[freqCutS_Start_A:freqCutEnd_A],
    #                     linewidth=0.4, color='red')
                plt.xticks([1, 5, 8, 12])
    
                myFig1.savefig('../cppData' + fileName[myData] + '/subject' + str(subject) + '/py_subject_'
                               + str(subject) + '_Figs', quality=10, format='eps', bbox_inches='tight')
                plt.show()
    
    if doParamSweep == 1:
        alpha_max = alpha_matrix.max(axis=1)
        alpha_max_coefficient = alpha_matrix.argmax(axis=1)
        alpha_max_coefficient[np.where(alpha_max == 0)] = 3
        delta_max = delta_matrix.max(axis=1)
        delta_max_coefficient = delta_matrix.argmax(axis=1)
        delta_max_coefficient[np.where(delta_max == 0)] = 3
        goodOnes = np.where((delta_max_coefficient < 3) & (alpha_max_coefficient < 3))[0]
        np.savetxt('../cppData' + fileName[myData] + '/py_goodOnes.txt', goodOnes, fmt='%d')
        np.savetxt('../cppData' + fileName[myData] + '/py_alpha_coefficient.txt', alpha_max_coefficient, fmt='%d')
        np.savetxt('../cppData' + fileName[myData] + '/py_delta_coefficient.txt', delta_max_coefficient, fmt='%d')
    
    if doParamSweep == 0:
        alpha_max = alpha_matrix
        alpha_max_coefficient = correction_a[np.where((correction_a < 3) & (correction_d < 3))]
        delta_max = delta_matrix
        delta_max_coefficient = correction_d[np.where((correction_a < 3) & (correction_d < 3))]
    
    if DoMovingSweep == 1:
        big_alpha_data[sweeper,:] = alpha_max
        big_delta_data[sweeper,:] = delta_max
        
    print()
    print("----> ", len(np.where(alpha_max > 0)[0]), " alpha with sum of: ", np.sum(alpha_max))
    print("----> ", len(np.where(delta_max > 0)[0]), " delta with sum of: ", np.sum(delta_max))
    print("----> ", len(goodOnes), ' good ones are: ', goodOnes)
    print('ALPHA SNR: ')
    print(alpha_max)
    print(alpha_max_coefficient)
    print('DELTA SNR: ')
    print(delta_max)
    print(delta_max_coefficient)


















