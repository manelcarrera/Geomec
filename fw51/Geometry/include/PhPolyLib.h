/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef _POLYLIB_H_
#define _POLYLIB_H_

#include <math.h>

#include "GeometryExports.h"

#define PHEPS 1e-10

typedef double Value;
#define VALUE_FMT "%f"

#define value_eq(v1, v2) ((v1) == (v2))
#define value_ne(v1, v2) ((v1) != (v2))
#define value_gt(v1, v2) ((v1) > (v2))
#define value_ge(v1, v2) ((v1) >= (v2))
#define value_lt(v1, v2) ((v1) < (v2))
#define value_le(v1, v2) ((v1) <= (v2))

#define value_min(v1, v2) (value_le((v1), (v2)) ? (v1) : (v2))
#define value_max(v1, v2) (value_ge((v1), (v2)) ? (v1) : (v2))

#define value_init(d) ((d) = (0))
#define value_assign(d, v) ((d) = (v))
#define value_absolute(abs_d, d) ((abs_d) = (fabs(d)))
#define value_division(res, num, den) ((res) = (num) / (den))
#define value_oppose(neg, d) ((neg) = -(d))
#define value_clear(d) ((d) = (0))
#define value_zero_p(d) ((fabs(d)) < (PHEPS))
#define value_pos_p(d) ((d) > (0))
#define value_neg_p(d) ((d) < (0))
#define value_multiply(res, d1, d2) ((res) = (d1) * (d2))
#define value_addto(res, d1, d2) ((res) = (d1) + (d2))
#define value_notzero_p(d) ((fabs(d)) > (PHEPS))
#define value_set_si(d, i) ((d) = (Value)(i))
#define value_increment(res, d) ((res) = (d) + 1)
#define value_decrement(res, d) ((res) = (d) - 1)
#define value_one_p(d) (fabs((d) - 1) < (PHEPS))
#define value_mone_p(d) (fabs((d) + 1) < (PHEPS))
#define value_notone_p(d) (fabs((d) - 1) > (PHEPS))
#define value_print(fp, fmt, d) (fprintf(fp, fmt, d))
#define value_read(val, str) (sscanf((str), "%lf", &(val)))
#define value_posz_p(d) ((d) > -(PHEPS))
#define value_add_int(res, d, i) ((res) = (d) + (Value)(i))
#define value_sub_int(res, d, i) ((res) = (d) - (Value)(i))
#define value_modulus(res, nom, den) ((res) = (0))
#define value_substract(res, v1, v2) ((res) = (v1) - (v2))
#define value_swap(d1, d2)                                                                                             \
  {                                                                                                                    \
    Value tmp;                                                                                                         \
    tmp = d2;                                                                                                          \
    d2 = d1;                                                                                                           \
    d1 = tmp;                                                                                                          \
  }
#define value_orto(res, val1, val2) ((res) = (double)((int)(val1) | (int)(val2)))
#define value_maximum(res, val1, val2) ((res) = value_max((val1), (val2)))
#define value_minimum(res, val1, val2) ((res) = value_min((val1), (val2)))

#define __CURRENT_FUNCTION_NAME__ "(unknown)"

#define THROW(what)                                                                                                    \
  (fprintf(stdout, "exception thrown %d, function %s, file %s, line %d", what, __CURRENT_FUNCTION_NAME__, __FILE__,    \
           __LINE__))

#define CATCH(what) if (0)

#define UNCATCH(what)

#define TRY else

extern unsigned int the_last_just_thrown_exception;

#define RETHROW() THROW(the_last_just_thrown_exception)

extern void errormsg1(char *f, char *msgname, char *msg);

#endif
