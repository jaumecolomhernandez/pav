#include <stdio.h>
#include <math.h>
#include "pav_analysis.h"

float  compute_power(const  float *x, unsigned  int N) {
    //Càlcula la potència del vector x
    float power = 0;
    //Càlculem el quadrat de cada pràctica
    for (int i = 0;i < N; i++){
        power = power + pow(x[i],2);
    }
    power = power/N;
    power = 10*log10(power);
    return power;
}

float  compute_am(const  float *x, unsigned  int N){
    //Càlcula l'amplitud mitja de x
    float amplitude = 0;
    //Càlculem el valor absolut de cada valor
    for (int i = 0;i < N; i++){
        amplitude = amplitude + fabs(x[i]);
    }
    amplitude = amplitude/N;
    return amplitude;
}
float  compute_zcr(const  float *x, unsigned  int N){
    //Càlcula el zero cross rate
    int count = 0;
    for (int i=1;i < N;i++){
        //Mirem si la mostra actual i l'anterior tenen signe diferent
        if((x[i]<0 && x[i-1]>0)){
            count = count + 1;
        }
        else if((x[i]>0 && x[i-1]<0)){
            count = count + 1;
        }
    }
    float value_f = count/(2.0*(N-1));
    //coment per a provar el sync
    return value_f;
}

void hamming_window(float* w, float *w_p, unsigned int N){
    //Computes a Hamming window of N samples and calculates the power of the window (1)
    for (int i=0; i<N; i++){
        w[i] = 0.54 - 0.46*cos(2*M_PI*(i)/(N-1));
    }

    //Càlcul de la potència
    float size = 0.0;
    for (int i=0; i<N; i++){
        size = size + pow(w[i],2);
    }
    *w_p = -10*log10(size);
    //printf("%f",size);
}

void apply_window(float* x, float* w, float* x_windowed, unsigned int N){
    //Applies window to function x
    for (int i =0; i<N; i++){
        x_windowed[i] = (x[i]*w[i]);
    }
}