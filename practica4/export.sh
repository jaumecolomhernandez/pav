#!/bin/bash

#make -C ./prj all

./bin/release/wav2lpcc 8 12 ./audios/ona.wav ./exp.f
./bin/release/fmatrix_show -H ./exp.f > ./exp.fil
cut -f 2 ./exp.fil > ./exports/col2.pot
cut -f 3 ./exp.fil > ./exports/col3.pot