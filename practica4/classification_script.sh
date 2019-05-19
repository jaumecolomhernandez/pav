#!/bin/bash

#CLASSIFICATION SCRIPT 
#Aquest script executa tot el procés de classificació

#path_jaume = /home/jc/pav/practica4/bin/release
#path_uni = /home/pav113/pav/practica4/bin/release/
#path_miquel = /

#Compilem
make -C ./prj all

#Export de PATH
which gmm_train > /dev/null
if [[ $? != 0 ]] ; then
   export PATH=$PATH:/home/pav113/pav/practica4/bin/release
fi

#1- Crear llistes
./bin/release/run_spkid lists
#2- Extracció de features
./bin/release/run_spkid extract 
#3- Entreno (trobar els coeficients(mitjana i variança))
./bin/release/run_spkid trainmcp
#4- Llancem el validador
./bin/release/run_spkid testmcp
#5- Contem els errors
./bin/release/run_spkid classerr