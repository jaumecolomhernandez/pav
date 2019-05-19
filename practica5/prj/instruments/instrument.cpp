#include <iostream>
#include "instrument_dumb.h"
#include "instrument_not_dumb.h"
#include "instrumentFM.h"
/*
  For each new instrument:
  - Add the header
  - Add the constructor in get_instrument()
*/

using namespace std;

namespace upc {
  Instrument * get_instrument(const string &name,
			      const string &parameters) {
    Instrument * pInst = 0;
    //cout << name << ": " << parameters << endl;
    if (name == "InstrumentDumb") {
      pInst = (Instrument *) new InstrumentDumb(parameters);
    }
    if (name == "InstrumentNotDumb") {
      pInst = (Instrument *) new InstrumentNotDumb(parameters);
    }
    if (name == "InstrumentFM") {
      pInst = (Instrument *) new InstrumentFM(parameters);
    }
    return pInst;
  }
}
