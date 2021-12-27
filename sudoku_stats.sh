#!/bin/bash

mkdir -p sudoku
n=1;
max=20;
while [ "$n" -le "$max" ]; do
  rm -rf "sudoku/subject$n"
  mkdir -p "sudoku/subject$n"
  n=`expr "$n" + 1`;
done

echo "Successfully created the results folders"

cmake .
make
./eeg_filter -a sudoku
