#!/bin/bash

#C compiler
gcc -o mai main.c pav_analysis.c -lm
#Execute compiled file
./mai ./audios/output-16k-mono.wav ./exports/exports
#Transform exports file
cut -f 2 ./exports/exports > ./exports/power.txt
cut -f 3 ./exports/exports > ./exports/amplitude.txt
cut -f 4 ./exports/exports > ./exports/zero_cross_rate.txt
#Create plots with python script
python plots.py

