/*
 * File: fftM.c
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 19-Jul-2023 11:16:34
 */

/* Include Files */
#include "fftM.h"
#include "FFTImplementationCallback.h"
#include "fftM_data.h"
#include "fftM_initialize.h"
#include "rt_nonfinite.h"
#include <math.h>

/* Function Declarations */
static double rt_hypotd_snf(double u0, double u1);

/* Function Definitions */
/*
 * Arguments    : double u0
 *                double u1
 * Return Type  : double
 */
static double rt_hypotd_snf(double u0, double u1)
{
  double a;
  double b;
  double y;
  a = fabs(u0);
  b = fabs(u1);
  if (a < b) {
    a /= b;
    y = b * sqrt(a * a + 1.0);
  } else if (a > b) {
    b /= a;
    y = a * sqrt(b * b + 1.0);
  } else if (rtIsNaN(b)) {
    y = rtNaN;
  } else {
    y = a * 1.4142135623730951;
  }
  return y;
}

/*
 * Arguments    : const double A[256]
 *                double freqPerc[6]
 * Return Type  : void
 */
void fftM(const double A[256], double freqPerc[6])
{
  cdouble yCol[256];
  double ai;
  double xtmp_im;
  double xtmp_re;
  int a;
  int b_yCol_tmp;
  int dim;
  int i;
  int i1;
  int i2;
  int ib;
  int k;
  int lowerDim;
  int npages;
  int vlend2;
  int yCol_tmp;
  if (!isInitialized_fftM) {
    fftM_initialize();
  }
  c_FFTImplementationCallback_doH(A, yCol);
  for (yCol_tmp = 0; yCol_tmp < 256; yCol_tmp++) {
    xtmp_re = yCol[yCol_tmp].re;
    ai = yCol[yCol_tmp].im;
    if (ai == 0.0) {
      xtmp_im = xtmp_re / 256.0;
      xtmp_re = 0.0;
    } else if (xtmp_re == 0.0) {
      xtmp_im = 0.0;
      xtmp_re = ai / 256.0;
    } else {
      xtmp_im = xtmp_re / 256.0;
      xtmp_re = ai / 256.0;
    }
    yCol[yCol_tmp].re = xtmp_im;
    yCol[yCol_tmp].im = xtmp_re;
  }
  for (dim = 0; dim < 2; dim++) {
    a = 255 * dim;
    yCol_tmp = 255 * dim + 1;
    if (yCol_tmp > 1) {
      vlend2 = yCol_tmp / 2 - 1;
      npages = 1;
      lowerDim = dim + 2;
      for (k = lowerDim; k < 3; k++) {
        npages <<= 8;
      }
      if ((vlend2 + 1) << 1 == yCol_tmp) {
        i2 = 0;
        for (i = 0; i < npages; i++) {
          i1 = i2;
          i2 += a;
          i1++;
          i2++;
          ib = i1 + vlend2;
          for (k = 0; k <= vlend2; k++) {
            lowerDim = (i1 + k) - 1;
            xtmp_re = yCol[lowerDim].re;
            xtmp_im = yCol[lowerDim].im;
            b_yCol_tmp = ib + k;
            yCol[lowerDim] = yCol[b_yCol_tmp];
            yCol[b_yCol_tmp].re = xtmp_re;
            yCol[b_yCol_tmp].im = xtmp_im;
          }
        }
      } else {
        i2 = 0;
        for (i = 0; i < npages; i++) {
          i1 = i2;
          i2 += a;
          i1++;
          i2++;
          ib = i1 + vlend2;
          xtmp_re = yCol[ib].re;
          xtmp_im = yCol[ib].im;
          for (k = 0; k <= vlend2; k++) {
            b_yCol_tmp = ib + k;
            yCol_tmp = (i1 + k) - 1;
            yCol[b_yCol_tmp] = yCol[yCol_tmp];
            yCol[yCol_tmp] = yCol[b_yCol_tmp + 1];
          }
          ib = (ib + vlend2) + 1;
          yCol[ib].re = xtmp_re;
          yCol[ib].im = xtmp_im;
        }
      }
    }
  }
  xtmp_re = 0.0;
  for (i = 0; i < 128; i++) {
    xtmp_im = rt_hypotd_snf(yCol[i].re, yCol[i].im);
    if (xtmp_im > xtmp_re) {
      xtmp_re = xtmp_im;
    }
  }
  for (i = 0; i < 6; i++) {
    yCol_tmp = (i << 1) + 131;
    freqPerc[i] = rt_hypotd_snf(yCol[yCol_tmp].re, yCol[yCol_tmp].im) / xtmp_re;
  }
}

/*
 * File trailer for fftM.c
 *
 * [EOF]
 */
