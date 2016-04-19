 /*------------------------------------------------------------------------*/
 /*  VectorMath.h                                                          */
 /*     Joe Kniss                                                          */
 /*   GLUV test bed for Volume rendering                                   */
 /*   Inline functions for vector mathematical manipulation man            */
 /*   Mass props to Milan Ikits & Gordon Kindlmann for some of these       */
 /*    THIS MUST BE MADE "C -SAFE" !!!!!!!!!!!!!!!!!!!!!!!!!               */ 
 /*                                                                        */
 /*                   ________    ____   ___                               */ 
 /*                  |        \  /    | /  /                               */
 /*                  +---+     \/     |/  /                                */
 /*                  +---|  |\    /|     <                                 */
 /*                  |  ||  | \  / |  |\  \                                */
 /*                  |      |  \/  |  | \  \                               */
 /*                   \_____|      |__|  \__\                              */
 /*                       Copyright  2001                                  */
 /*                      Joe Michael Kniss                                 */
 /*------------------------------------------------------------------------*/

#ifndef __VectorMath_defined
#define __VectorMath_defined

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <iostream.h>
#include <limits.h>
#define EPSILON 2e-07
#define V_PI    3.1415926535897932384626433832795

#if 0 //matrix representation for mat[16], same as OpenGl matrix rep
  
  +-          -+
  |0  4  8   12|
  |1  5  9   13|
  |2  6  10  14|
  |3  7  11  15|
  +-          -+

#endif

/*
******** NRRD_AFFINE(i,x,I,o,O)
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
** both greater than zero
*/

#define CLAMP(x) ((((x)>0) ? (((x)<1) ? x : 1) : 0))
#define MAX(x,y) (((x)>(y)) ? (x) : (y))
#define MIN(x,y) (((x)<(y)) ? (x) : (y))
#define ABS(x) ((x)<0 ? (-x) : (x)) 

inline double CLAMP_ARB(const double c, const double x, const double C)
{
   return ((((x)>c) ? (((x)<(C)) ? x : (C)) : c));
}

inline double affine(const double i, const double x, const double I, 
		     const double o, const double O)
{
   return  ( ((O)-(o))*((x)-(i)) / ((I)-(i)) + (o) );
}

// make a vector all zeros -----------------------------------------
inline void zeroV3(GLfloat in[3])
{
  in[0] = in[1] = in[2] = 0;
}

inline void zeroV3(GLdouble in[3])
{
  in[0] = in[1] = in[2] = 0;
}

// negate all components of a vector -------------------------------
inline void negateV3(GLfloat in[3])
{
  in[0] = -in[0];
  in[1] = -in[1];
  in[2] = -in[2];
}

inline void negateV3(GLdouble in[3])
{
  in[0] = -in[0];
  in[1] = -in[1];
  in[2] = -in[2];
}

// copy vector 'out' = 'in';---------------------------------------
inline void copyV3(GLfloat out[3], GLfloat in[3])
{
   out[0] = in[0];
   out[1] = in[1];
   out[2] = in[2];
}

inline void copyV3(GLdouble out[3], GLdouble in[3])
{
   out[0] = in[0];
   out[1] = in[1];
   out[2] = in[2];
}

inline void copyV3(GLfloat out[3], GLdouble in[3])
{
   out[0] = (float)in[0];
   out[1] = (float)in[1];
   out[2] = (float)in[2];
}

inline void copyV3(GLdouble out[3], GLfloat in[3])
{
   out[0] = in[0];
   out[1] = in[1];
   out[2] = in[2];
}

// out = inl - inr -----------------------------------------------
inline void subV3(GLfloat out[3], GLfloat inl[3], GLfloat inr[3])
{
   out[0] = inl[0] - inr[0];
   out[1] = inl[1] - inr[1];
   out[2] = inl[2] - inr[2];
}

inline void subV3(GLdouble out[3], GLdouble inl[3], GLdouble inr[3])
{
   out[0] = inl[0] - inr[0];
   out[1] = inl[1] - inr[1];
   out[2] = inl[2] - inr[2];
}

// out = inl + inr ---------------------------------------------
inline void addV3(GLfloat out[3], GLfloat inl[3], GLfloat inr[3])
{
   out[0] = inl[0] + inr[0];
   out[1] = inl[1] + inr[1];
   out[2] = inl[2] + inr[2];
}

inline void addV3(GLdouble out[3], GLdouble inl[3], GLdouble inr[3])
{
   out[0] = inl[0] + inr[0];
   out[1] = inl[1] + inr[1];
   out[2] = inl[2] + inr[2];
}

// one *= s --------------------------------------------------
inline void scaleV3(float s, GLfloat one[3])
{
   one[0] *= s;
   one[1] *= s;
   one[2] *= s;
}

inline void scaleV3(double s, GLdouble one[3])
{
   one[0] *= s;
   one[1] *= s;
   one[2] *= s;
}

// out = in * s --------------------------------------------
inline void cscaleV3(GLfloat out[3], float s, GLfloat in[3])
{
  out[0] = s*in[0];
  out[1] = s*in[1];
  out[2] = s*in[2];
}

inline void cscaleV3(GLdouble out[3], double s, GLdouble in[3])
{
  out[0] = s*in[0];
  out[1] = s*in[1];
  out[2] = s*in[2];
}

// set a matrix 'm' to the identity -----------------------
inline void identityMatrix(GLfloat m[16])
{
   m[0]=1; m[4]=0; m[8]= 0; m[12]=0;
   m[1]=0; m[5]=1; m[9]= 0; m[13]=0;
   m[2]=0; m[6]=0; m[10]=1; m[14]=0;
   m[3]=0; m[7]=0; m[11]=0; m[15]=1;
}

inline void identityMatrix(GLdouble m[16])
{
   m[0]=1; m[4]=0; m[8]= 0; m[12]=0;
   m[1]=0; m[5]=1; m[9]= 0; m[13]=0;
   m[2]=0; m[6]=0; m[10]=1; m[14]=0;
   m[3]=0; m[7]=0; m[11]=0; m[15]=1;
}

// vector matrix multiplication [4] vector ----------------------
// out = mat * in
inline void translateV4(GLfloat out[4], GLfloat mat[16], GLfloat in[4])
{
   out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2] + mat[12]*in[3];
   out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2] + mat[13]*in[3];
   out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2] + mat[14]*in[3];
   out[3] = mat[3]*in[0] + mat[7]*in[1] + mat[11]*in[2] + mat[15]*in[3];
}

//3 vector with implict w=1 mult matrix to 4 vector
inline void translateV4_3W(GLfloat out[4], GLfloat mat[16], GLfloat in[3])
{
   out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2] + mat[12];
   out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2] + mat[13];
   out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2] + mat[14];
   out[3] = mat[3]*in[0] + mat[7]*in[1] + mat[11]*in[2] + mat[15];
}

inline void translateV4(GLdouble out[4], GLdouble mat[16], GLdouble in[4])
{
   out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2] + mat[12]*in[3];
   out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2] + mat[13]*in[3];
   out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2] + mat[14]*in[3];
   out[3] = mat[3]*in[0] + mat[7]*in[1] + mat[11]*in[2] + mat[15]*in[3];
}

// vector matrix multiplicaiton [3] vector with no translation ---
// (only rotation) out = mat * in;
inline void translateV3(GLfloat out[3], GLfloat mat[16], GLfloat in[3])
{
   out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2];// + mat[12];
   out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2];// + mat[13];
   out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2];// + mat[14];
}

inline void translateV3(GLdouble out[3], GLdouble mat[16], GLdouble in[3])
{
   out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2];// + mat[12];
   out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2];// + mat[13];
   out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2];// + mat[14];
}

inline void translateV3(GLfloat out[3], GLdouble mat[16], GLfloat in[3])
{
   out[0] = (float)(mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2]);// + mat[12];
   out[1] = (float)(mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2]);// + mat[13];
   out[2] = (float)(mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2]);// + mat[14];
}

// [3] vector * matrix --------------------------------------------
// out = mat * in (with translation)
inline void translateV3W(GLfloat out[3], GLfloat mat[16], GLfloat in[3])
{
   out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2] + mat[12];
   out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2] + mat[13];
   out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2] + mat[14];
}

inline void translateV3W(GLfloat out[3], GLdouble mat[16], GLfloat in[3])
{
   out[0] = (float)(mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2] + mat[12]);
   out[1] = (float)(mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2] + mat[13]);
   out[2] = (float)(mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2] + mat[14]);
}

inline void translateV3W(GLdouble out[3], GLdouble mat[16], GLdouble in[3])
{
   out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2] + mat[12];
   out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2] + mat[13];
   out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2] + mat[14];
}

//transformation of 3 vector with implicit w=1 and homoginization
inline void translateV3WD(GLfloat out[3], GLfloat mat[16], GLfloat in[3])
{
	out[0] = (float)(mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2] + mat[12]);
  out[1] = (float)(mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2] + mat[13]);
  out[2] = (float)(mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2] + mat[14]);
	float w= (float)(mat[3]*in[0] + mat[7]*in[1] + mat[11]*in[2] + mat[15]);
	out[0]/=w;
	out[1]/=w;
	out[2]/=w;
}

// legacy call
inline void transMatrixV3(GLfloat out[3], GLfloat mat[16], GLfloat in[3])
{

   out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2] + mat[12];
   out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2] + mat[13];
   out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2] + mat[14];
}

// dot product of two [4] vecotrs --------------------------
inline GLfloat dotV4(GLfloat one[4], GLfloat two[4])
{
   return one[0]*two[0] + one[1]*two[1] + one[2]*two[2] + one[3]*two[3];
}

inline GLdouble dotV4(GLdouble one[4], GLdouble two[4])
{
   return one[0]*two[0] + one[1]*two[1] + one[2]*two[2] + one[3]*two[3];
}

// dot product of two [3] vectors ------------------------
inline GLfloat dotV3(GLfloat one[4], GLfloat two[4])
{
   return one[0]*two[0] + one[1]*two[1] + one[2]*two[2];
}

inline GLdouble dotV3(GLdouble one[4], GLdouble two[4])
{
   return one[0]*two[0] + one[1]*two[1] + one[2]*two[2];
}

// compute the length of a [3] vector --------------------
inline GLfloat normV3(GLfloat one[3])
{
   return (GLfloat)sqrt( one[0]*one[0] + one[1]*one[1] + one[2]*one[2]);
} 

inline GLdouble normV3(GLdouble one[3])
{
   return sqrt( one[0]*one[0] + one[1]*one[1] + one[2]*one[2]);
} 

// normalize a [4] vector --------------------------------
inline void normalizeV4(GLfloat v[4])
{
   GLfloat len = (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3]);
   if(len > 0){
      v[0] /= len;
      v[1] /= len;
      v[2] /= len;
      v[3] /= len;
   }
}

inline void normalizeV4(GLdouble v[4])
{
   GLfloat len = (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3]);
   if(len > 0){
      v[0] /= len;
      v[1] /= len;
      v[2] /= len;
      v[3] /= len;
   }
}

// normalize a [3] vector ---------------------------------
inline void normalizeV3(GLfloat v[3])
{
   GLfloat len = (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]); 
   if(len > 0){
      v[0] /= len;
      v[1] /= len;
      v[2] /= len;
   }
}

inline void normalizeV3(GLdouble v[3])
{
   GLfloat len = (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]); 
   if(len > 0){
      v[0] /= len;
      v[1] /= len;
      v[2] /= len;
   }
}

// divide out the 'W' part of a [4] vector ----------------
inline void homogV4(GLfloat v[4])
{
   v[0] /= v[3];
   v[1] /= v[3];
   v[2] /= v[3];
   v[3] = 1;
}

inline void homogV4(GLdouble v[4])
{
   v[0] /= v[3];
   v[1] /= v[3];
   v[2] /= v[3];
   v[3] = 1;
}

// compute cross product of [3] vector -------------------
// out = one X two;
inline void crossV3(GLfloat out[3], GLfloat one[3], GLfloat two[3])
{
   out[0] = one[1]*two[2] - one[2]*two[1];
   out[1] = one[2]*two[0] - one[0]*two[2];
   out[3] = one[0]*two[1] - one[1]*two[0];
}

inline void crossV3(GLdouble out[3], GLdouble one[3], GLdouble two[3])
{
   out[0] = one[1]*two[2] - one[2]*two[1];
   out[1] = one[2]*two[0] - one[0]*two[2];
   out[3] = one[0]*two[1] - one[1]*two[0];
}

// copy two matricies -----------------------------------
inline void matrixEqual(GLfloat me[16], GLfloat m[16])
{
   for(int i=0; i< 16; ++i) me[i] = m[i];
}

inline void matrixEqual(GLdouble me[16], GLdouble m[16])
{
   for(int i=0; i< 16; ++i) me[i] = m[i];
}

// maxb = ma * mb --------------------------------------
inline void matrixMult( GLfloat maxb[16], GLfloat ma[16], GLfloat mb[16] )
{
  maxb[0] = ma[0]*mb[0] + ma[4]*mb[1] + ma[8]*mb[2] + ma[12]*mb[3];
  maxb[1] = ma[1]*mb[0] + ma[5]*mb[1] + ma[9]*mb[2] + ma[13]*mb[3];
  maxb[2] = ma[2]*mb[0] + ma[6]*mb[1] + ma[10]*mb[2] + ma[14]*mb[3];
  maxb[3] = ma[3]*mb[0] + ma[7]*mb[1] + ma[11]*mb[2] + ma[15]*mb[3];
  
  maxb[4] = ma[0]*mb[4] + ma[4]*mb[5] + ma[8]*mb[6] + ma[12]*mb[7];
  maxb[5] = ma[1]*mb[4] + ma[5]*mb[5] + ma[9]*mb[6] + ma[13]*mb[7];
  maxb[6] = ma[2]*mb[4] + ma[6]*mb[5] + ma[10]*mb[6] + ma[14]*mb[7];
  maxb[7] = ma[3]*mb[4] + ma[7]*mb[5] + ma[11]*mb[6] + ma[15]*mb[7];
  
  maxb[8] = ma[0]*mb[8] + ma[4]*mb[9] + ma[8]*mb[10] + ma[12]*mb[11];
  maxb[9] = ma[1]*mb[8] + ma[5]*mb[9] + ma[9]*mb[10] + ma[13]*mb[11];
  maxb[10] = ma[2]*mb[8] + ma[6]*mb[9] + ma[10]*mb[10] + ma[14]*mb[11];
  maxb[11] = ma[3]*mb[8] + ma[7]*mb[9] + ma[11]*mb[10] + ma[15]*mb[11];
  
  maxb[12] = ma[0]*mb[12] + ma[4]*mb[13] + ma[8]*mb[14] + ma[12]*mb[15];
  maxb[13] = ma[1]*mb[12] + ma[5]*mb[13] + ma[9]*mb[14] + ma[13]*mb[15];
  maxb[14] = ma[2]*mb[12] + ma[6]*mb[13] + ma[10]*mb[14] + ma[14]*mb[15];
  maxb[15] = ma[3]*mb[12] + ma[7]*mb[13] + ma[11]*mb[14] + ma[15]*mb[15];
}

inline void matrixMult( GLdouble maxb[16], GLdouble ma[16], GLdouble mb[16] )
{
  maxb[0] = ma[0]*mb[0] + ma[4]*mb[1] + ma[8]*mb[2] + ma[12]*mb[3];
  maxb[1] = ma[1]*mb[0] + ma[5]*mb[1] + ma[9]*mb[2] + ma[13]*mb[3];
  maxb[2] = ma[2]*mb[0] + ma[6]*mb[1] + ma[10]*mb[2] + ma[14]*mb[3];
  maxb[3] = ma[3]*mb[0] + ma[7]*mb[1] + ma[11]*mb[2] + ma[15]*mb[3];
  
  maxb[4] = ma[0]*mb[4] + ma[4]*mb[5] + ma[8]*mb[6] + ma[12]*mb[7];
  maxb[5] = ma[1]*mb[4] + ma[5]*mb[5] + ma[9]*mb[6] + ma[13]*mb[7];
  maxb[6] = ma[2]*mb[4] + ma[6]*mb[5] + ma[10]*mb[6] + ma[14]*mb[7];
  maxb[7] = ma[3]*mb[4] + ma[7]*mb[5] + ma[11]*mb[6] + ma[15]*mb[7];
  
  maxb[8] = ma[0]*mb[8] + ma[4]*mb[9] + ma[8]*mb[10] + ma[12]*mb[11];
  maxb[9] = ma[1]*mb[8] + ma[5]*mb[9] + ma[9]*mb[10] + ma[13]*mb[11];
  maxb[10] = ma[2]*mb[8] + ma[6]*mb[9] + ma[10]*mb[10] + ma[14]*mb[11];
  maxb[11] = ma[3]*mb[8] + ma[7]*mb[9] + ma[11]*mb[10] + ma[15]*mb[11];
  
  maxb[12] = ma[0]*mb[12] + ma[4]*mb[13] + ma[8]*mb[14] + ma[12]*mb[15];
  maxb[13] = ma[1]*mb[12] + ma[5]*mb[13] + ma[9]*mb[14] + ma[13]*mb[15];
  maxb[14] = ma[2]*mb[12] + ma[6]*mb[13] + ma[10]*mb[14] + ma[14]*mb[15];
  maxb[15] = ma[3]*mb[12] + ma[7]*mb[13] + ma[11]*mb[14] + ma[15]*mb[15];
}

// compute the inverse of a matrix
// invm = m^(-1)
inline void inverseMatrix( GLfloat invm[16], GLfloat m[16] )
{
  GLfloat det = 
    m[0]*m[5]*m[10]-
    m[0]*m[6]*m[9]-
    m[1]*m[4]*m[10]+
    m[1]*m[6]*m[8]+
    m[2]*m[4]*m[9]-
    m[2]*m[5]*m[8];

  invm[0] = (m[5]*m[10]-m[6]*m[9])/det;
  invm[1] = (-m[1]*m[10]+m[2]*m[9])/det;
  invm[2] = (m[1]*m[6]-m[2]*m[5])/det;
  invm[3] = 0.0;

  invm[4] = (-m[4]*m[10]+m[6]*m[8])/det;
  invm[5] = (m[0]*m[10]-m[2]*m[8])/det;
  invm[6] = (-m[0]*m[6]+m[2]*m[4])/det;
  invm[7] = 0.0;

  invm[8] = (m[4]*m[9]-m[5]*m[8])/det;
  invm[9] = (-m[0]*m[9]+m[1]*m[8])/det;
  invm[10] = (m[0]*m[5]-m[1]*m[4])/det;
  invm[11] = 0.0;
  
  invm[12] = (-m[4]*m[9]*m[14]+m[4]*m[13]*m[10]+
	      m[5]*m[8]*m[14]-m[5]*m[12]*m[10]-
	      m[6]*m[8]*m[13]+m[6]*m[12]*m[9])/det;
  invm[13] = (m[0]*m[9]*m[14]-m[0]*m[13]*m[10]-
	      m[1]*m[8]*m[14]+m[1]*m[12]*m[10]+
	      m[2]*m[8]*m[13]-m[2]*m[12]*m[9])/det;
  invm[14] = (-m[0]*m[5]*m[14]+m[0]*m[13]*m[6]+
	      m[1]*m[4]*m[14]-m[1]*m[12]*m[6]-
	      m[2]*m[4]*m[13]+m[2]*m[12]*m[5])/det;
  invm[15] = 1.0;
}

inline void inverseMatrix( GLdouble invm[16], GLdouble m[16] )
{
  GLfloat det = (float)( 
    m[0]*m[5]*m[10]-
    m[0]*m[6]*m[9]-
    m[1]*m[4]*m[10]+
    m[1]*m[6]*m[8]+
    m[2]*m[4]*m[9]-
    m[2]*m[5]*m[8]);

  invm[0] = (m[5]*m[10]-m[6]*m[9])/det;
  invm[1] = (-m[1]*m[10]+m[2]*m[9])/det;
  invm[2] = (m[1]*m[6]-m[2]*m[5])/det;
  invm[3] = 0.0;

  invm[4] = (-m[4]*m[10]+m[6]*m[8])/det;
  invm[5] = (m[0]*m[10]-m[2]*m[8])/det;
  invm[6] = (-m[0]*m[6]+m[2]*m[4])/det;
  invm[7] = 0.0;

  invm[8] = (m[4]*m[9]-m[5]*m[8])/det;
  invm[9] = (-m[0]*m[9]+m[1]*m[8])/det;
  invm[10] = (m[0]*m[5]-m[1]*m[4])/det;
  invm[11] = 0.0;
  
  invm[12] = (-m[4]*m[9]*m[14]+m[4]*m[13]*m[10]+
	      m[5]*m[8]*m[14]-m[5]*m[12]*m[10]-
	      m[6]*m[8]*m[13]+m[6]*m[12]*m[9])/det;
  invm[13] = (m[0]*m[9]*m[14]-m[0]*m[13]*m[10]-
	      m[1]*m[8]*m[14]+m[1]*m[12]*m[10]+
	      m[2]*m[8]*m[13]-m[2]*m[12]*m[9])/det;
  invm[14] = (-m[0]*m[5]*m[14]+m[0]*m[13]*m[6]+
	      m[1]*m[4]*m[14]-m[1]*m[12]*m[6]-
	      m[2]*m[4]*m[13]+m[2]*m[12]*m[5])/det;
  invm[15] = 1.0;
}

//transpose a matrix
inline void transposeMatrix(GLfloat m[16])
{
  GLfloat tmp;
  tmp = m[1];
  m[1] = m[4];
  m[4] = tmp;
  tmp = m[2];
  m[2] = m[8];
  m[8] = tmp;
  tmp = m[3];
  m[3] = m[12];
  m[12] = tmp;
  tmp = m[6];
  m[6] = m[9];
  m[9] = tmp;
  tmp = m[7];
  m[7] = m[13];
  m[13] = tmp;
  tmp = m[11];
  m[11] = m[14];
  m[14] = tmp;

}

inline void transposeMatrix(GLdouble m[16])
{
  GLdouble tmp;
  tmp = m[1];
  m[1] = m[4];
  m[4] = tmp;
  tmp = m[2];
  m[2] = m[8];
  m[8] = tmp;
  tmp = m[3];
  m[3] = m[12];
  m[12] = tmp;
  tmp = m[6];
  m[6] = m[9];
  m[9] = tmp;
  tmp = m[7];
  m[7] = m[13];
  m[13] = tmp;
  tmp = m[11];
  m[11] = m[14];
  m[14] = tmp;

}

//angle and axis to a rotation matrix ----------------------------
inline void axis2Rot( GLfloat m[16], GLfloat k[3], GLfloat theta )
{
  float c = (float)cos(theta);
  float s = (float)sin(theta);
  float v = 1 - c;

  m[0] = k[0]*k[0]*v + c;
  m[4] = k[0]*k[1]*v - k[2]*s;
  m[8] = k[0]*k[2]*v + k[1]*s;

  m[1] = k[0]*k[1]*v + k[2]*s;
  m[5] = k[1]*k[1]*v + c;
  m[9] = k[1]*k[2]*v - k[0]*s;

  m[2] = k[0]*k[2]*v - k[1]*s;
  m[6] = k[1]*k[2]*v + k[0]*s;
  m[10] = k[2]*k[2]*v + c;
}

inline void axis2Rot( GLdouble m[16], GLdouble k[3], GLdouble theta )
{
  float c = (float)cos(theta);
  float s = (float)sin(theta);
  float v = 1 - c;

  m[0] = k[0]*k[0]*v + c;
  m[4] = k[0]*k[1]*v - k[2]*s;
  m[8] = k[0]*k[2]*v + k[1]*s;

  m[1] = k[0]*k[1]*v + k[2]*s;
  m[5] = k[1]*k[1]*v + c;
  m[9] = k[1]*k[2]*v - k[0]*s;

  m[2] = k[0]*k[2]*v - k[1]*s;
  m[6] = k[1]*k[2]*v + k[0]*s;
  m[10] = k[2]*k[2]*v + c;
}

// Inverse angle-axis formula.-----------------------------------

inline void rot2Axis( GLfloat k[3], GLfloat *theta, GLfloat m[16] )
{
  GLfloat c = (float)(0.5 * (m[0] + m[5] + m[10] - 1.0));
  GLfloat r1 = m[6] - m[9];
  GLfloat r2 = m[8] - m[2];
  GLfloat r3 = m[1] - m[4];
  GLfloat s = (float)(0.5 * sqrt(r1*r1+r2*r2+r3*r3));

  *theta = (float)atan2(s, c);

  if( fabs(s) > EPSILON )
    {
      c = (float)(2.0*s);

      k[0] = r1 / c;
      k[1] = r2 / c;
      k[2] = r3 / c;
    }
  else
    {
      if( c > 0 ) // theta = 0
	{
	  k[0] = 0;
	  k[1] = 0;
	  k[2] = 1;
	}
      else // theta = +-pi: Shepperd procedure
	{
	  GLfloat k0_2 = (m[0] + 1)/2;
	  GLfloat k1_2 = (m[5] + 1)/2;
	  GLfloat k2_2 = (m[10] + 1)/2;

	  if( k0_2 > k1_2 )
	    {
	      if( k0_2 > k2_2 ) // k0 biggest
		{
		  k[0] = (float)sqrt(k1_2);
		  k[1] = (m[1] + m[4])/(4*k[0]);
		  k[2] = (m[2] + m[8])/(4*k[0]);
		}
	      else // k2 biggest
		{
		  k[2] = (float)sqrt(k2_2);
		  k[0] = (m[2] + m[8])/(4*k[2]);
		  k[1] = (m[6] + m[9])/(4*k[2]);
		}
	    }
	  else
	    {
	      if( k1_2 > k2_2 ) // k1 biggest
		{
		  k[1] = (float)sqrt(k1_2);
		  k[0] = (m[1] + m[4])/(4*k[1]);
		  k[2] = (m[6] + m[9])/(4*k[1]);
		}
	      else // k2 biggest
		{
		  k[2] = (float)sqrt(k2_2);
		  k[0] = (m[2] + m[8])/(4*k[2]);
		  k[1] = (m[6] + m[9])/(4*k[2]);
		}
	    }
	}
    }
}

//quaternion to rotation matrix

inline void q2R( GLfloat m[16], GLfloat q[4] )
{
  m[0] = q[0]*q[0] + q[1]*q[1] - q[2]*q[2] - q[3]*q[3];
  m[1] = 2*q[1]*q[2] + 2*q[0]*q[3];
  m[2] = 2*q[1]*q[3] - 2*q[0]*q[2];

  m[4] = 2*q[1]*q[2] - 2*q[0]*q[3];
  m[5] = q[0]*q[0] + q[2]*q[2] - q[1]*q[1] - q[3]*q[3];
  m[6] = 2*q[2]*q[3] + 2*q[0]*q[1];

  m[8] = 2*q[1]*q[3] + 2*q[0]*q[2];
  m[9] = 2*q[2]*q[3] - 2*q[0]*q[1];
  m[10]= q[0]*q[0] + q[3]*q[3] - q[1]*q[1] - q[2]*q[2];
}

//angle axis to quaternion

inline void axis2q(GLfloat q[4], float theta, float a[3])
{
   normalizeV3(a);
   copyV3(q, a);
   scaleV3((float)sin(theta/2.0), q);
   q[3] = (float)cos(theta/2.0);
}

//normalize a quaternion

inline void normalizeQ(GLfloat q[4])
{
   float  mag = (q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
   for (int i = 0; i < 4; i++) q[i] /= mag;
}

/*
 * Prints matrix to stderr. 
 */

inline void printMatrix( GLfloat m[16] )
{
  int i, j;

  for( i=0; i<4; i++ )
    {
      for( j=0; j<4; j++ )
				fprintf(stderr, "%f ", m[i+j*4]);
      fprintf(stderr, "\n");
    }
  fprintf(stderr, "\n");
}
inline void printMatrix( GLdouble m[16] )
{
  int i, j;

  for( i=0; i<4; i++ )
    {
      for( j=0; j<4; j++ )
	fprintf(stderr, "%f ", m[i+j*4]);
      fprintf(stderr, "\n");
    }
  fprintf(stderr, "\n");
}

inline void printV3(double v[3])
{
	cerr << " " << v[0] << "," << v[1] << "," << v[2];
}

inline void printV3(float v[3])
{
	cerr << " " << v[0] << "," << v[1] << "," << v[2];
}

inline void printV4(float v[3])
{
	cerr << " " << v[0] << "," << v[1] << "," << v[2] << "," << v[3];
}

inline void buildLookAt(GLfloat m[16], 
			GLfloat eye[3], GLfloat at[3], GLfloat up[3])
{
   //I am not 100% certain that my cross products are in the right order

   GLfloat tmp1[3], tmp2[3], tmp3[3];
   
   subV3(tmp1, eye, at);
   GLfloat norm;
   norm = normV3(tmp1);
   tmp1[0] /=norm;
   tmp1[1] /=norm;
   tmp1[2] /=norm;

   m[2]     =tmp1[0];
   m[6]     =tmp1[1];
   m[10]    =tmp1[2];
   
   crossV3(tmp2, up, tmp1);
   norm = normV3(tmp2);
   tmp2[0] /=norm;
   tmp2[1] /=norm;
   tmp2[2] /=norm;

   m[0]     =tmp2[0];
   m[4]     =tmp2[1];
   m[8]     =tmp2[2];
   
   crossV3(tmp3, tmp1, tmp2);
   norm = normV3(tmp3);
   tmp3[0] /=norm;
   tmp3[1] /=norm;
   tmp3[2] /=norm;

   m[1]     =tmp3[0];
   m[5]     =tmp3[1];
   m[9]     =tmp3[2];

   m[12]= -eye[0];
   m[13]= -eye[1];
   m[14]= -eye[2];
   m[15]= 1;

   m[3]  = 0;
   m[7]  = 0;
   m[11] = 0;
}


//======================== calculus ===================================

//standard scalar derivative measure
inline derivative3D(unsigned char *magV1, 
										float *gradV3, 
										const int sx, const int sy, const int sz, 
										const unsigned char *dat)
{
	float *tmp = new float[sx*sy*sz];
	float maxm = 0;
	for(int i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				if((k<2)||(k>sx-3)||(j<2)||(j>sy-3)||(i<2)||(i>sz-3)){
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = 0;
					tmp[i*sx*sy + j*sx + k] = 0;	
				}
				else {
					int dx = dat[i*sx*sy + j*sx + (k+1)] - dat[i*sx*sy + j*sx + (k-1)];
					int dy = dat[i*sx*sy + (j+1)*sx + k] - dat[i*sx*sy + (j-1)*sx + k];
					int dz = dat[(i+1)*sx*sy + j*sx + k] - dat[(i-1)*sx*sy + j*sx + k];
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = (float)dx;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = (float)dy;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = (float)dz;
					tmp[i*sx*sy + j*sx + k] = (float)sqrt(dx*dx + dy*dy + dz*dz);
					maxm = maxm < tmp[i*sx*sy + j*sx + k] ? tmp[i*sx*sy + j*sx + k] : maxm;
				}
			}
		}
	}
	for( i = 0; i < sz; ++i){
		for(int j = 0; j < sy; ++j){
			for(int k = 0; k < sx; ++k){
				magV1[i*sx*sy + j*sx + k] = (unsigned char)((tmp[i*sx*sy + j*sx + k]/maxm)*255);
			}
		}
	}
	delete[] tmp;
}


//standard scalar derivative measure - vgh volume (value grad hessian)
inline derivative3DVGH(float *gradV3, 
										   const int sx, const int sy, const int sz, 
										   const unsigned char *dat)
{
	//cerr << "hi!!!!!!!!!! " << sz << " " << sy << " " << sx << endl;
	for(int i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				if((k<1)||(k>sx-2)||(j<1)||(j>sy-2)||(i<1)||(i>sz-2)){
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = 0;
				}
				else {
					int dx = dat[i*sx*sy*3     + j*sx*3     +(k+1)*3] - dat[i*sx*sy*3     + j*sx*3     + (k-1)*3];
					int dy = dat[i*sx*sy*3     + (j+1)*sx*3 + k*3]    - dat[i*sx*sy*3     + (j-1)*sx*3 + k*3];
					int dz = dat[(i+1)*sx*sy*3 + j*sx*3     + k*3]    - dat[(i-1)*sx*sy*3 + j*sx*3     + k*3];
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = (float)dx;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = (float)dy;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = (float)dz;
				}
			}
		}
	}
}

inline void addDer(unsigned char *magV1, float *gradV3, 
									 const int sx, const int sy, const int sz,
									 const unsigned char *dat1, const unsigned char *dat2)
{
	float *tmp = new float[sx*sy*sz];
	float maxm = 0;
	for(int i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				if((k<2)||(k>sx-3)||(j<2)||(j>sy-3)||(i<2)||(i>sz-3)){
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = 0;
					tmp[i*sx*sy + j*sx + k] = 0;	
				}
				else {
					int dx1 = dat1[i*sx*sy + j*sx + (k+1)] - dat1[i*sx*sy + j*sx + (k-1)];
					int dy1 = dat1[i*sx*sy + (j+1)*sx + k] - dat1[i*sx*sy + (j-1)*sx + k];
					int dz1 = dat1[(i+1)*sx*sy + j*sx + k] - dat1[(i-1)*sx*sy + j*sx + k];

					int dx2 = dat2[i*sx*sy + j*sx + (k+1)] - dat2[i*sx*sy + j*sx + (k-1)];
					int dy2 = dat2[i*sx*sy + (j+1)*sx + k] - dat2[i*sx*sy + (j-1)*sx + k];
					int dz2 = dat2[(i+1)*sx*sy + j*sx + k] - dat2[(i-1)*sx*sy + j*sx + k];
					
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = (float)(dx1 + dx2);
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = (float)(dy1 + dy2);
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = (float)(dz1 + dz2);
					tmp[i*sx*sy + j*sx + k] = (float)sqrt((dx1+dx2)*(dx1+dx2) + (dy1+dy2)*(dy1+dy2) + (dz1+dz2)*(dz1+dz2));
					maxm = maxm < tmp[i*sx*sy + j*sx + k] ? tmp[i*sx*sy + j*sx + k] : maxm;
				}
			}
		}
	}
	for( i = 0; i < sz; ++i){
		for(int j = 0; j < sy; ++j){
			for(int k = 0; k < sx; ++k){
				magV1[i*sx*sy + j*sx + k] = (unsigned char)((tmp[i*sx*sy + j*sx + k]/maxm)*255);
			}
		}
	}
	delete[] tmp;
}

template<class T>                     //compute an additive gradient
inline
int AGradArb(float *gradV3,           //put it in here, create your data first
						 int sx, int sy, int sz,  //size of each axis
						 int n,                   //first n elements of data to compute grad with
						 int elts,                //number of elements in the data
						 T *data)                 //the data
{
#if 0
	T **minmax = new T*[elts];
	for(int e=0; e<elts; ++e){
		minmax[e] = new T[2];
		minmax[e][0] = (T)10000000000;
		minmax[e][1] = (T)-10000000000;
	}

	for(int i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				for(int e=0; e<n; ++e){
					minmax[e][0] = MIN(data[i*sx*sy*elts + j*sx*elts + (k+1)*elts + e], minmax[e][0]);
					minmax[e][1] = MAX(data[i*sx*sy*elts + j*sx*elts + (k+1)*elts + e], minmax[e][1]);
				}
			}
		}
	}
	
	for(e=0; e<elts; ++e){
		cerr << "Volume " << e << " , min: " << (float)minmax[e][0] << "   max: " << (float)minmax[e][1] << endl;
	}
#endif

	for(int i = 0; i < sz; ++i)
  {
		for(int j = 0; j < (sy); ++j)
    {
			for(int k = 0; k < (sx); ++k)
      {
				if((k<1)||(k>sx-2)||(j<1)||(j>sy-2)||(i<1)||(i>sz-2))
        {
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = 0;
				}
				else 
        {
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = 0;
					for(int e=0; e<n; ++e)
          {
						 gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] += data[i*sx*sy*elts + j*sx*elts + (k+1)*elts + e] -
							                                       data[i*sx*sy*elts + j*sx*elts + (k-1)*elts + e];
						 gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] += data[i*sx*sy*elts + (j+1)*sx*elts + k*elts + e] -
							                                       data[i*sx*sy*elts + (j-1)*sx*elts + k*elts + e];
						 gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] += data[(i+1)*sx*sy*elts + j*sx*elts + k*elts + e] -
							                                       data[(i-1)*sx*sy*elts + j*sx*elts + k*elts + e];
					}
				}
			}
		}
	}	
  
	return 0;
}

inline
void GMag(unsigned char *gmag, int sx, int sy, int sz, float *grad){
	float maxm = 0;
	float *tmp = new float[sx*sy*sz];
	for(int i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				tmp[i*sx*sy + j*sx + k] = normV3(&grad[i*sx*sy*3 + j*sx*3 + k*3]);
				maxm = MAX(tmp[i*sx*sy + j*sx + k], maxm);
			}
		}
	}
	for(i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				gmag[i*sx*sy + j*sx + k] = (unsigned char)(tmp[i*sx*sy + j*sx + k]/maxm * 255.0);
			}
		}
	}
	delete[] tmp;
}

inline
void GMagHess(unsigned char *gmag, unsigned char *hess, int sx, int sy, int sz, float *gradV3){
  
	float maxm = 0;
	//find the gradient magnitude and it's max val
	float *tmpg = new float[sx*sy*sz];
	for(int i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				tmpg[i*sx*sy + j*sx + k] = normV3(&gradV3[i*sx*sy*3 + j*sx*3 + k*3]);
				maxm = MAX(tmpg[i*sx*sy + j*sx + k], maxm);
			}
		}
	}

	float hmax = -100000000;
	float hmin = 100000000;
	float *tmph = new float[sx*sy*sz];

	//compute the 2nd derivative in the gradient direction
	//  a mask would probably help out here??
	for(i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				if((k<1)||(k>sx-2)||(j<1)||(j>sy-2)||(i<1)||(i>sz-2)){
					tmph[i*sx*sy + j*sx + k] = 0;
				}
				else {
					float h[9];
					h[0] = gradV3[i*sx*sy*3 + j*sx*3 + (k+1)*3 + 0] - gradV3[i*sx*sy*3 + j*sx*3 + (k-1)*3 + 0];
					h[1] = gradV3[i*sx*sy*3 + (j+1)*sx*3 + k*3 + 0] - gradV3[i*sx*sy*3 + (j-1)*sx*3 + k*3 + 0];
					h[2] = gradV3[(i+1)*sx*sy*3 + j*sx*3 + k*3 + 0] - gradV3[(i-1)*sx*sy*3 + j*sx*3 + k*3 + 0];
					h[3] = gradV3[i*sx*sy*3 + j*sx*3 + (k+1)*3 + 1] - gradV3[i*sx*sy*3 + j*sx*3 + (k-1)*3 + 1];
					h[4] = gradV3[i*sx*sy*3 + (j+1)*sx*3 + k*3 + 1] - gradV3[i*sx*sy*3 + (j-1)*sx*3 + k*3 + 1];
					h[5] = gradV3[(i+1)*sx*sy*3 + j*sx*3 + k*3 + 1] - gradV3[(i-1)*sx*sy*3 + j*sx*3 + k*3 + 1];
					h[6] = gradV3[i*sx*sy*3 + j*sx*3 + (k+1)*3 + 2] - gradV3[i*sx*sy*3 + j*sx*3 + (k-1)*3 + 2];
					h[7] = gradV3[i*sx*sy*3 + (j+1)*sx*3 + k*3 + 2] - gradV3[i*sx*sy*3 + (j-1)*sx*3 + k*3 + 2];
					h[8] = gradV3[(i+1)*sx*sy*3 + j*sx*3 + k*3 + 2] - gradV3[(i-1)*sx*sy*3 + j*sx*3 + k*3 + 2];
					float tv[3];
					float tg[3] = {gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0]/tmpg[i*sx*sy + j*sx + k], 
						             gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1]/tmpg[i*sx*sy + j*sx + k],
												 gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2]/tmpg[i*sx*sy + j*sx + k]};

					tv[0] = tg[0] * h[0] + 
						      tg[1] * h[1] +
									tg[2] * h[2];
					tv[1] = tg[0] * h[3] + 
						      tg[1] +
									tg[2] * h[5];
					tv[2] = tg[0] * h[6] + 
						      tg[1] * h[7] +
									tg[2] * h[8];
					tmph[i*sx*sy + j*sx + k] = tg[0] * tv[0] + 
						                         tg[1] * tv[1] +
									                   tg[2] * tv[2];
					hmax = MAX(hess[i*sx*sy + j*sx + k], hmax);
					hmin = MIN(hess[i*sx*sy + j*sx + k], hmin);

				}
			}
		}
	}

	//now quantize to chars
	for(i = 0; i < sz; ++i){ 
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				gmag[i*sx*sy + j*sx + k] = (unsigned char)(tmpg[i*sx*sy + j*sx + k]/maxm * 255.0);
				if(hess[i*sx*sy + j*sx + k] < 0){
						float th = (float)affine(hmin, tmph[i*sx*sy + j*sx + k], 0, 0, 1);
						//th = (float)(1.0-((1.0-th)*(1.0-th)*(1.0-th)));
						hess[i*sx*sy + j*sx + k] = (unsigned char)affine(0,th,1, 0, 255/3); 
				} else {
						float th = (float)affine(0, tmph[i*sx*sy + j*sx + k], hmax, 0, 1);
						//th = (float)(1.0-((1.0-th)*(1.0-th)*(1.0-th)));
						hess[i*sx*sy + j*sx + k] = (unsigned char)affine(0,th,1,255/3,255/3*2);
				}
			}
		}
	}
	delete[] tmpg;
	delete[] tmph;
}

//float vector scale and bias to 8bit unsigned char
inline void scalebias(unsigned char *ucgradV3, float *gradV3, int sx, int sy, int sz){
	int stx = 3;
	int sty = sx*3;
	int stz = sy*sx*3;
	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				ucgradV3[i*stz + j*sty + k*stx + 0] = (unsigned char)(gradV3[i*stz + j*sty + k*stx + 0]*128 + 128);
				ucgradV3[i*stz + j*sty + k*stx + 1] = (unsigned char)(gradV3[i*stz + j*sty + k*stx + 1]*128 + 128);
				ucgradV3[i*stz + j*sty + k*stx + 2] = (unsigned char)(gradV3[i*stz + j*sty + k*stx + 2]*128 + 128);
			}
		}
	}

}

//float vector normalize, scale, and bias to 8bit unsigned char
inline void scalebiasN(unsigned char *ucgradV3, float *gradV3, int sx, int sy, int sz){
	int stx = 3;
	int sty = sx*3;
	int stz = sy*sx*3;
	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				normalizeV3(&gradV3[i*stz + j*sty + k*stx + 0]);
				ucgradV3[i*stz + j*sty + k*stx + 0] = (unsigned char)(gradV3[i*stz + j*sty + k*stx + 0]*128 + 128);
				ucgradV3[i*stz + j*sty + k*stx + 1] = (unsigned char)(gradV3[i*stz + j*sty + k*stx + 1]*128 + 128);
				ucgradV3[i*stz + j*sty + k*stx + 2] = (unsigned char)(gradV3[i*stz + j*sty + k*stx + 2]*128 + 128);
			}
		}
	}

}

inline void scaleBiasHiLoUC(unsigned char *ucgradV2, float *gradV3, int sx, int sy, int sz){
	int stx = 3;
	int sty = sx*3;
	int stz = sy*sx*3;
	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				normalizeV3(&gradV3[i*stz + j*sty + k*stx + 0]);
				if(gradV3[i*stz + j*sty + k*stx + 2] < 0){
					gradV3[i*stz + j*sty + k*stx + 0] = -gradV3[i*stz + j*sty + k*stx + 0];
					gradV3[i*stz + j*sty + k*stx + 1] = -gradV3[i*stz + j*sty + k*stx + 1];
				}
				ucgradV2[i*sx*sy*2 + j*sx*2 + k*2 + 0] = (unsigned char)(gradV3[i*stz + j*sty + k*stx + 0]*128 + 128);
				ucgradV2[i*sx*sy*2 + j*sx*2 + k*2 + 1] = (unsigned char)(gradV3[i*stz + j*sty + k*stx + 1]*128 + 128);
			}
		}
	}

}

//copy scale and add vector

inline void csaddV3(GLfloat out[3], float s, GLfloat in[3])
{
  out[0] += s*in[0];
  out[1] += s*in[1];
  out[2] += s*in[2];
}

inline void csaddVARB(GLfloat out[3], float s, GLfloat in[3], int elts)
{
	for(int e=0; e<elts; ++e){
		out[e] += s*in[e];
	}
}

inline void csaddVARB(GLfloat out[3], float s, unsigned char in[3], int elts)
{
	for(int e=0; e<elts; ++e){
		out[e] += (float)(s*(in[e]/255.0));
	}
}

//blurr a 3D 3 vector field using these weights
//				------------
//       /w3 /w2 /w3 / 
//      /-----------/
//	   /w2 /w1 /w2 /        <-- z+1
//    /-----------/
//   /w3 /w2 /w3 /
//  /-----------/
//				------------
//       /w2 /w1 /w2 / y+1 
//      /-----------/
//	   /w1 /w0 /w1 / y      <-- z
//    /-----------/
//   /w2 /w1 /w2 / y-1
//  /-----------/
//				------------
//       /w3 /w2 /w3 / 
//      /-----------/
//	   /w2 /w1 /w2 /        <-- z-1
//    /-----------/
//   /w3 /w2 /w3 /
//  /-----------/
//   x-1  x  x+1

inline blurV3D(float *gradV3, float w0, float w1, float w2, float w3,
							 int sx, int sy, int sz){
	float *tmp = new float[sx*sy*sz*3];
	int stx = 3;
	int sty = sx*3;
	int stz = sy*sx*3;
	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				zeroV3(&tmp[i*stz + j*sty + k*stx + 0]);
			}
		}
	}
	
	for(i = 1; i<sz-1; ++i){
		for(int j = 1; j<sy-1; ++j){
			for(int k = 1; k<sx-1; ++k){
				int idx = i*stz + j*sty + k*stx + 0;
				csaddV3(&tmp[i*stz + j*sty + k*stx + 0], w0, &gradV3[idx]);
					csaddV3(&tmp[(i+1)*stz + j*sty + k*stx + 0], w1, &gradV3[idx]);
						csaddV3(&tmp[(i+1)*stz + (j+1)*sty + k*stx + 0], w2, &gradV3[idx]);
							csaddV3(&tmp[(i+1)*stz + (j+1)*sty + (k+1)*stx + 0], w3, &gradV3[idx]);						
							csaddV3(&tmp[(i+1)*stz + (j+1)*sty + (k-1)*stx + 0], w3, &gradV3[idx]);						
						csaddV3(&tmp[(i+1)*stz + (j-1)*sty + k*stx + 0], w2, &gradV3[idx]);
							csaddV3(&tmp[(i+1)*stz + (j-1)*sty + (k+1)*stx + 0], w3, &gradV3[idx]);
							csaddV3(&tmp[(i+1)*stz + (j-1)*sty + (k-1)*stx + 0], w3, &gradV3[idx]);
						csaddV3(&tmp[(i+1)*stz + j*sty + (k+1)*stx + 0], w2, &gradV3[idx]);
						csaddV3(&tmp[(i+1)*stz + j*sty + (k-1)*stx + 0], w2, &gradV3[idx]);

					csaddV3(&tmp[(i-1)*stz + j*sty + k*stx + 0], w1, &gradV3[i*stz + j*sty + k*stx + 0]);
						csaddV3(&tmp[(i-1)*stz + (j+1)*sty + k*stx + 0], w2, &gradV3[idx]);
							csaddV3(&tmp[(i-1)*stz + (j+1)*sty + (k+1)*stx + 0], w3, &gradV3[idx]);						
							csaddV3(&tmp[(i-1)*stz + (j+1)*sty + (k-1)*stx + 0], w3, &gradV3[idx]);						
						csaddV3(&tmp[(i-1)*stz + (j-1)*sty + k*stx + 0], w2, &gradV3[idx]);
							csaddV3(&tmp[(i-1)*stz + (j-1)*sty + (k+1)*stx + 0], w3, &gradV3[idx]);
							csaddV3(&tmp[(i-1)*stz + (j-1)*sty + (k-1)*stx + 0], w3, &gradV3[idx]);
						csaddV3(&tmp[(i-1)*stz + j*sty + (k+1)*stx + 0], w2, &gradV3[idx]);
						csaddV3(&tmp[(i-1)*stz + j*sty + (k-1)*stx + 0], w2, &gradV3[idx]);

					csaddV3(&tmp[i*stz + (j+1)*sty + k*stx + 0], w1, &gradV3[idx]);
						csaddV3(&tmp[i*stz + (j+1)*sty + (k+1)*stx + 0], w2, &gradV3[idx]);
						csaddV3(&tmp[i*stz + (j+1)*sty + (k-1)*stx + 0], w2, &gradV3[idx]);
					csaddV3(&tmp[i*stz + (j-1)*sty + k*stx + 0], w1, &gradV3[idx]);
						csaddV3(&tmp[i*stz + (j-1)*sty + (k+1)*stx + 0], w2, &gradV3[idx]);
						csaddV3(&tmp[i*stz + (j-1)*sty + (k-1)*stx + 0], w2, &gradV3[idx]);
					csaddV3(&tmp[i*stz + j*sty + (k+1)*stx + 0], w1, &gradV3[idx]);
					csaddV3(&tmp[i*stz + j*sty + (k-1)*stx + 0], w1, &gradV3[idx]);
			}
		}
	}

	float div = w0 + 6*w1 + 12*w2 + 8*w3;

	for(i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				gradV3[i*stz + j*sty + k*stx + 0] = tmp[i*stz + j*sty + k*stx + 0]/div;
				gradV3[i*stz + j*sty + k*stx + 1] = tmp[i*stz + j*sty + k*stx + 1]/div;
				gradV3[i*stz + j*sty + k*stx + 2] = tmp[i*stz + j*sty + k*stx + 2]/div;
			}
		}
	}
	
	delete[] tmp;
}

inline blurVARB(unsigned char *dataV, float w0, float w1, float w2, float w3,
							 int sx, int sy, int sz, int elts){
	
	//cerr << "BLUR :   " << sx << "  " << sy  << "  " << sz << " * " << elts << endl;
	
	float *tmp = new float[sx*sy*sz*elts];
	int stx = elts;
	int sty = sx*elts;
	int stz = sy*sx*elts;
		//cerr << "   stride " << stx << " " << sty << " " << stz << endl;



	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				for(int e = 0; e< elts; ++e){
					tmp[i*stz + j*sty + k*stx + e] = 0.0;
				}
			}
		}
	}
	
	for(i = 1; i<sz-1; ++i){
		for(int j = 1; j<sy-1; ++j){
			for(int k = 1; k<sx-1; ++k){
				for(int e = 0; e<elts; ++ e){		
					int idx = i*stz + j*sty + k*stx + e;
					float v0 = (float)(dataV[idx]/255.0*w0);
					float v1 = (float)(dataV[idx]/255.0*w1/6.0);
					float v2 = (float)(dataV[idx]/255.0*w2/12.0);
					float v3 = (float)(dataV[idx]/255.0*w3/8.0);
					
				  tmp[i*stz + j*sty + k*stx + e] += v0; //1
						tmp[(i+1)*stz + j*sty + k*stx + e] += v1; //1
							tmp[(i+1)*stz + (j+1)*sty + k*stx + e] += v2; //1
								tmp[(i+1)*stz + (j+1)*sty + (k+1)*stx + e] += v3;	//1					
								tmp[(i+1)*stz + (j+1)*sty + (k-1)*stx + e] += v3;	//2					
							tmp[(i+1)*stz + (j-1)*sty + k*stx + e] += v2; //2
								tmp[(i+1)*stz + (j-1)*sty + (k+1)*stx + e] += v3; //3
								tmp[(i+1)*stz + (j-1)*sty + (k-1)*stx + e] += v3; //4
							tmp[(i+1)*stz + j*sty + (k+1)*stx + e] += v2; //3
							tmp[(i+1)*stz + j*sty + (k-1)*stx + e] += v2; //4

						tmp[(i-1)*stz + j*sty + k*stx + e] += v1; //2
							tmp[(i-1)*stz + (j+1)*sty + k*stx + e] += v2; //5
								tmp[(i-1)*stz + (j+1)*sty + (k+1)*stx + e] += v3;	//5				
								tmp[(i-1)*stz + (j+1)*sty + (k-1)*stx + e] += v3;	//6					
							tmp[(i-1)*stz + (j-1)*sty + k*stx + e] += v2; //6
								tmp[(i-1)*stz + (j-1)*sty + (k+1)*stx + e] += v3; //7
								tmp[(i-1)*stz + (j-1)*sty + (k-1)*stx + e] += v3; //8
							tmp[(i-1)*stz + j*sty + (k+1)*stx + e] += v2; //7
							tmp[(i-1)*stz + j*sty + (k-1)*stx + e] += v2; //8

						tmp[i*stz + (j+1)*sty + k*stx + e] += v1; //3
							tmp[i*stz + (j+1)*sty + (k+1)*stx + e] += v2; //9
							tmp[i*stz + (j+1)*sty + (k-1)*stx + e] += v2; //10
						tmp[i*stz + (j-1)*sty + k*stx + e] += v1; //4
							tmp[i*stz + (j-1)*sty + (k+1)*stx + e] += v2; //11
							tmp[i*stz + (j-1)*sty + (k-1)*stx + e] += v2; //12
						tmp[i*stz + j*sty + (k+1)*stx + e] += v1; //5
						tmp[i*stz + j*sty + (k-1)*stx + e] += v1; //6
				}
			}
		}
	}

	//float div = w0 + 6*w1 + 12*w2 + 8*w3;
	float div = w0 + w1 + w2 + w3;

	for(i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				for(int e =0; e<elts; ++e){
					dataV[i*stz + j*sty + k*stx + e] = (unsigned char)(CLAMP((tmp[i*stz + j*sty + k*stx + e]/div))*255);
				}
			}
		}
	}
	
	delete[] tmp;  //why does this explode?????
}

//same as above but normalize too

inline blurV3DN(float *gradV3, float w0, float w1, float w2, float w3,
							 int sx, int sy, int sz){
	float *tmp = new float[sx*sy*sz*3];
	int stx = 3;
	int sty = sx*3;
	int stz = sy*sx*3;
	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				zeroV3(&tmp[i*stz + j*sty + k*stx + 0]);
			}
		}
	}
	
	for(i = 1; i<sz-1; ++i){
		for(int j = 1; j<sy-1; ++j){
			for(int k = 1; k<sx-1; ++k){
				int idx = i*stz + j*sty + k*stx + 0;
				csaddV3(&tmp[i*stz + j*sty + k*stx + 0], w0, &gradV3[idx]);
					csaddV3(&tmp[(i+1)*stz + j*sty + k*stx + 0], w1, &gradV3[idx]);
						csaddV3(&tmp[(i+1)*stz + (j+1)*sty + k*stx + 0], w2, &gradV3[idx]);
							csaddV3(&tmp[(i+1)*stz + (j+1)*sty + (k+1)*stx + 0], w3, &gradV3[idx]);						
							csaddV3(&tmp[(i+1)*stz + (j+1)*sty + (k-1)*stx + 0], w3, &gradV3[idx]);						
						csaddV3(&tmp[(i+1)*stz + (j-1)*sty + k*stx + 0], w2, &gradV3[idx]);
							csaddV3(&tmp[(i+1)*stz + (j-1)*sty + (k+1)*stx + 0], w3, &gradV3[idx]);
							csaddV3(&tmp[(i+1)*stz + (j-1)*sty + (k-1)*stx + 0], w3, &gradV3[idx]);
						csaddV3(&tmp[(i+1)*stz + j*sty + (k+1)*stx + 0], w2, &gradV3[idx]);
						csaddV3(&tmp[(i+1)*stz + j*sty + (k-1)*stx + 0], w2, &gradV3[idx]);

					csaddV3(&tmp[(i-1)*stz + j*sty + k*stx + 0], w1, &gradV3[i*stz + j*sty + k*stx + 0]);
						csaddV3(&tmp[(i-1)*stz + (j+1)*sty + k*stx + 0], w2, &gradV3[idx]);
							csaddV3(&tmp[(i-1)*stz + (j+1)*sty + (k+1)*stx + 0], w3, &gradV3[idx]);						
							csaddV3(&tmp[(i-1)*stz + (j+1)*sty + (k-1)*stx + 0], w3, &gradV3[idx]);						
						csaddV3(&tmp[(i-1)*stz + (j-1)*sty + k*stx + 0], w2, &gradV3[idx]);
							csaddV3(&tmp[(i-1)*stz + (j-1)*sty + (k+1)*stx + 0], w3, &gradV3[idx]);
							csaddV3(&tmp[(i-1)*stz + (j-1)*sty + (k-1)*stx + 0], w3, &gradV3[idx]);
						csaddV3(&tmp[(i-1)*stz + j*sty + (k+1)*stx + 0], w2, &gradV3[idx]);
						csaddV3(&tmp[(i-1)*stz + j*sty + (k-1)*stx + 0], w2, &gradV3[idx]);

					csaddV3(&tmp[i*stz + (j+1)*sty + k*stx + 0], w1, &gradV3[i*stz + j*sty + k*stx + 0]);
						csaddV3(&tmp[i*stz + (j+1)*sty + (k+1)*stx + 0], w2, &gradV3[i*stz + j*sty + k*stx + 0]);
						csaddV3(&tmp[i*stz + (j+1)*sty + (k-1)*stx + 0], w2, &gradV3[i*stz + j*sty + k*stx + 0]);
					csaddV3(&tmp[i*stz + (j-1)*sty + k*stx + 0], w1, &gradV3[i*stz + j*sty + k*stx + 0]);
						csaddV3(&tmp[i*stz + (j-1)*sty + (k+1)*stx + 0], w2, &gradV3[i*stz + j*sty + k*stx + 0]);
						csaddV3(&tmp[i*stz + (j-1)*sty + (k-1)*stx + 0], w2, &gradV3[i*stz + j*sty + k*stx + 0]);
					csaddV3(&tmp[i*stz + j*sty + (k+1)*stx + 0], w1, &gradV3[i*stz + j*sty + k*stx + 0]);
					csaddV3(&tmp[i*stz + j*sty + (k-1)*stx + 0], w1, &gradV3[i*stz + j*sty + k*stx + 0]);
			
			}
		}
	}

	float div = w0 + 6*w1 + 12*w2 + 8*w3;

	for(i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				scaleV3(1/div, &tmp[i*stz + j*sty + k*stx + 0]);
				normalizeV3(&tmp[i*stz + j*sty + k*stx + 0]);
				copyV3(&gradV3[i*stz + j*sty + k*stx + 0], &tmp[i*stz + j*sty + k*stx + 0]);
			}
		}
	}
						
	delete[] tmp;

}

//=====================================================================================
//---------------------- Quantize -----------------------------------------------------
//=====================================================================================

inline
void quantize(unsigned char *dout, int sx, int sy, int sz, unsigned short *din){

	unsigned short max = 0;
	unsigned short min = USHRT_MAX;
	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				max = MAX(max, din[i*sx*sy + j*sx + k]);
				min = MIN(min, din[i*sx*sy + j*sx + k]);

			}
		}
	}
	for(i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				dout[i*sx*sy + j*sx + k] = (unsigned char)affine(min,din[i*sx*sy + j*sx + k], max, 0, 255);
			}
		}
	}
}

inline
void quantize(unsigned char *dout, int sx, int sy, int sz, short *din){

	short max = SHRT_MIN;
	short min = SHRT_MAX;
	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				max = MAX(max, din[i*sx*sy + j*sx + k]);
				min = MIN(min, din[i*sx*sy + j*sx + k]);

			}
		}
	}
	for(i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				dout[i*sx*sy + j*sx + k] = (unsigned char)affine(min,din[i*sx*sy + j*sx + k], max, 0, 255);
			}
		}
	}
}

inline
void quantize(unsigned char *dout, int sx, int sy, int sz, int *din){

	int max = INT_MIN;
	int min = INT_MAX;
	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				max = MAX(max, din[i*sx*sy + j*sx + k]);
				min = MIN(min, din[i*sx*sy + j*sx + k]);

			}
		}
	}
	for(i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				dout[i*sx*sy + j*sx + k] = (unsigned char)affine(min,din[i*sx*sy + j*sx + k], max, 0, 255);
			}
		}
	}
}

inline
void quantize(unsigned char *dout, int sx, int sy, int sz, unsigned int *din){

	unsigned int max = 0;
	unsigned int min = UINT_MAX;
	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				max = MAX(max, din[i*sx*sy + j*sx + k]);
				min = MIN(min, din[i*sx*sy + j*sx + k]);

			}
		}
	}
	for(i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				dout[i*sx*sy + j*sx + k] = (unsigned char)affine(min,din[i*sx*sy + j*sx + k], max, 0, 255);
			}
		}
	}
}

inline
void quantize(unsigned char *dout, int sx, int sy, int sz, float *din){

	float max = -10000000000;
	float min = 10000000000;
	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				max = MAX(max, din[i*sx*sy + j*sx + k]);
				min = MIN(min, din[i*sx*sy + j*sx + k]);
			}
		}
	}
	for(i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				dout[i*sx*sy + j*sx + k] = (unsigned char)affine(min, din[i*sx*sy + j*sx + k], max, 0, 255);
			}
		}
	}
}

//=====================================================================================
//------------------------ endian conversions -----------------------------------------
//=====================================================================================

inline
void _nrrdSwapShortEndian(void *_data, int N) {
  short *data, s, fix;
  int I;
  
  if (_data) {
    data = (short *)_data;
    for (I=0; I<=N-1; I++) {
      s = data[I];
      fix =  (s & 0x00FF);
      fix = ((s & 0xFF00) >> 0x08) | (fix << 0x08);
      data[I] = fix;
    }
  }
}

inline
void _nrrdSwapWordEndian(void *_data, int N) {
  int *data, w, fix;
  int I;

  if (_data) {
    data = (int *)_data;
    for (I=0; I<=N-1; I++) {
      w = data[I];
      fix =  (w & 0x000000FF);
      fix = ((w & 0x0000FF00) >> 0x08) | (fix << 0x08);
      fix = ((w & 0x00FF0000) >> 0x10) | (fix << 0x08);
      fix = ((w & 0xFF000000) >> 0x18) | (fix << 0x08);
      data[I] = fix;
    }
  }
}

#if 0
inline
void _nrrdSwapLongLongWordEndian(void *_data, int N) {
  unsigned long long *data, l, fix;
  int I;

  if (_data) {
    data = (unsigned long long  *)_data;
    for (I=0; I<=N-1; I++) {
      l = data[I];
      fix =  (l & 0x00000000000000FF);
      fix = ((l & 0x000000000000FF00) >> 0x08) | (fix << 0x08);
      fix = ((l & 0x0000000000FF0000) >> 0x10) | (fix << 0x08);
      fix = ((l & 0x00000000FF000000) >> 0x18) | (fix << 0x08);
      fix = ((l & 0x000000FF00000000) >> 0x20) | (fix << 0x08);
      fix = ((l & 0x0000FF0000000000) >> 0x28) | (fix << 0x08);
      fix = ((l & 0x00FF000000000000) >> 0x30) | (fix << 0x08);
      fix = ((l & 0xFF00000000000000) >> 0x38) | (fix << 0x08);
      data[I] = fix;
    }
  }
}

#endif

#endif








