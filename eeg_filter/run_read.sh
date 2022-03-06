#!/bin/bash

# Jaw clench experiment data:
# Filters all subjects without graphical output

rm -rf read
mkdir -p read
n=1;
max=20;
while [ "$n" -le "$max" ]; do
  rm -rf "read/subject$n"
  mkdir -p "read/subject$n"
  n=`expr "$n" + 1`;
done

echo "Successfully created the results folders"

./eeg_filter -b read
