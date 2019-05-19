#include <iostream>
#include <math.h>
#include "instrument_dumb.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

InstrumentDumb::InstrumentDumb(const std::string &param) 
  : adsr(SamplingRate, param) {
  bActive = false;
  x.resize(BSIZE);

  /*
    You can use the class keyvalue to parse "param" 
    and configure your instrument.
    Take a Look at keyvalue.h    
  */

  KeyValue kv(param);
  int N;

  if (!kv.to_int("N",N))
    N = 40; //default value
  
  //Create a tbl with a tone, fixed frequency
  tbl.resize(N);
  float phase = 0, step = 2 * M_PI /(float) N;
  index = 0;
  for (int i=0; i < N ; ++i) {
    tbl[i] = sin(phase);
    phase += step;
  }

}

void InstrumentDumb::command(long cmd, long note, long vel) {

  if (cmd == 9) {
    bActive = true;
    adsr.start();
    index = 0;
  } else if (cmd == 0 || cmd == 8) {
    adsr.stop();
  }
}


const vector<float> & InstrumentDumb::synthesize() {

  if (!bActive)
    return x;

  if (!adsr.active()) {
    bActive = false;
    x.assign(x.size(), 0);
    return x;
  }

  for (unsigned int i=0; i<x.size(); ++i) {
    x[i] = tbl[index++];
    if (index == tbl.size())
      index = 0;
  }
  adsr(x); //apply envelope to x (and updates internal status of adsr)

  return x;
}
