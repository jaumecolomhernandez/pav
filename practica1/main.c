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
    char header[44];
    fread(header, 1, 44, fpr); //Llegim la capçalera

    //Declaració del buffer i factor normalització de la senyal
    short  buffer[NSAMPLES];
    float x[NSAMPLES];
    float x_windowed[NSAMPLES];
    float w[NSAMPLES];
    float  norm_factor = 1.0/ (float) 0x8000; /* 16 bits: 0x8000 = 2^15 */
    int i = 0;
    float w_p;

    //AMPLIACIÓ: Inicialització de la finestra
    hamming_window(&w[0], &w_p, NSAMPLES);

    //While loop. Executem fins que no queden bytes per llegir en el fitxer
    while( fread(buffer, 2, NSAMPLES, fpr) == NSAMPLES)
    {
        //Normalització de la senyal
        for (int i=0; i<NSAMPLES; i++){
            x[i] = (float) buffer[i] * norm_factor;
        }

        //AMPLIACIÓ: Càlcul de la senyal enfinestrada
        apply_window(&x[0], &w[0], &x_windowed[0], NSAMPLES);

        //Càlcul de les mètriques
        float power = compute_power(x_windowed, NSAMPLES);
        //AMPLIACIÓ: segons la definició de la potencia en l'apartat 4
        //és calcula sense dividir per N
        power = power - w_p + 10*log10(NSAMPLES);
        float amplitude = compute_am(x, NSAMPLES);
        float zero_cross_rate = compute_zcr(x, NSAMPLES);

        //Guardem en el fitxer
        fprintf(fpw, "%d\t%f\t%f\t%f\n",i, power, amplitude, zero_cross_rate);

        //Constant per a tenir el numero de la traça
        i = i + 1;
    }

    printf("Execució completada!\n");
}
