/* GTS - Library for the manipulation of triangulated surfaces
 * Copyright (C) 1999 Stéphane Popinet
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"

#ifdef HAVE_FPU_CONTROL_H
#  include <fpu_control.h>
   static fpu_control_t fpu_round_double =
     (_FPU_DEFAULT & ~ _FPU_EXTENDED)|_FPU_DOUBLE;
   static fpu_control_t fpu_init;
#  define FPU_ROUND_DOUBLE  { _FPU_GETCW(fpu_init);\
                             _FPU_SETCW(fpu_round_double); }
#  define FPU_RESTORE       {_FPU_SETCW(fpu_init);}
#else /* not HAVE_FPU_CONTROL_H */
#  ifdef __FreeBSD__
#    include <floatingpoint.h>
#    define FPU_ROUND_DOUBLE  (fpsetprec(FP_PD))
#    define FPU_RESTORE       (fpsetprec(FP_PE))
#  else /* not __FreeBSD__ */
#    if 0
//   we skip setting the precision flag, as it is not supported on WIN32 x64 (bug 81153)
//#    ifdef WIN32 
#      ifdef _MSC_VER
#        include <float.h>
         static unsigned int fpu_init;
#        define FPU_ROUND_DOUBLE (fpu_init = _controlfp (0, 0),\
                                 _controlfp (_PC_53, MCW_PC))
#        define FPU_RESTORE      (_controlfp (fpu_init, 0xfffff))
#      else /* not _MSC_VER */
#        error "You need the Microsoft C compiler for the Win32 version"
#      endif /*  not _MSC_VER */
#    else /* not WIN32 */
/*#      warning "Unknown CPU: assuming default double precision rounding"*/
#      define FPU_ROUND_DOUBLE
#      define FPU_RESTORE
#    endif /* not WIN32 */
#  endif /* not __FreeBSD__ */
#endif /* not HAVE_FPU_CONTROL_H */
