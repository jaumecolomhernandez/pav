#include <iostream>
#include <math.h>
#include "pitch_analyzer.h"
#include "fft.h"
#include "pav_analysis.h"
using namespace std;

namespace upc
{
void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const
{
  //This function returns the autocorrelation of vector x, via the parameter r
  //Computes => R(i) = Σ(x[n]*x[n+i])
  //Returns a FrameLen length vector
  short size = r.size();

  //printf("%d aa\n",size);

  float sum;
  int i, j;

  bool normalization = false;

  for (i = 0; i < size; ++i)
  {
    sum = 0.0;
    int n_size = size - i;
    for (j = 0; j < n_size; j++)
    { //for each calculate sum
      sum = sum + x[j] * x[j + i];
    }
    if (normalization)
    //OJO AQUÍ
    //Añadimos un factor de normalización al resultado de la autocorrelacion,
    //esto se basa en que se calcula la suma con size - i operaciones, entonces los 
    //últimos valores tienen mucho menos peso. Este factor de normalización solventa esto.
    //Añadimos el 1000 para que sea todo más grande.
    {
      r[i] = sum * 1000.0 / n_size;
    }
    else
    {
      r[i] = sum;
    }
    //DEBUG - 
    printf("%f ",sum);
  }
  printf("\n\n");

  if (r[0] == 0.0F) //to avoid log() and divide zero
    r[0] = 1e-10;
}




void PitchAnalyzer::set_window(Window win_type)
{
  //This function returns the window to be used with the trace
  //It can return a rectangular or Hamming window. In both cases it will have FrameLen length
  if (frameLen == 0)
    return;

  window.resize(frameLen);

  switch (win_type)
  {
  case HAMMING:
    //Hamming window case
    //Implemented as http://mathworld.wolfram.com/HammingFunction.html
    for (int i = 0; i < frameLen; i++)
    {
      window[i] = 0.54 - 0.46 * cos(2 * M_PI * (i) / (frameLen - 1));
    }
    break;
  case RECT:
    //Rectangular window case
    window.assign(frameLen, 1);
    break;
  default:
    //Also a rectangular window
    window.assign(frameLen, 1);
    break;
  }
}

void PitchAnalyzer::set_f0_range(float min_F0, float max_F0)
{
  //Sets the f0 range
  npitch_min = (unsigned int)samplingFreq / max_F0;
  if (npitch_min < 2)
    npitch_min = 2; // samplingFreq/2

  npitch_max = 1 + (unsigned int)samplingFreq / min_F0;

  //frameLen should include at least 2*T0
  if (npitch_max > frameLen / 2)
    npitch_max = frameLen / 2;
}

bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm) const
{
  //Returns if the trace is voiced or not.

  //if (pot > -18.00 && (r1norm > 0.75 || rmaxnorm >0.65)) //Valors optims per al cas de autocorrelació
  if (pot > -20.0 && r1norm >0.8)  //valors optims per cepstrum
  {
    return false;
  }
  else
  {
    return true;
  }
}

float PitchAnalyzer::compute_pitch(vector<float> &x) const
{
  //Main function, computes the pitch and returns it
  //We pass a float vector with the trace

  //If we are at the end of the file end,
  if (x.size() != frameLen)
  {
    return -1.0F;
  }

  //WINDOW INPUT FRAME
  for (unsigned int i = 0; i < x.size(); ++i)
    x[i] = x[i] * window[i];

  //COMPUTES CORRELATION
  //Ens retorna un vector amb les autocorrelacions de llargada frameLen
  //En el zero te la potència => x[n]*x[n]
  vector<float> r(npitch_max);
  autocorrelation(x, r);

  //CÀLCUL DEL VALOR MÀXIM EN L'AUTOCORRELACIÓ
  //El lag del màxim es troba en mostra*samplingFrequency i per a treure la
  //frèquencia fem el invers.
  bool firstNegative = false;
  float maxVal = 0.0; //Inicialitzem a zero, suposem que l'autocorr max sempre > 0
  int index = 0;

  for (int i = 0; i < r.size(); i++)
  {
    //Primer comprovem que haguem trobat un negatiu ja que amb la correlació després del primer pic i abans del següent
    //hi ha un pas per 0
    //També es posa com a condició i>60 per a tenir una resolució màxima de fins a 333Hz
    if (firstNegative && i>60)
    {
      //Després comprobem si és un màxim
      if (r[i] > maxVal)
      {
        index = i;
        maxVal = r[i];
      }
    }
    else if (r[i] < 0)
    { //En cas que sigui menor que zero
      firstNegative = true;
    }
  }

  //CÀLCUL DEL CEPSTRUM
  vector<float> c(npitch_max);
  int sizepre=x.size();
  cepstrum(x, c);
  int sizepos=x.size();

  //Càlcul del valor màxim del Cepstrum

  float maxVal2 = 0; //Inicialitzem a 0, suposem que el cepstrum max sempre > 0
  int index2 = 0;
  //Amb n<285 estem tenint en compte les frequencies de pitch a partir de 70 Hz
  //Amb n>60 com a molt tenim una resolució de fins a 333Hz, valor que es troba molt per sobre de la mitjana de les dones,
  //tot i que el pitch pot arribar fins a 500Hz al detectar homes i dones si es possa un rang tant ampli i han molts errors.
  for (int n=60; n < 285;n++)
  {
    //Debug - Print dels valors del cepstrum
    //cout << c[i] << " ";

    //Comprovem que sigui major que el anterior màxim
    if (c[n] > maxVal2)
    {
      index2 = n;
      maxVal2 = c[n];
    }
  }
  
  //Debug -  print del tamany de de la finestra
  //printf("%d %d\n", sizepre,sizepos);

  //Debug - Print del valor màxim de l'autocorrelació i del cepstrum
  //printf("\n%d-%d\n", index, index2);
  //printf("%d\n",samplingFreq);

  //https://www.johndcook.com/blog/2016/05/18/cepstrum-quefrency-and-pitch/

  float frequency =1/(float)index * (float)samplingFreq;
  float pot = 10 * log10(r[0]);

//Comprovem que sigui una trama de veu 
//Sino no es pot extreure el pitch

//Debug - Printeja potencia, r1/pot i rmax/pot
#if 0
      if (r[0] > 0.0F)
        cout << pot << '\t' << r[1] / r[0] << '\t' << r[index] / r[0] << endl;
#endif

  if (unvoiced(pot, r[1] / r[0], r[index] / r[0]) or index == 0)
    return 0.0;
  else
    return frequency;
}

} // namespace upc
