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
// PNoise.cpp: implementation of the PNoise class.
//
//////////////////////////////////////////////////////////////////////

#include "PNoise.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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
}

PNoise::~PNoise()
{

}
