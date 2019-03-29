#!/bin/bash

#C compiler
make -C ./prj all
#Execute compiled file
./bin/release/get_pitch  ./audios/rl009.wav ./exports/rl009.f0 > ./exports_file
#Exporta a fichero
cut -f 1 ./exports_file > ./exports/power.txt    
cut -f 2 ./exports_file > ./exports/n1_n0.txt
cut -f 3 ./exports_file > ./exports/nmax_n0.txt

