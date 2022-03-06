#!/bin/bash

# Jaw clench experiment data:
# Filters all subjects without graphical output

rm -rf jawclench
mkdir -p jawclench
n=1;
max=20;
while [ "$n" -le "$max" ]; do
  rm -rf "jawclench/subject$n"
  mkdir -p "jawclench/subject$n"
  n=`expr "$n" + 1`;
done

echo "Successfully created the results folders"

./eeg_filter -b jawclench
