/*                                         Copyright (c) 2011 TNO DIANA BV */
#include "tet_utils.h"

extern double ArcCosinus(double arg) {
  /* Take care of round-off errors */
  assert(arg >= -1.01);
  assert(arg <= 1.01);
  if (arg > 1.) {
    arg = 1.;
  } else if (arg < -1.) {
    arg = -1.;
  }

  return acos(arg);
}

/* Print a positive interger in a string (tight fit) */
extern char *UtilInt2String(char *string, int num) {
  assert(num >= 0);
  if (num < 10) {
    sprintf(string, "%1d", num);
  } else if (num < 100) {
    sprintf(string, "%2d", num);
  } else if (num < 1000) {
    sprintf(string, "%3d", num);
  } else if (num < 10000) {
    sprintf(string, "%4d", num);
  } else if (num < 100000) {
    sprintf(string, "%5d", num);
  } else {
    assert(0);
  }
  return string;
}

extern bool_t UtilNormalize(double *vec, double *vecLen, int dim) {
  bool_t errFlag = FALSE;
  const double len = VecL2Norm(vec, dim);
  if (len > DBL_MIN) {
    VecOperScal(vec, EqlPos, vec, OperTimes, 1 / len, dim);
  } else {
    errFlag = TRUE;
  }
  if (vecLen)
    *vecLen = len;
  return errFlag;
}
