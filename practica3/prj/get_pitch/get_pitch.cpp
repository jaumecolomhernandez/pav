#include <iostream>
#include <fstream>
#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#define FRAME_LEN 0.03   /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

/**
   Main program: 
   Arguments: 
   - input (wav) file
   - output (txt) file with f0 
     one value per line with the f0 for each frame 
     (or 0 for unvoiced frames)
*/

int main(int argc, const char *argv[])
{
  if (argc != 3)
  {
    cerr << "Usage: " << argv[0] << " input_file.wav output_file.f0\n";
    return -1;
  }

  /// Read input sound file
  unsigned int rate;
  vector<float> x;
  int retv = readwav_mono(argv[1], rate, x);
  if (retv != 0)
  {
    cerr << "Error reading input file: %d\n"
         << "Error value: " << retv << endl;
    return -2;
  }

  FILE *filePointer;

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  ///Define analyzer
  PitchAnalyzer analyzer(n_len, rate, PitchAnalyzer::HAMMING, 50, 500);

  ///Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;


  /*
  float maxVal=0;
  //Es busca el valor absolut màxim de la trama analitzada
  for (int i = 0; i< x.size();i++){
      if (abs(x[i]) > abs(maxVal)){
            maxVal = abs(x[i]);
    }
  }
  //Es defineix com a llindar de decissió el 20% del màxim de la senyal 
  maxVal=maxVal*0.2;
  for (int i = 0; i< x.size();i++){
    if(x[i]>maxVal){
      x[i]=x[i]-maxVal; //En cas de ser una mostra positiva i major que el llindar, hem de restarli aquest llindar a cada mostra
    }
    else if(x[i]<-maxVal){
      x[i]=x[i]+maxVal; // En cas de ser negativa, el llindar té el mateix valor però amb signe negatiu 
    }
    else{
      x[i]=0; //Finalment si la mostra es troba entre maxVal i -maxVal el seu valor passarà a ser 0.
    }
  }
  */



  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift)
  {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  //POSTPROCESSAT - Càlcul del filtre de mitjana
  //Utilitzem un filtre de mitjana per a millorar els resultats del detector
  if (1)
  {
    //Declaració variables
    vector<float> f0_windowed;
    float result;

    //Loop de càlcul
    for (int i = 0; i < f0.size(); i++)
    {
      //Com que implementem una finestra de 5 mostres sol podem agafar 
      //mostres de les posicions >2 i <tamany-2
      if (i < 3 || i >= (f0.size() - 2))
      {
        //Sinó sol copiem el resultat
        f0_windowed.push_back(f0[i]);
      }
      else
      {
        //Càlcul del filtre
        float window[5];
        for (int j = 0; j < 5; ++j) //Copia dels elements a window
          window[j] = f0[i - 2 + j];
        //Ordenem els elements (sòl la mitat)
        for (int j = 0; j < 3; ++j)
        {
          //Trobem la posició del element més petit
          int min = j;
          for (int k = j + 1; k < 5; ++k)
            if (window[k] < window[min])
              min = k;
          //Posem el mínim al seu lloc
          const float temp = window[j];
          window[j] = window[min];
          window[min] = temp;
        }
        //El resultat que volem és l'element del mig
        result = window[2];
        f0_windowed.push_back(result);
      }
    }
    f0 = f0_windowed;
  }

  ///Write f0 contour into the output file
  ofstream os(argv[2]);
  if (!os.good())
  {
    cerr << "Error opening output file " << argv[2] << endl;
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX)
    os << *iX << '\n';
  os << 0 << '\n'; //pitch at t=Dur

  return 0;
}
