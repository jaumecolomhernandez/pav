#!/usr/bin/env python3

#SCRIPT TO REPLACE STRING IN ALL THE FILES
#To correct the .lab in the database

import fileinput
import os
import sys
import glob

for root, dirs, files in os.walk("./audios"):
    for file in files:
        if file.endswith(".lab"):
            str_file = os.path.join(root, file)
            with open(str_file) as f:
                newText=f.read().replace('s', 'S')
            
            with open(str_file, "w") as f:
                f.write(newText)
                
        if file.endswith(".lab"):
            with open(str_file) as f:
                newText=f.read().replace('v', 'V')
            
            with open(str_file, "w") as f:
                f.write(newText)