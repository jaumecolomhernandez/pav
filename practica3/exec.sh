#!/bin/bash

#C compiler
make -C ./prj all
#Execute compiled file
python run_pitch.py 
#Evaluation
./bin/release/pitch_evaluate ./exports/*.f0ref
