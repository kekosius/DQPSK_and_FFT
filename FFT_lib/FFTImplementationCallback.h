/*
 * File: FFTImplementationCallback.h
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 19-Jul-2023 11:16:34
 */

#ifndef FFTIMPLEMENTATIONCALLBACK_H
#define FFTIMPLEMENTATIONCALLBACK_H

/* Include Files */
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
typedef struct {
	double re;
	double im;
} cdouble;

void c_FFTImplementationCallback_doH(const double x[256], cdouble y[256]);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for FFTImplementationCallback.h
 *
 * [EOF]
 */
