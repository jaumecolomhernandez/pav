#!/bin/bash

#C compiler
make -C ./prj all
#PRE-PROCESSING 2KH SAMPLING
#python run_sox.py
#Execute compiled file
python run_pitch.py 
#Evaluation
./bin/release/pitch_evaluate ./exports/*.f0ref
