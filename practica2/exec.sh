#!/bin/bash

#C compiler
make
#Execute compiled file
./vad ./audios/output-16k-mono.wav ./exports/output.vad
#Open file
gedit ./exports/output.vad
