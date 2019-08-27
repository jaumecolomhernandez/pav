#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include "vad.h"

#define DEBUG_VAD 0x1

int main(int argc, const char *argv[]) {
  int verbose = 0; //verbose bool

  SNDFILE *sndfile_in, *sndfile_out = 0;
  SF_INFO sf_info;
  FILE *vadfile;
  int n_read, n_write, i;

  VAD_DATA *vad_data;
  VAD_STATE state, last_state;

  float *buffer, *buffer_zeros;
  short *buffer_short;
  int frame_size;        /* in samples */
  float frame_duration;  /* in seconds */
  float t, last_t;

  if (argc != 3 && argc != 4) {
    fprintf(stderr, "Usage: %s input_file.wav output.vad [output_file.wav]\n",
            argv[0]);
    return -1;
  }

  /* Open input sound file */
  sndfile_in = sf_open(argv[1], SFM_READ, &sf_info);
  if (sndfile_in == 0) {
    fprintf(stderr, "Error opening input file: %s\n", argv[1]);
    return -1;
  }

  if (sf_info.channels != 1) {
    fprintf(stderr, "Error: the input file has to be mono: %s\n", argv[1]);
    return -2;
  }

  /* Open vad file */
  vadfile = fopen(argv[2], "wt");
  if (vadfile == 0) {
    fprintf(stderr, "Error opening output vad file: %s\n", argv[2]);
    return -1;
  }

  /* Open output sound file, with same format, channels, etc. than input */
  if (argc == 4) {
    sndfile_out = sf_open(argv[3], SFM_WRITE, &sf_info);
    if (sndfile_out == 0) {
      fprintf(stderr, "Error opening output wav file: %s\n", argv[3]);
      return -1;
    }
  }

  vad_data = vad_open(sf_info.samplerate);
  /* Allocate memory for buffer */
  frame_size   = vad_frame_size(vad_data);
  buffer       = (float *) malloc(frame_size * sizeof(float));
  buffer_zeros = (float *) malloc(frame_size * sizeof(float));
  for (i=0; i< frame_size; ++i) buffer_zeros[i] = 0.0F;

  frame_duration = (float) frame_size/ (float) sf_info.samplerate;
  t = last_t = 0;
  last_state = ST_UNDEF;

  //OWN VARIABLES
  //It is the time we have been in silence
  double silence_time = 0;
  // It is is the trace analysing
  int count = 0;
  //Information vector of three first traces of audio
  Features feats[3];
  //Mean power of the first three traces
  float mean_power=0;
  //Threshold up
  float t_up;
  //Threshold down
  float t_down;
  //System variable to define write behavior
  int silence = 1;

  while(1) { /* For each frame ... */
    n_read = sf_read_float(sndfile_in, buffer, frame_size);

    // Closes the file if it's finished
    if  (n_read != frame_size)
      break;

    //CALCULATES THRESHOLD VALUE DEPENDING ON THE FIRST THREE TRACES
    if (count < 3){
      //Special case on first 3 blocks
      //Saves the first three Features structs
      state = ST_SILENCE;
      feats[count] = compute_features(buffer, vad_data->frame_length);
    }else {
      if(count == 3){
        //Calculates thresholds
        //Calculates mean power and thresholds
        mean_power=(feats[0].p+feats[1].p+feats[2].p)/3;
        printf("%f", mean_power);
        t_up = mean_power + 10; // +10 and +8 are the optimals values we have found
        t_down = mean_power + 8;
      }
      //Calculates vad traditional way
      state = vad(vad_data, buffer, &silence_time, &t_up, &t_down);
    }

    count = count + 1;

    //COPY DATA INTO A .WAV FILE
    if (sndfile_out != 0) {
      //We have defined the "silence" variable to choose if we want to copy the
      //original file or put silence when we have detected there isn't voice
      if (silence == 1){
        if (state == ST_VOICE){
          sf_write_float(sndfile_out, buffer, frame_size);
        }else{
          // buffer_zeros it's and array full of zeros
          sf_write_float(sndfile_out, buffer_zeros, frame_size);
        }
      }else{
        //Copy from the original file whithout modifications
        sf_write_float(sndfile_out, buffer, frame_size);
      }

    }

    if (verbose & DEBUG_VAD)
      vad_show_state(vad_data, stdout);

    //ESCRIPTURA DEL FITXER EN CAS QUE CANVÏI L'ESTAT
    if (state != last_state) {
      if (t != last_t)
        fprintf(vadfile, "%f\t%f\t%s\n", last_t, t, state2str(last_state));
      last_state = state;
      last_t = t;
    }
    t += frame_duration;
  }

  //TODO: MIRAR LA FUNCIÓ VAD_CLOSE
  state = vad_close(vad_data);
  if (t != last_t)
    fprintf(vadfile, "%f\t%f\t%s\n", last_t, t, state2str(state));

  /* clean up: free memory, close open files */
  free(buffer);
  free(buffer_zeros);
  sf_close(sndfile_in);
  fclose(vadfile);
  if (sndfile_out) sf_close(sndfile_out);
  return 0;
}
