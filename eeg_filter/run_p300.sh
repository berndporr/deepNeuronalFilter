#!/bin/bash

# P300 data:
# Filters all subjects without graphical output

rm -rf results
mkdir -p results
n=1;
max=20;
while [ "$n" -le "$max" ]; do
  rm -rf "results/subject$n"
  mkdir -p "results/subject$n"
  n=`expr "$n" + 1`;
done

echo "Successfully created the results folders"

./eeg_filter -b
