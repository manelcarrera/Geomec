/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef TET_UTILS_H
#define TET_UTILS_H

#include "assert.h"
#include "float.h"
#include "lbgs.h"
#include "limits.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "Utils.h"

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884197169399375105820974944592308
#endif

extern bool_t UtilNormalize(double *, double *, int);
extern char *UtilInt2String(char *, int);
extern double ArcCosinus(double);

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif // UNUSED

#endif
