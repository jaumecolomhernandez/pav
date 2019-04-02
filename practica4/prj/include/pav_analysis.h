#ifndef  PAV_ANALYSIS_H
#define  PAV_ANALYSIS_H

#ifdef __cplusplus
extern "C" {
#endif

float  compute_power(const  float *x, unsigned  int N);
float  compute_am(const  float *x, unsigned  int N);
float  compute_zcr(const  float *x, unsigned  int N);
void hamming_window(float* w, float *w_p, unsigned int N);
void apply_window(float* x, float* w, float* x_windowed ,unsigned int N);

#ifdef __cplusplus
}
#endif

#endif
