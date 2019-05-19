#include <orchest.h>
#include "sndfile.h"
#include "midi_score.h"
#include <set>
using namespace std;
using namespace upc;

float bpm = 180; //beats per minute : tempo
float tpq = 120; //ticks per quarter note: must match the input score

const int PRINT_SCORE = 1;

void warn_unregistered_instruments(const vector<MidiMsg> &score,
                                   const Orchest &orchest, ostream &os)
{
  set<int> channels;
  //Collect channels
  for (unsigned int i = 0; i < score.size(); ++i)
    channels.insert(score[i].getChannel());

  //Check for unregistered and warn
  set<int>::const_iterator it;
  for (it = channels.begin(); it != channels.end(); ++it)
  {
    if (!orchest.is_registered(*it))
      os << "Warning: No instrument registered for channel " << *it << '\n';
  }
}

int main(int argc, const char **argv)
{

  int verbose = 0;
  verbose |= PRINT_SCORE;

  if (argc != 4)
  {
    cerr << argv[0] << " instruments_file score_file output_file.wav\n";
    return -1;
  }

  //Set up the orchest
  Orchest orchest(argv[1]);
  if (!orchest.ready())
  {
    cerr << "Error loading instruments from file: " << argv[1] << endl;
    return -2;
  }

  //Read score
  vector<MidiMsg> score;
  string s(argv[2]);
  if (read_midi_score(s, score))
  {
    cerr << argv[0] << ": Error reading score file: " << argv[2] << endl;
    return -3;
  }

  warn_unregistered_instruments(score, orchest, cout);

  //Open output wave file
  SF_INFO fileInfo;
  fileInfo.channels = 1;
  fileInfo.samplerate = SamplingRate;
  fileInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
  SNDFILE *soundFile = sf_open(argv[3], SFM_WRITE, &fileInfo);
  if (soundFile == 0)
  {
    cerr << "Error opening file " << argv[3] << endl;
    return -4;
  }

  //Sequencer
  float t = 0;                                       //real time, generated and saved
  float step_t = (float)BSIZE / (float)SamplingRate; //Duration of BSIZE samples

  float t_score = 0;                        //time according to the score and tempo (beats-per-second, bps)
  float ticks2seconds = 60.0 / (tpq * bpm); //Duration of one tick in the score
  unsigned long nticks = 0;

  for (unsigned int i = 0; i < score.size(); ++i)
  {
    //if (verbose & PRINT_SCORE) cout << score[i] << endl;

    nticks += score[i].getTick();
    t_score = nticks * ticks2seconds;

    //Audio synthesis till we arrive to next time in the score
    while (t < t_score)
    {
      //cout << "synth:\t" << t << '\t' << t_score << endl;

      const vector<float> &xt = orchest.synthesize();
      if (BSIZE != sf_writef_float(soundFile, xt.data(), BSIZE))
      {
        cerr << "Error writting file " << argv[3] << endl;
        return -2;
      }
      t += step_t;
    }

    orchest.command(score[i].getChannel(), score[i].getEvent(),
                    score[i].getParam1(), score[i].getParam2());
  }

  sf_close(soundFile);
  return 0;
}
