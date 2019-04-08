#!/bin/bash

#Compilem
make -C ./prj all

#Export de PATH
which gmm_train > /dev/null
if [[ $? != 0 ]] ; then
   export PATH=$PATH:/home/jc/pav/practica4/bin/release/
fi

#Crear llistes
#./bin/release/run_spkid lists
#Extracció de features
#./bin/release/run_spkid mcp
#Entreno (trobar els coeficients(mitjana i variança))
./bin/release/run_spkid verify
#Llancem el verificador
./bin/release/run_spkid verif_err
#Contem els errors
./bin/release/run_spkid roc