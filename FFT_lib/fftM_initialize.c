/*
 * File: fftM_initialize.c
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 19-Jul-2023 11:16:34
 */

/* Include Files */
#include "fftM_initialize.h"
#include "fftM_data.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : void
 * Return Type  : void
 */
void fftM_initialize(void)
{
  rt_InitInfAndNaN();
  isInitialized_fftM = true;
}

/*
 * File trailer for fftM_initialize.c
 *
 * [EOF]
 */
