#ifndef ROTATE_H
#define ROTATE_H
#include <stdio.h>
#include <math.h>

#define Pi 3.141592653589

#include "bilinear_interpolation.h"

float*** rotate (float*** input, int M_in, int N_in);

#endif
