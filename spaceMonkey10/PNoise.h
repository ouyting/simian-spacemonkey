//-------------------------------------------------------------------------
//
//   Joe Kniss
//    Perlin Noise Generator
//  PNoise.h Handles basic perlin noise generation, taken from Ken's 
//    origional implementaion  
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
	PNoise();
	virtual ~PNoise();

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

};

#endif // !defined(AFX_PNOISE_H__C1CDB3F2_6FDA_49C8_BF27_1DEDA73F3EE5__INCLUDED_)
