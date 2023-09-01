/*
 * File: rtGetNaN.c
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 19-Jul-2023 11:16:34
 */

/*
 * Abstract:
 *       MATLAB for code generation function to initialize non-finite, NaN
 */
/* Include Files */
#include "rtGetNaN.h"
#include "rt_nonfinite.h"

/*
 * Function: rtGetNaN
 * ======================================================================
 *  Abstract:
 * Initialize rtNaN needed by the generated code.
 *  NaN is initialized as non-signaling. Assumes IEEE.
 */
double rtGetNaN(void)
{
  double nan = 0.0;
  uint16_t one = 1U;
  enum
  {
    LittleEndian,
    BigEndian
  } machByteOrder = (*((uint8_t *)&one) == 1U) ? LittleEndian : BigEndian;
  switch (machByteOrder) {
  case LittleEndian: {
    union {
      LittleEndianIEEEDouble bitVal;
      double fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0xFFF80000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    nan = tmpVal.fltVal;
    break;
  }
  case BigEndian: {
    union {
      BigEndianIEEEDouble bitVal;
      double fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0x7FFFFFFFU;
    tmpVal.bitVal.words.wordL = 0xFFFFFFFFU;
    nan = tmpVal.fltVal;
    break;
  }
  }
  return nan;
}

/*
 * Function: rtGetNaNF
 * =====================================================================
 *  Abstract:
 *  Initialize rtNaNF needed by the generated code.
 *  NaN is initialized as non-signaling. Assumes IEEE
 */
float rtGetNaNF(void)
{
  IEEESingle nanF = {{0}};
  uint16_t one = 1U;
  enum
  {
    LittleEndian,
    BigEndian
  } machByteOrder = (*((uint8_t *)&one) == 1U) ? LittleEndian : BigEndian;
  switch (machByteOrder) {
  case LittleEndian: {
    nanF.wordL.wordLuint = 0xFFC00000U;
    break;
  }

  case BigEndian: {
    nanF.wordL.wordLuint = 0x7FFFFFFFU;
    break;
  }
  }

  return nanF.wordL.wordLreal;
}

/*
 * File trailer for rtGetNaN.c
 *
 * [EOF]
 */
