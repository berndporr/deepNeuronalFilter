#!/bin/bash

# P300 data:
# Filters all subjects without graphical output

rm -rf p300
mkdir -p p300
n=1;
max=20;
while [ "$n" -le "$max" ]; do
  rm -rf "p300/subject$n"
  mkdir -p "p300/subject$n"
  n=`expr "$n" + 1`;
done

echo "Successfully created the p300 folders"

./eeg_filter -b
