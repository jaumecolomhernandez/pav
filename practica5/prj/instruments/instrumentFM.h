#ifndef INSTRUMENT_FM
#define INSTRUMENT_FM

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc {
  class InstrumentFM: public upc::Instrument {
    EnvelopeADSR adsr;
    unsigned int index;
    std::vector<float> tbl;

    float l;
    float m;
    
    float step1;
    float step2;

    float velc;
    
    float phase1;
    float phase2;

    float I;
  public:
    InstrumentFM(const std::string &param = "");
    void command(long cmd, long note, long velocity=1); 
    const std::vector<float> & synthesize();
    bool is_active() const {return bActive;} 
  };
}

#endif
