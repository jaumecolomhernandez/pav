#!/bin/python

#SCRIPT TO RUN THE PITCHDETECTOR ON ALL FILES 

import os
import sys
import glob

for root, dirs, files in os.walk("./audios"):
    for file in files:
        if file.endswith(".wav"):
            str_file = os.path.join(root, file)
            os.system(f'./bin/release/get_pitch {str_file} ./exports/{file[:-3]}f0')
