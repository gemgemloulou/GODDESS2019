#!/bin/bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib
echo "Run this script by doing './sorter.sh x y', where x and y are your first and last run. No preceding zeroes needed."
if [ -z "$1 $2" ];
then 
  echo "*** Please put the run number range after the script"
  exit 1
fi

for(( i=$1; i<=$2; i++ ))
do
./SortData $i
done
