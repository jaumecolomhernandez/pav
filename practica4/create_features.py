#!/bin/python

#SCRIPT TO COMPUTE FEATURES ON ALL .WAV FILES 

import os
import sys
import glob

for root, dirs, files in os.walk("./audios"):
    for file in files:
        if file.endswith(".wav"):
            #Creem strings per a exportar
            str_file = os.path.join(root, file)
            base_export = os.path.join('./exports', file[:-4])
            str_export = os.path.join(base_export, file[:-4]+'.lp')
            str_export_txt = os.path.join(base_export, file[:-4]+'.txt')

            #Creem directori si no existeix
            if not os.path.exists(base_export):
                os.makedirs(base_export)
            
            #print(str_export_txt)
            #Calculem features amb el wav21p
            os.system(f'./bin/release/wav2lp 8 {str_file} {str_export}')
            #Passem a fitxer de text
            os.system(f'./bin/release/fmatrix_show -H {str_export} > {str_export_txt}')
            #Exportem a columnes
            for i in range(1,10):
                str_column = os.path.join(base_export,f'col{i}.col')
                os.system(f'cut -f {i} {str_export_txt} > {str_column}')