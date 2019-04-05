#!/bin/bash

#Compute model
make -C ./prj all
#Calculate model
./bin/debug/gmm_train -d "./exports/ona" -e "lp" -F  LP_ona 
#Evaluation
