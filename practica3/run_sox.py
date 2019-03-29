#!/bin/python

#SCRIPT TO RUN SOX TO CHANGE SAMPLE RATE ON ALL FILES 

import os
import sys
import glob

for root, dirs, files in os.walk("./audios_no"):
    for file in files:
        if file.endswith(".wav"):
            str_file = os.path.join(root, file)
            os.system(f'sox {str_file} -r 2000 ./audios/{file}')
