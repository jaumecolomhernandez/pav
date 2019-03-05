#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "vad.h"
#include "pav_analysis.h"

const float FRAME_TIME = 10.0F; /* in ms. */

/*
   As the output state is only ST_VOICE, ST_SILENCE, or ST_UNDEF,
   only this labels are needed. You need to add all labels, in case
   you want to print the internal state in string format */

const char *state_str[] = {
  "UNDEF", "S", "V", "INIT"
};

const char *state2str(VAD_STATE st) {
  return state_str[st];
}

/* Define a datatype with interesting features */
typedef struct {
  float zcr;
  float p;
  float am;
} Features;

/* TODO: Delete and use your own features! */
//const float *x ?????
Features compute_features(float *x, int N) {
  /*
     Input: x[i] : i=0 .... N-1
     Ouput: computed features
  */

  /* DELETE and include a call to your own functions */
  /* For the moment, compute random value
     between 0 and 1
  */

  float x_windowed[N];
  float w[N];
  //float  norm_factor = 1.0/ (float) 0x8000; /* 16 bits: 0x8000 = 2^15 */
  float norm_factor = 1.0;
  int i = 0;
  float w_p;

  hamming_window(&w[0], &w_p, N);
  apply_window(x, &w[0], &x_windowed[0], N);

  Features feat;
  feat.zcr = compute_zcr(x, N);
  feat.p = compute_power(x_windowed, N) - w_p + 10*log10(N);
  feat.am = compute_am(x, N);

  //printf("Power: %0.6f\nAmplitude: %0.6f\nZCR: %0.6f\n\n", feat.p, feat.p, feat.am);

  return feat;
}



/*
   TODO: Init the values of vad_data
 */


VAD_DATA * vad_open(float rate) {
  VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
  vad_data->state = ST_INIT;
  vad_data->sampling_rate = rate;
  vad_data->frame_length = rate * FRAME_TIME * 1e-3;
  return vad_data;
}

VAD_STATE vad_close(VAD_DATA *vad_data) {
  VAD_STATE state = ST_SILENCE;
  /* TODO: decide what to do with the last undecided frames */

  free(vad_data);
  return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data) {
  return vad_data->frame_length;
}



/*
   TODO: Implement the Voice Activity Detection
   using a Finite State Automata
*/

VAD_STATE vad(VAD_DATA *vad_data, float *x) {

  /* TODO
     You can change this, using your own features,
     program finite state automaton, define conditions, etc.
  */

  float POWER_HIGH = 1;
  float POWER_LOW = -1;

  Features f = compute_features(x, vad_data->frame_length);
  vad_data->last_feature = f.p; /* save feature, in case you want to show */


  //printf("power: %0.3f\n", f.p);

  switch (vad_data->state) {
  case ST_INIT:
    vad_data->state = ST_SILENCE;
    break;

  case ST_SILENCE:
    if (f.p > POWER_HIGH)
      vad_data->state = ST_VOICE;
    break;

  case ST_VOICE:
    if (f.p < POWER_LOW)
      vad_data->state = ST_SILENCE;
    break;
  }

  if (vad_data->state == ST_SILENCE ||
      vad_data->state == ST_VOICE)
    return vad_data->state;
  else
    return ST_UNDEF;
}


void vad_show_state(const VAD_DATA *vad_data, FILE *out) {
  fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}
