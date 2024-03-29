#ifndef PAV_ORCHEST
#define PAV_ORCHEST

#include <map>
#include <string>
#include "instrument.h"

namespace upc {
  class Orchest {  
    typedef std::map<unsigned int, Instrument *> Instruments;
    typedef Instruments::iterator iterator;
    typedef Instruments::const_iterator const_iterator;
    Instruments instruments;
    std::vector<float> xt;
    bool anyActive, bReady;
    float gain;

  public:
  Orchest() : xt(BSIZE), anyActive(false), bReady(false), gain(0.5) {}
    Orchest(const std::string conf_filename); //: xt(BSIZE), anyActive = false {}
    int register_instrument(unsigned int channel, const std::string &name, 
			    const std::string &parameters = "");
    int release_instrument(unsigned int channel);
    int command(unsigned int channel, unsigned int cmd,
		unsigned int param1,  unsigned int param2);
    const std::vector<float> & synthesize();
    bool is_registered(unsigned int channel) const {
      return (instruments.find(channel) != instruments.end()); 
    }
    bool is_active() const {return anyActive;}
    bool ready() const {return bReady;}
  };
}

#endif

