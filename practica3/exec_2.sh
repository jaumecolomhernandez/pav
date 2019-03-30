#!/bin/bash

#C compiler
make -C ./prj all
#Get Pitch
./bin/release/get_pitch ./audios/rl005.wav ./exports/rl005.f0
#Eval
./bin/release/pitch_evaluate ./exports/rl005.f0ref
#Evaluation
python plot_results.py
