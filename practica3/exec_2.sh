#!/bin/bash

#C compiler
make -C ./prj all
#Get Pitch
./bin/debug/get_pitch ./audios/rl019.wav ./exports/rl019.f0
#Eval
./bin/release/pitch_evaluate ./exports/rl019.f0ref
#Evaluation
#python plot_results.py
