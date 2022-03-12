# EEG filter

![alt tag](screenshot.png)

## How to run

### Single subject and task
```
/eeg_filter <subject number> [task]
```
where the subject number is compulsory and the task
(p300, jawclench, ...) is optional. Default is p300.

### All subjects and both p300 & jawclench

Just type:
```
./runall.sh
```
and the data from all 20 subjects is filtered for both
the p300 and the jawclench task.

## Analysis

For plotting the P300 peaks, calculating the SNR, the weight
developments go to the subdirectory `analysis`.