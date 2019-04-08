#!/bin/bash

#Compilem
make -C ./prj all

#Export de PATH
which gmm_train > /dev/null
if [[ $? != 0 ]] ; then
   export PATH=$PATH:/home/jc/pav/practica4/bin/release/
fi

#Crear llistes
./bin/release/run_spkid ver
#Extracció de features
./bin/release/run_spkid mcp
#Entreno (trobar els coeficients(mitjana i variança))
./bin/release/run_spkid trainmcp
#Llancem el validador
./bin/release/run_spkid testmcp
#Contem els errors
./bin/release/run_spkid classerr