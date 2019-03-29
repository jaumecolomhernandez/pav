#!/bin/bash

#C compiler
make -C ./prj all
#Execute compiled file
./bin/release/pitch_evaluate  ./audios/rl009.wav ./exports/rl009.f0

