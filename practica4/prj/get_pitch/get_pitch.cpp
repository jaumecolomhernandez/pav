#include <iostream>
#include <fstream>
#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#define FRAME_LEN 0.05   /* 30 ms. */
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


  /*FILE * pFile;
   int n;
   pFile = fopen ("myfile.txt","w");
  for (int i = 0; i< x.size();i++){
     fprintf (pFile, "%f\n",x[i]);
   }
   fclose (pFile);
  
  float maxVal=0;
  for (int i = 0; i< x.size();i++){
    if(x[i]>0){
      if (abs(x[i]) > abs(maxVal)){
            maxVal = abs(x[i]);
      }
    }
  }
  maxVal=maxVal*0.2;
  for (int i = 0; i< x.size();i++){
    if(x[i]>maxVal){
      x[i]=x[i]-maxVal;
    }
    else if(x[i]<-maxVal){
      x[i]=x[i]+maxVal;
    }
    else{
      x[i]=0;
    }
  }
  
  FILE * p2File;
   int n;
   p2File = fopen ("myfileout.txt","w");
  for (int i = 0; i< x.size();i++){
     fprintf (p2File, "%f\n",x[i]);
   }
   fclose (p2File);
  */



  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift)
  {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  if (1)
  {
    vector<float> f0_windowed;
    float result;
    for (int i = 0; i < f0.size(); i++)
    {
      if (i < 3 || i >= (f0.size() - 2))
      {
        f0_windowed.push_back(f0[i]);
      }
      else
      {
        float window[5];
        for (int j = 0; j < 5; ++j) //Copy elements to window
          window[j] = f0[i - 2 + j];
        //Order elements (only half of them)
        for (int j = 0; j < 3; ++j)
        {
          //Find position of minimum element
          int min = j;
          for (int k = j + 1; k < 5; ++k)
            if (window[k] < window[min])
              min = k;
          //Put found minimum element in its place
          const float temp = window[j];
          window[j] = window[min];
          window[min] = temp;
        }
        //Get result - the middle element
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
