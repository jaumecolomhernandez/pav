#!/bin/bash

#C compiler
make
#Execute compiled file
./vad ./audios/2017-18q2/pav_231.wav ./audios/2017-18q2/pav_231.vad ./copy.wav
#Evaluation
#./vad_evaluation.pl ./audios/*/*.lab 
