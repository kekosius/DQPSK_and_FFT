/*
 * File: rt_nonfinite.c
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 19-Jul-2023 11:16:34
 */

/*
 * Abstract:
 *      MATLAB for code generation function to initialize non-finites,
 *      (Inf, NaN and -Inf).
 */
/* Include Files */
#include "rt_nonfinite.h"
#include "rtGetInf.h"
#include "rtGetNaN.h"

double rtInf;
double rtMinusInf;
double rtNaN;
float rtInfF;
float rtMinusInfF;
float rtNaNF;

/*
 * Function: rt_InitInfAndNaN ==================================================
 *  Abstract:
 *  Initialize the rtInf, rtMinusInf, and rtNaN needed by the
 * generated code. NaN is initialized as non-signaling. Assumes IEEE.
 */
void rt_InitInfAndNaN(void)
{
  rtNaN = rtGetNaN();
  rtNaNF = rtGetNaNF();
  rtInf = rtGetInf();
  rtInfF = rtGetInfF();
  rtMinusInf = rtGetMinusInf();
  rtMinusInfF = rtGetMinusInfF();
}
/*
 * Function: rtIsInf ==================================================
 *  Abstract:
 *  Test if value is infinite
 */
bool rtIsInf(double value)
{
  return ((value == rtInf || value == rtMinusInf) ? true : false);
}

/*
 * Function: rtIsInfF =================================================
 *  Abstract:
 *  Test if single-precision value is infinite
 */
bool rtIsInfF(float value)
{
  return ((value == rtInfF || value == rtMinusInfF) ? true : false);
}

/*
 * Function: rtIsNaN ==================================================
 *  Abstract:
 *  Test if value is not a number
 */
bool rtIsNaN(double value)
{
  return ((value != value) ? true : false);
}

/*
 * Function: rtIsNaNF =================================================
 *  Abstract:
 *  Test if single-precision value is not a number
 */
bool rtIsNaNF(float value)
{
  return ((value != value) ? true : false);
}

/*
 * File trailer for rt_nonfinite.c
 *
 * [EOF]
 */
