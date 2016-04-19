//-------------------------------------------------------------------------
//
//   Joe Kniss
//    Perlin Noise Generator
//  PNoise.h Handles basic perlin noise generation, taken from Ken's 
//    original implementaion, adapted for C++  
//     12-9-01
//                   ________    ____   ___ 
//                  |        \  /    | /  /
//                  +---+     \/     |/  /
//                  +--+|  |\    /|     < 
//                  |  ||  | \  / |  |\  \ 
//                  |      |  \/  |  | \  \ 
//                   \_____|      |__|  \__\
//                       Copyright  2001 
//                      Joe Michael Kniss
//               "All Your Base are Belong to Us"
//-------------------------------------------------------------------------
// PNoise.cpp: implementation of the PNoise class.
//
//////////////////////////////////////////////////////////////////////

#include "PNoise.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

//================ Defines ===========================================
//====================================================================
#define B 0x100
#define BM 0xff
#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

#define s_curve(t) ( t * t * (3. - 2. * t) )
#define lerp(t, a, b) ( a + t * (b - a) )
#define setup(i,b0,b1,r0,r1)\
        t = vec[i] + N;\
        b0 = ((int)t) & BM;\
        b1 = (b0+1) & BM;\
        r0 = t - (int)t;\
        r1 = r0 - 1.;
#define at2(rx,ry) ( rx * q[0] + ry * q[1] )
#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PNoise::PNoise()
{
  p = new int[B + B + 2];
  
  g3 = new double*[B + B + 2];
  for(int i=0; i<(B + B + 2); ++i){
    g3[i] = new double[3];
  }
  
  g2 = new double*[B + B + 2];
  for(i=0; i<(B + B + 2); ++i){
    g2[i] = new double[2];
  }

  g1 = new double[B + B + 2];
  
  start = 1;

  _n = 1;
  _alpha = 2;
  _beta  = 2;

  seed();
  init();
}

PNoise::PNoise(int s)
{
  p = new int[B + B + 2];
  
  g3 = new double*[B + B + 2];
  for(int i=0; i<(B + B + 2); ++i){
    g3[i] = new double[3];
  }
  
  g2 = new double*[B + B + 2];
  for(i=0; i<(B + B + 2); ++i){
    g2[i] = new double[2];
  }

  g1 = new double[B + B + 2];
  
  start = 1;

  _n = 1;
  _alpha = 2;
  _beta  = 2;

  seed(s);
  init();
}

PNoise::~PNoise()
{
  delete[] p;
  for(int i=0; i< B+B+2; ++ i)
    delete[] g3[i];
  delete[] g3;
  for(i=0; i< B+B+2; ++ i)
    delete[] g2[i];
  delete[] g2;
  delete[] g1;
}

//////////////////////////////////////////////////////////////////////
// Public
//////////////////////////////////////////////////////////////////////

void PNoise::seed()
{
  srand(time(NULL));
}

void PNoise::seed(int s)
{
  srand(s);
}


double PNoise::PerlinNoise1D(double x, int n, double alpha,double beta)
{
   int i;
   double val,sum = 0;
   double p,scale = 1;

   p = x;
   for (i=0;i<n;i++) {
      val = noise1(p);
      sum += val / scale;
      scale *= alpha;
      p *= beta;
   }
   return(sum);
}

//======================================================================================
//======================================================================================
double PNoise::PerlinNoise2D(double x,double y, int n, double alpha,double beta)
{
   int i;
   double val,sum = 0;
   double p[2],scale = 1;

   p[0] = x;
   p[1] = y;
   for (i=0;i<n;i++) {
      val = noise2(p);
      sum += val / scale;
      scale *= alpha;
      p[0] *= beta;
      p[1] *= beta;
   }
   return(sum);
}

//======================================================================================
//======================================================================================
double PNoise::PerlinNoise3D(double x,double y,double z, int n, double alpha,double beta)
{
   int i;
   double val,sum = 0;
   double p[3],scale = 1;

   p[0] = x;
   p[1] = y;
   p[2] = z;
   for (i=0;i<n;i++) {
      val = noise3(p);
      sum += val / scale;
      scale *= alpha;
      p[0] *= beta;
      p[1] *= beta;
      p[2] *= beta;
   }
   return(sum);
}

//======================================================================================
//======================================================================================
#ifndef ABS
#define ABS(x) ((x)<0 ? (-x) : (x))
#endif

double PNoise::PerlinNoise3DABS(double x,double y,double z, int n, double alpha,double beta)
{
   int i;
   double val,sum = 0;
   double p[3],scale = 1;

   p[0] = x;
   p[1] = y;
   p[2] = z;
   for (i=0;i<n;i++) {
      val = noise3(p);
			val = ABS(val);
      sum += val / scale;
      scale *= alpha;
      p[0] *= beta;
      p[1] *= beta;
      p[2] *= beta;
   }
   return(sum);
}

//======================================================================================
//======================================================================================
void PNoise::setParams(int n, double alpha, double beta)
{
  _alpha = alpha;
  _beta  = beta;
  _n     = n;
}

//======================================================================================
//======================================================================================
double PNoise::PNoise1D(double x)
{
  return PerlinNoise1D(x,_n,_alpha,_beta);
}

//======================================================================================
//======================================================================================
double PNoise::PNoise2D(double x, double y)
{
  return PerlinNoise2D(x, y, _n, _alpha, _beta);
}

//======================================================================================
//======================================================================================
double PNoise::PNoise3D(double x, double y, double z)
{
  return PerlinNoise3D(x,y,z, _n, _alpha, _beta);
}

//======================================================================================
//======================================================================================
double PNoise::PNoise3DA(double x, double y, double z)
{
  return PerlinNoise3DABS(x,y,z, _n, _alpha, _beta);
}


//======================================================================================
//======================================================================================
////////////////////////////////////////////////////////////////////////////////////////
// Private
////////////////////////////////////////////////////////////////////////////////////////
//======================================================================================
//======================================================================================

inline int PNoise::random()
{
  return rand();
}

//======================================================================================
//======================================================================================
double PNoise::noise1(double arg)
{
   int bx0, bx1;
   double rx0, rx1, sx, t, u, v, vec[1];

   vec[0] = arg;
   if (start) {
      start = 0;
      init();
   }

   setup(0,bx0,bx1,rx0,rx1);

   sx = s_curve(rx0);
   u = rx0 * g1[ p[ bx0 ] ];
   v = rx1 * g1[ p[ bx1 ] ];

   return(lerp(sx, u, v));
}

//======================================================================================
//======================================================================================
double PNoise::noise2(double vec[2])
{
   int bx0, bx1, by0, by1, b00, b10, b01, b11;
   double rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
   int i, j;

   if (start) {
      start = 0;
      init();
   }

   setup(0, bx0,bx1, rx0,rx1);
   setup(1, by0,by1, ry0,ry1);

   i = p[ bx0 ];
   j = p[ bx1 ];

   b00 = p[ i + by0 ];
   b10 = p[ j + by0 ];
   b01 = p[ i + by1 ];
   b11 = p[ j + by1 ];

   sx = s_curve(rx0);
   sy = s_curve(ry0);

   q = g2[ b00 ] ; u = at2(rx0,ry0);
   q = g2[ b10 ] ; v = at2(rx1,ry0);
   a = lerp(sx, u, v);

   q = g2[ b01 ] ; u = at2(rx0,ry1);
   q = g2[ b11 ] ; v = at2(rx1,ry1);
   b = lerp(sx, u, v);

   return lerp(sy, a, b);
}

//======================================================================================
//======================================================================================
double PNoise::noise3(double vec[3])
{
   int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
   double rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
   int i, j;

   if (start) {
      start = 0;
      init();
   }

   setup(0, bx0,bx1, rx0,rx1);
   setup(1, by0,by1, ry0,ry1);
   setup(2, bz0,bz1, rz0,rz1);

   i = p[ bx0 ];
   j = p[ bx1 ];

   b00 = p[ i + by0 ];
   b10 = p[ j + by0 ];
   b01 = p[ i + by1 ];
   b11 = p[ j + by1 ];

   t  = s_curve(rx0);
   sy = s_curve(ry0);
   sz = s_curve(rz0);

   q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
   q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
   a = lerp(t, u, v);

   q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
   q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
   b = lerp(t, u, v);

   c = lerp(sy, a, b);

   q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
   q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
   a = lerp(t, u, v);

   q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
   q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
   b = lerp(t, u, v);

   d = lerp(sy, a, b);

   return lerp(sz, c, d);
}

//======================================================================================
//======================================================================================
void PNoise::normalize2(double v[2])
{
   double s;

   s = sqrt(v[0] * v[0] + v[1] * v[1]);
   v[0] = v[0] / s;
   v[1] = v[1] / s;
}

//======================================================================================
//======================================================================================
void PNoise::normalize3(double v[3])
{
   double s;

   s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
   v[0] = v[0] / s;
   v[1] = v[1] / s;
   v[2] = v[2] / s;
}

//======================================================================================
//======================================================================================
void PNoise::init(void)
{
   int i, j, k;

   for (i = 0 ; i < B ; i++) {
      p[i] = i;
      g1[i] = (double)((random() % (B + B)) - B) / B;

      for (j = 0 ; j < 2 ; j++)
         g2[i][j] = (double)((random() % (B + B)) - B) / B;
      normalize2(g2[i]);

      for (j = 0 ; j < 3 ; j++)
         g3[i][j] = (double)((random() % (B + B)) - B) / B;
      normalize3(g3[i]);
   }

   while (--i) {
      k = p[i];
      p[i] = p[j = random() % B];
      p[j] = k;
   }

   for (i = 0 ; i < B + 2 ; i++) {
      p[B + i] = p[i];
      g1[B + i] = g1[i];
      for (j = 0 ; j < 2 ; j++)
         g2[B + i][j] = g2[i][j];
      for (j = 0 ; j < 3 ; j++)
         g3[B + i][j] = g3[i][j];
   }
}