#!/bin/bash

#C compiler
make
#Execute compiled file
python run_vad.py 
#Evaluation
./vad_evaluation.pl ./audios/*/*.lab
