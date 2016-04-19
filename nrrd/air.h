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

#ifndef AIR_HAS_BEEN_INCLUDED
#define AIR_HAS_BEEN_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#if 0
#include <sys/time.h> 
#endif
#include <math.h>
#include <string.h>

#define AIR_SMALL_STRLEN 64

/*
******** airFP enum
**
** the different kinds of floating point number afforded by IEEE 754
**
** the values probably won't agree with those in #include's like
** ieee.h, ieeefp.h, fp_class.h, but there don't seem to be 
** completely standard #define names for these anyway, so what's
** the point.
*/
typedef enum {
  airFP_Unknown,               /*  0: nobody knows */
  airFP_SNAN,                  /*  1: signalling NaN */
  airFP_QNAN,                  /*  2: quiet NaN */
  airFP_POS_INF,               /*  3: positive infinity */
  airFP_NEG_INF,               /*  4: negative infinity */
  airFP_POS_NORM,              /*  5: positive normalized non-zero */
  airFP_NEG_NORM,              /*  6: negative normalized non-zero */
  airFP_POS_DENORM,            /*  7: positive denormalized non-zero */
  airFP_NEG_DENORM,            /*  8: negative denormalized non-zero */
  airFP_POS_ZERO,              /*  9: +0.0, positive zero */
  airFP_NEG_ZERO,              /* 10: -0.0, negative zero */
  airFP_Last                   /* after the last valid one */
} airFP;

/*
******** airEndian enum
**
** for identifying how a file was written to disk, for those encodings
** where the data on disk is dependent on the endianness of the
** architecture.
*/
typedef enum {
  airEndianUnknown,         /* 0: nobody knows */
  airEndianLittle = 1234,   /* 1234: Intel and friends */
  airEndianBig = 4321,      /* 4321: the rest */
  airEndianLast
} airEndian;

/* array.c: poor-man's dynamically resizable arrays */
typedef struct {
  void *data,         /* where the data is */
    **dataP;          /* (possibly NULL) address of user's data variable,
			 kept in sync with internal "data" variable */
  int len,            /* length of array: # units for which there is
		         considered to be data (which is <= total # units
		         allocated).  The # bytes which contain data is
		         len*unit.  Always updated (unlike "*lenP") */
    *lenP,            /* (possibly NULL) address of user's length variable,
			 kept in sync with internal "len" variable */
    incr,             /* the granularity of the changes in amount of space
			 allocated: when the length reaches a multiple of
			 "incr", then the array is resized */
    unit,             /* the size in bytes of one element in the array */
    size;             /* array is allocated to have "size" increments, or,
			 size*incr elements, or, 
			 size*incr*unit bytes */

  /* the following are all callbacks useful for maintaining either an array
     of pointers (allocCB and freeCB) or array of structs (initCB and
     doneCB).  allocCB or initCB is called when the array length increases,
     and freeCB or doneCB when it decreases.  Any of them can be NULL if no
     such activity is desired.  allocCB sets values in the array (as in
     storing the return from malloc(); freeCB is called on values in the
     array (as in calling free()), and the values are cast to void*.  allocCB
     and freeCB don't care about the value of "unit" (though perhaps they
     should).  initCB and doneCB are called on the _addresses_ of elements in
     the array.  allocCB and initCB are called for the elements in ascending
     order in the array, and freeCB and doneCB are called in descending
     order.  allocCB and initCB are mutually exclusive- they can't both be
     non-NULL. Same goes for freeCB and doneCB */
  void *(*allocCB)(void);  /* values of new elements set to return of this */
  void (*freeCB)(void *);  /* called on the values of invalidated elements */
  void (*initCB)(void *);  /* called on addresses of new elements */
  void (*doneCB)(void *);  /* called on addresses of invalidated elements */

} airArray;
extern airArray *airArrayNew(void **dataP, int *lenP, int unit, int incr);
extern void airArrayStructCB(airArray *a, 
			     void (*initCB)(void *), void (*doneCB)(void *));
extern void airArrayPointerCB(airArray *a, 
			      void *(*allocCB)(void), void (*freeCB)(void *));
extern int airArraySetLen(airArray *a, int newlen);
extern int airArrayIncrLen(airArray *a, int delta);
extern void airArrayNix(airArray *a);
extern void airArrayNuke(airArray *a);

/* endian.c */
extern int airStrToEndian(char *str);
extern char *airEndianToStr(int endian);
extern const int airMyEndian;

/* 754.c: IEEE-754 related stuff values */
typedef union {
  int i;
  float f;
} airFloat;
extern const int airMyQNaNHiBit;
extern float airFPGen_f(int cls);
extern int airFPClass_f(float val);
extern void airFPFprintf_f(FILE *file, float val);
extern const airFloat airFloatNaN;
extern const airFloat airFloatQNaN;
extern const airFloat airFloatSNaN;
extern const airFloat airFloatPosInf;
extern const airFloat airFloatNegInf;
extern float airNaN();
extern int airIsNaN(float f);
extern int airIsInf(float f);
extern int airExists(float f);

/* parse.c */
extern int airParseStrI(int *out, char *s, char *ct, int n);
extern int airParseStrF(float *out, char *s, char *ct, int n);
extern int airParseStrD(double *out, char *s, char *ct, int n);
extern int airParseStrC(char *out, char *s, char *ct, int n);
extern int airParseStrS(char **out, char *_s, char *ct, int n);

/* misc.c */
extern void *airFree(void *ptr);
extern char *airStrdup(char *s);
extern char *airDoneStr(float start, float here, float end, char *str);
extern double airTime();
extern int airLog2(float n);

/*******     the interminable sea of #define and macros     *******/

#define AIR_TRUE 1
#define AIR_FALSE 0

/*
******** AIR_ENDIAN, AIR_QNANHIBIT
**
** these reflect particulars of hardware which we're running on
*/
#define AIR_ENDIAN (airMyEndian)
#define AIR_QNANHIBIT (airMyQNaNHiBit)

/*
******** AIR_NAN, AIR_QNAN, AIR_SNAN, AIR_POS_INF, AIR_NEG_INF
**
** its nice to have these values available without the cost of a 
** function call.
**
** NOTE: AIR_POS_INF and AIR_NEG_INF correspond to the _unique_
** bit-patterns which signify positive and negative infinity.  With
** the NaNs, however, they are only one of many possible
** representations.
*/
#define AIR_NAN (airFloatNaN.f)
#define AIR_QNAN (airFloatQNaN.f)
#define AIR_SNAN (airFloatSNaN.f)
#define AIR_POS_INF (airFloatPosInf.f)
#define AIR_NEG_INF (airFloatNegInf.f)

/* 
******** AIR_EXISTS
**
** is non-zero (true) only for floats which are not NaN or +/-infinity
** 
** You'd think that (x == x) might work, but no no no, some optimizing
** compilers (SGI's cc) say "well of course they're equal, for all
** possible values".  Bastards.
**
** One of the benefits of IEEE 754 floating point numbers is that
** gradual underflow means that x = y <==> x - y = 0 for any (positive
** or negative) normalized or denormalized float.  Otherwise this
** macro could not be valid.
**
** However, there may be problems on machines which use extended
** registers- where the same initial value 1) directly read from the
** register, versus 2) saved to memory and loaded back, may end up
** being different.  I have to to to produce this or convince myself
** it can't happen.  If you have problems, then use the version of the
** macro which is a function call, and please email me: gk AT
** cs.utah.edu 
*/
#define AIR_EXISTS(x) (!((x) - (x)))
/* #define AIR_EXISTS(x) (airExists(x)) */

/*
******** AIR_MAX(a,b), AIR_MIN(a,b), AIR_ABS(a)
**
** the usual
*/
#define AIR_MAX(a,b) ((a) > (b) ? (a) : (b))
#define AIR_MIN(a,b) ((a) < (b) ? (a) : (b))
#define AIR_ABS(a) ((a) > 0 ? (a) : -(a))

/*
******** AIR_COMPARE(a,b)
**
** the sort of compare that qsort() wants
*/
#define AIR_COMPARE(a,b) ((a) < (b)     \
		          ? -1          \
                          : ((a) == (b) \
                             ? 0        \
                             : 1))

/*
******** AIR_INSIDE(a,b,c), AIR_OPINSIDE(a,b,c), AIR_BETWEEN(a,b,c)
**
** is true if the middle argument is in the closed/open interval
** defined by the first and third arguments
*/
#define AIR_INSIDE(a,b,c) ((a) <= (b) && (b) <= (c))
#define AIR_OPINSIDE(a,b,c) ((a) < (b) && (b) < (c))
#define AIR_BETWEEN(a,b,c) AIR_OPINSIDE(a,b,c)

/*
******** AIR_CLAMP(a,b,c)
**
** returns the middle argument, after being clamped to the closed
** interval defined by the first and third arguments
*/
#define AIR_CLAMP(a,b,c) ((b) < (a)        \
		 	   ? (a)           \
			   : ((b) > (c)    \
			      ? (c)        \
			      : (b)))

/*
******** AIR_MOD(i, N)
**
** returns that integer in [0, N-1] which is i plus a multiple of N It
** may be unfortunate that the expression (i)%(N) appears three times;
** this should be inlined.  Or perhaps the compiler's optimizations
** (common sub-expression elimination) will save us.
*/
#define AIR_MOD(i, N) ((i)%(N) >= 0 ? (i)%(N) : N + (i)%(N))

/*
******** AIR_AFFINE(i,x,I,o,O)
**
** given intervals [i,I], [o,O] and a value x which may or may not be
** inside [i,I], return the value y such that y stands in the same
** relationship to [o,O] that x does with [i,I].  Or:
**
**    y - o         x - i     
**   -------   =   -------
**    O - o         I - i
**
** It is the callers responsibility to make sure I-i and O-o are 
** both non-zero.  Strictly speaking, real problems arise only when
** when I-i is zero: division by zero generates either NaN or infinity
*/
#define AIR_AFFINE(i,x,I,o,O) ( \
((double)(O)-(o))*((double)(x)-(i)) / ((double)(I)-(i)) + (o))

/*
******** AIR_DELTA(i,x,I,o,O)
**
** given intervals [i,I] and [o,O], calculates the number y such that
** a change of x within [i,I] is proportional to a change of y within
** [o,O].  Or:
**
**      y             x     
**   -------   =   -------
**    O - o         I - i
**
** It is the callers responsibility to make sure I-i and O-o are 
** both non-zero
*/
#define AIR_DELTA(i,x,I,o,O) ( \
((double)(O)-(o))*((double)(x)) / ((double)(I)-(i)) )

/*
******** AIR_INDEX(i,x,I,L,t)
**
** READ CAREFULLY!!
**
** Utility for mapping a floating point x in given range [i,I] to the
** index of an array with L elements, AND SAVES THE INDEX INTO GIVEN
** VARIABLE t, WHICH MUST BE OF SOME INTEGER TYPE because this relies
** on the implicit cast of an assignment to truncate away the
** fractional part.  ALSO, t must be of a type large enough to hold
** ONE GREATER than L.  So you can't pass a variable of type unsigned
** char if L is 256
**
** DOES NOT DO BOUNDS CHECKING: given an x which is not inside [i,I],
** this may produce an index not inside [0,L-1].  The mapping is
** accomplished by dividing the range from i to I into L intervals,
** all but the last of which is half-open; the last one is closed.
** For example, the number line from 0 to 3 would be divided as
** follows for a call with i = 0, I = 4, L = 4:
**
** index:       0    1    2    3 = L-1
** intervals: [   )[   )[   )[    ]
**            |----|----|----|----|
** value:     0    1    2    3    4
**
** The main point of the diagram above is to show how I made the
** arbitrary decision to orient the half-open interval, and which
** end has the closed interval.
**
** Note that AIR_INDEX(0,3,4,4,t) and AIR_INDEX(0,4,4,4,t) both set t = 3
**
** The reason that this macro requires a argument for saving the
** result is that this is the easiest way to avoid extra conditionals.
** Otherwise, we'd have to do some check to see if x is close enough
** to I so that the generated index would be L and not L-1.  "Close
** enough" because due to precision problems you can have an x < I
** such that (x-i)/(I-i) == 1, which was a bug with the previous version
** of this macro.  It is far simpler to just do the index generation
** and then do the sneaky check to see if the index is too large by 1.
** We are relying on the fact that C _defines_ boolean true to be exactly 1.
**
** Note also that we are never explicity casting to one kind of int or
** another-- the given t can be any integral type, including long long 
*/
#define AIR_INDEX(i,x,I,L,t) ( \
(t) = (L) * ((double)(x)-(i)) / ((double)(I)-(i)), \
(t) -= ((t) == (L)) )

/*
******** AIR_ROUNDUP, AIR_ROUNDDOWN
**
** rounds integers up or down; just wrappers around floor and ceil
*/
#define AIR_ROUNDUP(x)   ((int)(floor((x)+0.5)))
#define AIR_ROUNDDOWN(x) ((int)(ceil((x)-0.5)))

/*
******** AIR_MEMCPY1, AIR_MEMCPY2, AIR_MEMCPY4, AIR_MEMCPY
**
** offer a speed-up over memcpy() by eliminating calls to in on very
** small lengths of memory.  AIR_MEMCPY{1,2,4} will (like memcpy)
** return the first argument, but unfortunately AIR_MEMCPY does not.  
*/
#define AIR_MEMCPY1(a, b) (*((char*)(a))  = *((char*)(b)))
#define AIR_MEMCPY2(a, b) (*((short*)(a)) = *((short*)(b)))
#define AIR_MEMCPY4(a, b) (*((int*)(a))   = *((int*)(b)))
  /* do we really want to try long long int for 8-byte copies? */
#define AIR_MEMCPY(a, b, sz)                  \
switch (sz) {                                 \
  case 1: AIR_MEMCPY1(a, b); break;           \
  case 2: AIR_MEMCPY2(a, b); break;           \
  case 4: AIR_MEMCPY4(a, b); break;           \
  default: memcpy((a), (b), (sz)); break;     \
}


#ifdef __cplusplus
}
#endif
#endif /* AIR_HAS_BEEN_INCLUDED */
