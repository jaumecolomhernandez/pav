#include <stdio.h>
#include <math.h>
#include "pav_analysis.h"
#define NSAMPLES 160 /* 10 ms, fs=16kHz */

int main(int argc, const char *argv[]){
    //Comprovem que tinguem 2 o 3 parametres
    if (argc != 2 && argc != 3) {
        fprintf(stderr ," %s: inputfile.wav [outputfile.txt]\n", argv[0]);
        return  -1;
    }

    //Fitxers IO
    FILE *fpr;
    fpr = fopen(argv[1], "r");
    FILE *fpw;
    fpw = fopen(argv[2], "w");

    //Lectura header
    //Descartem els primers 44 bytes del fitxer (capçalera) 
    char header[88];
    fread(header, 1, 44, fpr); //Llegim la capçalera
    

    //Declaració del buffer i factor normalització de la senyal
    short  buffer[NSAMPLES];
    float x[NSAMPLES];
    float  norm_factor = 1.0/ (float) 0x8000; /* 16 bits: 0x8000 = 2^15 */
    int i = 0;

    //While loop. Executem fins que no queden bytes per llegir en el fitxer
    while( fread(buffer, 1, NSAMPLES, fpr) == NSAMPLES)
    {
        //Normalització de la senyal
        for (int i=0; i<NSAMPLES; i++){
            x[i] = (float) buffer[i] * norm_factor;
        }

        //Càlcul de les mètriques
        float power = compute_power(x, NSAMPLES);
        float amplitude = compute_am(x, NSAMPLES);
        float zero_cross_rate = compute_zcr(x, NSAMPLES);

        //Guardem en el fitxer
        fprintf(fpw, "%d\t%f\t%f\t%f\n",i, power, amplitude, zero_cross_rate);

        //Constant per a tenir el numero de la traça
        i = i + 1;
    }

    printf("Execució completada\n");
}
