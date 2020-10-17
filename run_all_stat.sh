#!/bin/bash

mkdir -p cppData
n=1;
max=12;
while [ "$n" -le "$max" ]; do
  mkdir -p "cppData/subject$n"
  n=`expr "$n" + 1`;
done

echo "Successfully created the cpp folders"

cmake .
make
./run_eeg_filter

