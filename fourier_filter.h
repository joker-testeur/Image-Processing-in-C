#ifndef FOURIER_FILTER_H
#define FOURIER_FILTER_H

#include <stdio.h>
#include <math.h>
void discrete_fourier_transform(float xr[], float xi[], int N, int inverse);
float*** fourier_filter(float*** input, int M_in, int N_in);

#endif
