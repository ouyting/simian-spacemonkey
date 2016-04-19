/*
  The contents of this file are subject to the University of Utah Public
  License (the "License"); you may not use this file except in
  compliance with the License.
  
  Software distributed under the License is distributed on an "AS IS"
  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See
  the License for the specific language governing rights and limitations
  under the License.

  The Original Source Code is "teem", released March 23, 2001.
  
  The Original Source Code was developed by the University of Utah.
  Portions created by UNIVERSITY are Copyright (C) 2001, 1998 University
  of Utah. All Rights Reserved.
*/


#include "air.h"
//#include <teem/need/endian.h>
//#include <teem/need/qnanhibit.h>

#ifdef _WIN32
#define TEEM_ENDIAN 1234
#define TEEM_QNANHIBIT 1
#endif

/*
** all this is based on a reading of
** Hennessy + Patterson "Computer Architecture, A Quantitative Approach"
** pages A-13 - A-17
**
** and some assorted web pages
*/

typedef union {
  int i;
  struct {
#if TEEM_ENDIAN == 1234
    unsigned int frac : 23;
    unsigned int exp : 8;
    unsigned int sign : 1;
#else
    unsigned int sign : 1;
    unsigned int exp : 8;
    unsigned int frac : 23;
#endif
  } m;
  float v;
} _airFloat;


/* the hex numbers in braces are examples of C's "initial member of a
   union" aggregate initialization */

#if TEEM_QNANHIBIT == 1
const int airMyQNaNHiBit = 1;
const airFloat airFloatNaN  = {0x7fffffff};
const airFloat airFloatQNaN = {0x7fffffff};
const airFloat airFloatSNaN = {0x7fbfffff};
#else
const int airMyQNaNHiBit = 0;
const airFloat airFloatNaN  = {0x7fbfffff};
const airFloat airFloatQNaN = {0x7fbfffff};
const airFloat airFloatSNaN = {0x7fffffff};
#endif

const airFloat airFloatPosInf = {0x7f800000};
const airFloat airFloatNegInf = {0xff800000};

#define FP_SET(flt, s, e, f) \
  flt.m.sign = s; \
  flt.m.exp = e; \
  flt.m.frac = f

#define FP_GET(s, e, f, flt) \
  s = flt.m.sign; \
  e = flt.m.exp; \
  f = flt.m.frac


/*
******** airFPGen_f()
**
** generates a floating point value which is a member of the given class
*/
float 
airFPGen_f(int cls) {
  _airFloat f;
  
  switch(cls) {
  case airFP_SNAN:
    /* sgn: anything, frc: anything non-zero with high bit !TEEM_QNANHIBIT */
    FP_SET(f, 0, 0xff, (!TEEM_QNANHIBIT << 22) | 0x3fffff);
    break;
  case airFP_QNAN:
    /* sgn: anything, frc: anything non-zero with high bit TEEM_QNANHIBIT */
    FP_SET(f, 0, 0xff, (TEEM_QNANHIBIT << 22) | 0x3fffff);
    break;
  case airFP_POS_INF:
    FP_SET(f, 0, 0xff, 0);
    break;
  case airFP_NEG_INF:
    FP_SET(f, 1, 0xff, 0);
    break;
  case airFP_POS_NORM:
    /* exp: anything non-zero but < 255, frc: anything */
    FP_SET(f, 0, 0x80, 0x7ff000);     
    break;
  case airFP_NEG_NORM:
    /* exp: anything non-zero but < 255, frc: anything */
    FP_SET(f, 1, 0x80, 0x7ff000);     
    break;
  case airFP_POS_DENORM:
    /* frc: anything non-zero */
    FP_SET(f, 0, 0, 0xff);        
    break;
  case airFP_NEG_DENORM:
    /* frc: anything non-zero */
    FP_SET(f, 1, 0, 0xff);        
    break;
  case airFP_POS_ZERO:
    FP_SET(f, 0, 0, 0);
    break;
  case airFP_NEG_ZERO:
    FP_SET(f, 1, 0, 0);
    break;
  default:
    /* User is a moron.  What can you do? */
    f.v = 42;
    break;
  }
  return f.v;
}

/*
******** airFPClass_f()
**
** given a floating point number, tells which class its in
*/
int
airFPClass_f(float val) {
  _airFloat f;
  int sign, exp, frac, index, ret;

  f.v = val;
  FP_GET(sign, exp, frac, f);
  index = (!!sign << 2) | (!!exp << 1) | !!frac;
  switch(index) {
  case 0: ret = airFP_POS_ZERO;   break;
  case 1: ret = airFP_POS_DENORM; break;
  case 2: 
    if (255 > exp)
      ret = airFP_POS_NORM;
    else
      ret = airFP_POS_INF;
    break;
  case 3:
    if (255 > exp)
      ret = airFP_POS_NORM;
    else {
      if (TEEM_QNANHIBIT == frac >> 22)
	ret = airFP_QNAN;
      else
	ret = airFP_SNAN;
    }
    break;
  case 4: ret = airFP_NEG_ZERO;   break;
  case 5: ret = airFP_NEG_DENORM; break;
  case 6:
    if (255 > exp)
      ret = airFP_NEG_NORM;
    else
      ret = airFP_NEG_INF;
    break;
  case 7:
    if (255 > exp)
      ret = airFP_NEG_NORM;
    else {
      if (TEEM_QNANHIBIT == frac >> 22)
	ret = airFP_QNAN;
      else
	ret = airFP_SNAN;
    }
    break;
  }
  return ret;
}

/*
******** airIsNaN()
**
** returns 1 if input is either kind of NaN, 0 otherwise
*/
int
airIsNaN(float g) {
  _airFloat f;
  
  f.v = g;
  return (255 == f.m.exp && f.m.frac);
}

/*
******** airIsInf()
**
** returns 1 if input is positive infinity,
** -1 if negative infinity, 
** or 0 otherwise (including NaN)
*/
int
airIsInf(float f) {
  int c, ret;
  
  c = airFPClass_f(f);
  if (airFP_POS_INF == c)
    ret = 1;
  else if (airFP_NEG_INF == c)
    ret = -1;
  else 
    ret = 0;
  return ret;
}

/*
******** airExists()
**
** returns 1 iff given float is not NaN and is not an infinity
*/
int
airExists(float g) {
  _airFloat f;
  
  f.v = g;
  return (255 > f.m.exp);
}

/*
******** airNaN()
**
** returns a quiet NaN
*/
float
airNaN(void) {

  return airFPGen_f(airFP_QNAN);
}

/*
******** airFPFprintf_f()
**
** prints out the bits of a "float", indicating the three different fields
*/
void
airFPFprintf_f(FILE *file, float val) {
  int i, sign, exp, frac;
  _airFloat f;
  
  if (file) {
    f.v = val;
    FP_GET(sign, exp, frac, f);
    fprintf(file, " %f (class %d) = 0x%x = (s:0x%x, e:0x%x, f:0x%x) = \n", 
            val, airFPClass_f(val), *((int*)&val), sign, exp, frac);
    fprintf(file, " S < . . Exp . . > "
	    "< . . . . . . . . . Frac. . . . . . . . . . >\n");
    fprintf(file, " %d ", sign);
    for (i=7; i>=0; i--)
      fprintf(file, "%d ", (exp >> i) & 1);
    for (i=22; i>=0; i--)
      fprintf(file, "%d ", (frac >> i) & 1);
    fprintf(file, "\n");
  }
}
