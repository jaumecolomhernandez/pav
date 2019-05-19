#include <iostream>
#include <math.h>
#include "instrument_not_dumb.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

InstrumentNotDumb::InstrumentNotDumb(const std::string &param) 
  : adsr(SamplingRate, param) {
  bActive = false;
  x.resize(BSIZE);

  /*
    You can use the class keyvalue to parse "param" 
    and configure your instrument.
    Take a Look at keyvalue.h    
  */

  cout << param << endl;

  KeyValue kv(param);
  int N;

  if (!kv.to_int("N",N))
    N = 60; //default value
  
  

}

void InstrumentNotDumb::command(long cmd, long note, long vel) {
  if (cmd == 9) {
    bActive = true;
    adsr.start();
   


    float f0=440.0*pow(2,(((float)note-69.0)/12.0));     
    float nota = f0/SamplingRate;

    step = 2 * M_PI * nota;
    velc = vel/127.0;
    index = 0;
    phase = 0;

  
  } else if (cmd == 0 || cmd == 8) {
    adsr.stop();
  }
}


const vector<float> & InstrumentNotDumb::synthesize() {

  if (!bActive)
    return x;

  if (!adsr.active()) {
    bActive = false;
    x.assign(x.size(), 0);
    return x;
  }

  
  for (unsigned int i=0; i<x.size(); ++i) {
    x[i] = 0.1*velc*sin(phase);

    phase += step;

    while(phase>2*M_PI) phase -= 2*M_PI;
  }

  adsr(x); //apply envelope to x (and updates internal status of adsr)

  return x;
}
