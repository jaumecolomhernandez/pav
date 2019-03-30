#include <stdio.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <iostream>

#include <ffft/FFTReal.h>

using namespace std;

const int N = 512;

void cepstrum(std::vector<float> &signal, std::vector<float> &cepstrum)
//Aquesta funció calcula el Cepstrum del vector signal
{
    //Objecte FFT
    ffft::FFTReal<float> fft_object(N);

    //Vars
    float x[N], X[N];
    int i, sgn = 1;

    //Delta para evitar errores de /0 y Nan
    float eps = 1e-20;

    //Copiamos la señal y añadimos ceros para llegar a 512
    //ATENCIÓN ZERO PADDING - QUE SUCEDE AL HACER LA INVERSA?
    for (i = 0; i < signal.size(); i++)
    {
        x[i] = signal[i];
    }

    for (; i < N; i++)
    {
        x[i] = 0;
    }
    
    //Càlcul de la FFT, es passen els vectors. El primer es la transformada i el segon es el vector a transformar
    fft_object.do_fft(X, x); // x (real) --FFT---> f (complex)
    
    //Guardem en el vector cepstrum
    //- El primer valor és real
    //- El últim també
    //- La resta tenen part complexa i imaginària i es guarden amb un offset de N/2
    X[0] = log10(X[0] * X[0] + eps);
    for (i = 1; i < N / 2 - 1; i++)
    {
        float re = X[i];
        float im = X[N / 2 + i];
        X[i] = log10(re * re + im * im + eps);
    }
    X[N / 2] = log10(X[N / 2] * X[N / 2] + eps);


    //Hacemos fft inversa y el reescalado
    fft_object.do_ifft (X, x);    // f (complex) --IFFT--> x (real)
    fft_object.rescale (x);       // Post-scaling should be done after FFT+IFFT

    //Copiamos el resultado al vector de resultado
    for (i=0; i<N/2;i++)
    {
        cepstrum[i] = x[i];
    }

    //Debug - Printeja el resultat del Cepstrum 
    //   for (i=0; i< N/2-1; i++)
    //     cout << cepstrum[i] << " ";
    //   cout << "\n";
}