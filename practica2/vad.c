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

// Hem definit el struct Features a vad.h

Features compute_features(float *x, int N) {
  /*Computes features and returns a Features struct*/

  float x_windowed[N];
  float w[N];
  float norm_factor = 1.0;
  int i = 0;
  float w_p;

  hamming_window(&w[0], &w_p, N);
  apply_window(x, &w[0], &x_windowed[0], N);

  Features feat;
  feat.zcr = compute_zcr(x, N);
  feat.p = compute_power(x_windowed, N) - w_p + 10*log10(N);
  feat.am = compute_am(x, N);

  //Debug print
  //printf("Power: %0.6f\nAmplitude: %0.6f\nZCR: %0.6f\n\n", feat.p, feat.p, feat.am);

  return feat;
}

/*
   TODO: Init the values of vad_data
   QUE EM DE FER AQUI?
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
  /* TODO: decide what to do with the last undecided frames 
  HEM DE PENSAR ALGO PER A FICAR A AQUESTA FUNCIO
  */
  free(vad_data);
  return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data) {
  return vad_data->frame_length;
}


VAD_STATE vad(VAD_DATA *vad_data, float *x, double *silence_time, int count, float *t_up, float *t_down) {

  double window_time=0.01; //160 samples / 16khz
  // int POWER_HIGH = 3;
  // int POWER_LOW = -1;
  float POWER_HIGH = *t_up;
  float POWER_LOW = *t_down; 
  float MAX_SILENCE_TIME = 0.35;

  Features f = compute_features(x, vad_data->frame_length);
  vad_data->last_feature = f.p; /* save feature, in case you want to show */

  //printf("power: %0.3f\n", f.p);

  switch (vad_data->state) {
    case ST_INIT:
      //Iniciem estat a silenci
      vad_data->state = ST_SILENCE;
      break;

    case ST_SILENCE:
      //Passem a só 
      if (f.p > POWER_HIGH)
        vad_data->state = ST_VOICE;
      break;

    case ST_VOICE:
      //Ús de finestra temporal per a canviar d'estat
      if (f.p < POWER_LOW){
        //Càlcul del temps en silènci
        *silence_time = *silence_time + window_time;
        if (*silence_time>MAX_SILENCE_TIME){
          //Canvi d'estat 
          vad_data->state = ST_SILENCE;
          *silence_time=0;
        }
      }
      else if (f.p > POWER_HIGH)
        // Posem a zero si hi ha só
        *silence_time=0;
      break;
  }

  if (vad_data->state == ST_SILENCE ||
      vad_data->state == ST_VOICE)
    //En cas que tinguém estat definit
    return vad_data->state;
  else
    return ST_UNDEF;
}


void vad_show_state(const VAD_DATA *vad_data, FILE *out) {
  fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}
