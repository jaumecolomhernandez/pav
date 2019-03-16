#!/bin/python

#SCRIPT TO RUN VAD ON ALL .WAV FILES 

import os
import sys
import glob

for root, dirs, files in os.walk("./audios"):
    for file in files:
        if file.endswith(".wav"):
            #WE HAVE TO CHECK THAT WE ARE NOT RUNNING THE VAD IN THE COPY FILE
            #OTHERWISE WE WILL WE DOUBLING THE FILES EVERYTIME
            if 'copia' in file:
                break
            str_file = os.path.join(root, file)
            os.system(f'./vad {str_file} {str_file[:-3]+"vad"} {str_file[:-4]+"_copia.wav"}')
