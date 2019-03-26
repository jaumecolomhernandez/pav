#include <iostream>
#include <math.h>
#include "pitch_analyzer.h"
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

  float sum;
  int i, j;

  for (i = 0; i < size; ++i)
  {
    sum = 0;
    for (j = 0; j < size - i; j++)
    { //for each calculate sum
      sum += x[j] * x[j + i];
    }
    r[i] = sum;
  }

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
  //HEM DE PENSAR QUE FIQUEM AQUÍ!

  bool debug = true; //Flag to print data to files for posterior use in wavesurfer

  if (debug)
  {
    //Prints data to file
    //PENSAR MANERA DE PASAR UN PUNTERO POR REFERENCIA!
    //printf("%f\t%f\t%f\n", pot, r1norm, rmaxnorm);
  }
  if (pot>-40.0 && r1norm>0.5){
    return false;
  }else{
    return true;
  }
}

float PitchAnalyzer::compute_pitch(vector<float> &x) const
{
  //Main function, computes the pitch and returns it
  //We pass a float vector with the trace

  //If we are at the end of the file end,
  if (x.size() != frameLen)
    return -1.0F;

  //WINDOW INPUT FRAME
  //Aplica la finestra; PREGUNTAR SOBRE EL SOLAPAMENT DE FINESTRA PQ EN AQUEST CAS NO N'HI HA
  for (unsigned int i = 0; i < x.size(); ++i)
    x[i] *= window[i];

  //COMPUTES CORRELATION
  //Ens retorna un vector amb les autocorrelacions de llargada frameLen
  //En el zero te la potència => x[n]*x[n]
  vector<float> r(npitch_max);
  autocorrelation(x, r);

  //CÀLCUL DEL VALOR MÀXIM EN L'AUTOCORRELACIÓ
  //El lag del màxim es troba en mostra*samplingFrequency i per a treure la
  //frèquencia fem el invers.
  bool firstNegative = false;
  float maxVal = 0; //Inicialitzem a zero, suposem que l'autocorr max sempre > 0
  int index = 0;

  for (int i = 0; i < frameLen; i++)
  {
    //Primer comprovem que haguem trobat un negatiu
    if (firstNegative)
    {
      //Després si és un màxim
      if (r[i] > maxVal){
        index = i;
        maxVal = r[i];
      }
    }
    else if (r[i] < 0)
    { //En cas que sigui menor que zero
      firstNegative = true;
    }
  }

  //printf("%d\n",index);
  float frequency = 1.0 / (float)index * (float)samplingFreq;
  float pot = 10 * log10(r[0]);

  //MIRAR QUE POT PASSAR PER LA FREQÜENCIA SURTI MALAMENT!
  //printf("%f\n",frequency);

  //Comprovamos que sea un tramo con voz
  //Sino no se puede sacar el pitch

  #if 0
    if (r[0] > 0.0F)
      cout << pot << '\t' << r[1] / r[0] << '\t' << r[index] / r[0] << endl;
  #endif

  //#if 1
    //int zcr = compute_zcr(&x[0],frameLen);
    //if (r[0] > 0.0F)
    //  cout << zcr << endl;
  //#endif

  if (unvoiced(pot, r[1] / r[0], r[index] / r[0]) or index == 0)
    return 0;
  else
    return frequency;

  //DE AQUÍ PARA ABAJO NO SE QUE HACE NADA

  // ESTE ITERADOR PARA QUE SE USA?
  //vector<float>::const_iterator iR = r.begin(), iRMax = iR;

  /* QUE HACE ESTO?
    unsigned int lag = 0;

    if (iRMax != r.end()) //normal case
      lag = iRMax - r.begin();
    */

  //float pot = 10 * log10(r[0]);

  //You can print these (and other) features, look at them using wavesurfer
  //Based on that, implement a rule for unvoiced
  //change to #if 1 and compile

  /*
    if (unvoiced(pot, r[1]/r[0], r[lag]/r[0]) or lag == 0)
      return 0;
    else
      return (float) samplingFreq/(float) lag;
    */
}

} // namespace upc
