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
// PNoise.h: interface for the PNoise class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PNOISE_H__C1CDB3F2_6FDA_49C8_BF27_1DEDA73F3EE5__INCLUDED_)
#define AFX_PNOISE_H__C1CDB3F2_6FDA_49C8_BF27_1DEDA73F3EE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PNoise  
{
public:
	PNoise();           //seeds itself with the time
	PNoise(int seed);   //you choose the seed
	virtual ~PNoise();

  void seed();      //seed rand with the time
  void seed(int s); //specify seed

  /*
   In what follows "alpha" is the weight when the sum is formed.
   Typically it is 2, As this approaches 1 the function is noisier.
   "beta" is the harmonic scaling/spacing, typically 2. "n" is the
   number of harmonics to sum.  

  Notice: n, alpha, and beta have default values set.
  These match Ken's original calls
  */
  double PerlinNoise1D(double x,int n =1, double alpha =2,double beta =2);
  double PerlinNoise2D(double x,double y,int n =1,double alpha =2,double beta =2);
  double PerlinNoise3D(double x,double y,double z,int n =1,double alpha =2,double beta =2);
  double PerlinNoise3DABS(double x,double y,double z,int n =1, double alpha =2,double beta =2);

  // This next set is to simplify calls with aliases
  void   setParams(int n, double alpha =2, double beta =2);
  double PNoise1D(double x);
  double PNoise2D(double x, double y);
  double PNoise3D(double x, double y, double z);
  double PNoise3DA(double x, double y, double z);

  // TO DO: fast 2D & 3D field generation, Vector noise

private:
  void init(void);
  double noise1(double);
  double noise2(double *);
  double noise3(double *);
  void normalize3(double *);
  void normalize2(double *);

  int    *p;
  double **g3;
  double **g2;
  double *g1;
  int    start;

  inline int random();

  double _alpha;
  double _beta;
  int    _n;

};

#endif // !defined(AFX_PNOISE_H__C1CDB3F2_6FDA_49C8_BF27_1DEDA73F3EE5__INCLUDED_)
