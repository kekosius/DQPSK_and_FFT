/*
 * File: rtGetInf.c
 *
 * MATLAB Coder version            : 5.5
 * C/C++ source code generated on  : 19-Jul-2023 11:16:34
 */

/*
 * Abstract:
 *       MATLAB for code generation function to initialize non-finite, Inf and
 * MinusInf
 */
/* Include Files */
#include "rtGetInf.h"
#include "rt_nonfinite.h"

/*
 * Function: rtGetInf
 * ================================================================== Abstract:
 *  Initialize rtInf needed by the generated code.
 */
double rtGetInf(void)
{
  double inf = 0.0;
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

    tmpVal.bitVal.words.wordH = 0x7FF00000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    inf = tmpVal.fltVal;
    break;
  }
  case BigEndian: {
    union {
      BigEndianIEEEDouble bitVal;
      double fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0x7FF00000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    inf = tmpVal.fltVal;
    break;
  }
  }
  return inf;
}

/*
 * Function: rtGetInfF
 * ================================================================= Abstract:
 *  Initialize rtInfF needed by the generated code.
 */
float rtGetInfF(void)
{
  IEEESingle infF;
  infF.wordL.wordLuint = 0x7F800000U;
  return infF.wordL.wordLreal;
}

/*
 * Function: rtGetMinusInf
 * ============================================================= Abstract:
 *  Initialize rtMinusInf needed by the generated code.
 */
double rtGetMinusInf(void)
{
  double minf = 0.0;
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

    tmpVal.bitVal.words.wordH = 0xFFF00000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    minf = tmpVal.fltVal;
    break;
  }
  case BigEndian: {
    union {
      BigEndianIEEEDouble bitVal;
      double fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0xFFF00000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    minf = tmpVal.fltVal;
    break;
  }
  }
  return minf;
}

/*
 * Function: rtGetMinusInfF
 * ============================================================ Abstract:
 *  Initialize rtMinusInfF needed by the generated code.
 */
float rtGetMinusInfF(void)
{
  IEEESingle minfF;
  minfF.wordL.wordLuint = 0xFF800000U;
  return minfF.wordL.wordLreal;
}

/*
 * File trailer for rtGetInf.c
 *
 * [EOF]
 */
