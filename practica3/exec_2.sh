#!/bin/bash

#C compiler
make -C ./prj all
#PRE-PROCESSING 2KH SAMPLING
./bin/release/get_pitch ./audios/sb043.wav ./exports/sb043.f0
#Evaluation
python plot_results.py
