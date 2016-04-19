//
// ============================================================
//                          === TRex ===
// $Date: 2001/06/27 21:05:15 $
// $Revision: 1.3 $
// Authors: T. Alan Keahey, J. Kniss 
//
//   Definitions for a Texture Look Up Table.
//
// ============================================================
//

#ifdef WIN32
#include <windows.h>
#endif

#include "TLUT.h"
#include <GL/gl.h>
#include <math.h>
#include <iostream.h>
#include "glUE.h"

//***************************************************************
// DE/CONSTRUCTORS
//***************************************************************
TLUT::TLUT(const int size) 
{
  _size = size;
  _rgba = new float[_numElts * size];
  theTable = new float[_numElts * size];
  _alpha = (float)(1.0 / _size);
  rgbGrayScaleRamp();
  alphaConstant(_alpha);
  lastSampleRate = 1.0f;
  lastAlphaScale = 1.0f;
}

TLUT::~TLUT() 
{ 
  delete _rgba;
  delete theTable;
}

//***************************************************************
// TLUT LOADING METHODS
//***************************************************************
void
TLUT::loadTransferTableRGBA()
{
  // this is an optional scaling factor. if you want identity scaling
  // (scale = 1), then you could skip theTable construction and
  // pass this->GetRGBA(0) instead of theTable to glColorTableSGI()
  float scale = 1;

  float *rgba;

#ifdef WIN32
#if    WILDCAT
  glEnable(GL_TEXTURE_COLOR_TABLE_EXT);
#endif
#else
  glEnable(GL_TEXTURE_COLOR_TABLE_SGI);
#endif

  for(int n = 0; n < _size; ++n){
    rgba = this->GetRGBA(n);
    theTable[n * _numElts + 0] = rgba[0] * scale * rgba[3];
    theTable[n * _numElts + 1] = rgba[1] * scale * rgba[3];
    theTable[n * _numElts + 2] = rgba[2] * scale * rgba[3];
    theTable[n * _numElts + 3] = rgba[3] * scale;
  }
#ifdef WIN32
#if    WILDCAT
  glColorTableEXT(GL_TEXTURE_COLOR_TABLE_EXT, GL_RGBA8, _size,
 		  GL_RGBA, GL_FLOAT, theTable);
#endif
#else
  glColorTableSGI(GL_TEXTURE_COLOR_TABLE_SGI, GL_RGBA8_EXT, _size,
 		  GL_RGBA, GL_FLOAT, theTable);
#endif
}

void
TLUT::loadTransferTableALPHA()
{
  float *rgba;
  float *alphaTable = new float[_size];
  for(int n = 0; n < _size; ++n){
    rgba = this->GetRGBA(n);
    alphaTable[n] = rgba[3];
  }
#ifdef WIN32
#if    WILDCAT
  glColorTableEXT(GL_TEXTURE_COLOR_TABLE_EXT, GL_ALPHA, _size,
 		  GL_RGBA, GL_FLOAT, alphaTable);
#endif
#else
  glColorTableSGI(GL_TEXTURE_COLOR_TABLE_SGI, GL_ALPHA, _size,
		  GL_ALPHA, GL_FLOAT, alphaTable);
#endif
}

void TLUT::copyTable(float *cmap, float scale)
{
   for(int n=0; n < _size; ++n){
      _rgba[n*_numElts+0] = cmap[n*_numElts+0];
      _rgba[n*_numElts+1] = cmap[n*_numElts+1];
      _rgba[n*_numElts+2] = cmap[n*_numElts+2];
      _rgba[n*_numElts+3] = cmap[n*_numElts+3]*scale;
   }
}

//***************************************************************
// CHANNEL MODIFIERS
//***************************************************************
void 
TLUT::channelConstant(const int channelIndex, const float a)
{
  for(int n = 0; n < _size; ++n){
    _rgba[n*_numElts+channelIndex] = a;
  }
}

void 
TLUT::channelRamp(const int channelIndex,
		  const int startIndex, const int stopIndex, 
		  const float startValue, const float stopValue)
{
  float denom = stopIndex - startIndex;
  float range = stopValue - startValue;

  for(int n = startIndex; n <= stopIndex; ++n){
    float alpha = startValue + range*(n - startIndex)/denom;
    _rgba[n*_numElts+channelIndex] = alpha;
  }
}

void TLUT::scaleAlpha(float sampleRate)
{
   //for an explantation of this see 'gkalpha.txt'
   if(lastSampleRate == sampleRate) return;
   
   float alphaScale = lastSampleRate/sampleRate;
   //float *rgba;
   lastSampleRate = sampleRate;
   //it is a damn good thing we dont have many entries in the TLUT
   for(int i=0; i<_size; ++i){
     
      _rgba[i *_numElts + 3] = 1- pow((1- _rgba[i*_numElts +3]), alphaScale);
   }

   loadTransferTableRGBA();

}

void TLUT::linearScaleAlpha(float scale)
{
   if(lastAlphaScale == scale) return;
   
   float alphaScale = scale/lastAlphaScale;
   lastAlphaScale = scale;
   cout << "scaling alpha by:" << alphaScale<<endl;
   for(int i=0; i< _size; ++i){
      _rgba[i *_numElts + 3] = _rgba[i*_numElts +3] * alphaScale;
   }
   loadTransferTableRGBA();
}

//***************************************************************
// RGB MODIFIERS (PREDEFINED COLORMAPS)
//***************************************************************
void 
TLUT::rgbConstant(float r, float g, float b)
{
  for(int n = 0; n < _size; ++n){
    _rgba[n*_numElts] = r;
    _rgba[n*_numElts+1] = g;
    _rgba[n*_numElts+2] = b;
  }
}

void 
TLUT::rgbGrayScaleRamp()
{
  for(int n = 0; n < _size; ++n){
    _rgba[n*_numElts] = _rgba[n*_numElts+1] = _rgba[n*_numElts+2] = 
      n / (float)(_size - 1.0);
  }
}

void 
TLUT::rgbRedScaleRamp()
{
  for(int n = 0; n < _size; ++n){
    _rgba[n*_numElts] = 0.9;
    _rgba[n*_numElts+1] = _rgba[n*_numElts+2] = 0.9 * n / (float) (_size - 1.0);
  }
}

void 
TLUT::rgbSpectral()
{
  int n;
  float t;
  
  float r1 = 238/255.0;
  float g1 = 138/255.0;
  float b1 = 238/255.0;
  float r2 = 25/255.0;
  float g2 = 25/255.0;
  float b2 = 112/255.0;
  int min = 0;
  int max = _size/16;
  for(n = min; n < max; ++n){
    t = (n-min)/(max-min-1.0);
    _rgba[n*_numElts+0] = (1-t) * r1 + t * r2;
    _rgba[n*_numElts+1] = (1-t) * g1 + t * g2;
    _rgba[n*_numElts+2] = (1-t) * b1 + t * b2;
  }
  
  r1 = r2;  g1 = g2;  b1 = b2;
  r2 = 0/255.0;
  g2 = 0/255.0;
  b2 = 255/255.0;
  min = max;
  max += 3*_size/16;
  for(n = min; n < max; ++n){
    t = (n-min)/(max-min-1.0);
    _rgba[n*_numElts+0] = (1-t) * r1 + t * r2;
    _rgba[n*_numElts+1] = (1-t) * g1 + t * g2;
    _rgba[n*_numElts+2] = (1-t) * b1 + t * b2;
  }
  
  r1 = r2;  g1 = g2;  b1 = b2;
  r2 = 0/255.0;
  g2 = 255/255.0;
  b2 = 0/255.0;
  min = max;
  max += 3*_size/16;
  for(n = min; n < max; ++n){
    t = (n-min)/(max-min-1.0);
    _rgba[n*_numElts+0] = (1-t) * r1 + t * r2;
    _rgba[n*_numElts+1] = (1-t) * g1 + t * g2;
    _rgba[n*_numElts+2] = (1-t) * b1 + t * b2;
  }
  
  r1 = r2;  g1 = g2;  b1 = b2;
  r2 = 173/255.0;
  g2 = 252/255.0;
  b2 = 0/255.0;
  min = max;
  max += 3*_size/16;
  for(n = min; n < max; ++n){
    t = (n-min)/(max-min-1.0);
    _rgba[n*_numElts+0] = (1-t) * r1 + t * r2;
    _rgba[n*_numElts+1] = (1-t) * g1 + t * g2;
    _rgba[n*_numElts+2] = (1-t) * b1 + t * b2;
  }
  
  r1 = r2;  g1 = g2;  b1 = b2;
  r2 = 255/255.0;
  g2 = 255/255.0;
  b2 = 0/255.0;
  min = max;
  max += 1*_size/16;
  for(n = min; n < max; ++n){
    t = (n-min)/(max-min-1.0);
    _rgba[n*_numElts+0] = (1-t) * r1 + t * r2;
    _rgba[n*_numElts+1] = (1-t) * g1 + t * g2;
    _rgba[n*_numElts+2] = (1-t) * b1 + t * b2;
  }
  
  r1 = r2;  g1 = g2;  b1 = b2;
  r2 = 255/255.0;
  g2 = 165/255.0;
  b2 = 0/255.0;
  min = max;
  max += 2*_size/16;
  for(n = min; n < max; ++n){
    t = (n-min)/(max-min-1.0);
    _rgba[n*_numElts+0] = (1-t) * r1 + t * r2;
    _rgba[n*_numElts+1] = (1-t) * g1 + t * g2;
    _rgba[n*_numElts+2] = (1-t) * b1 + t * b2;
  }
  
  r1 = r2;  g1 = g2;  b1 = b2;
  r2 = 255/255.0;
  g2 = 0/255.0;
  b2 = 0/255.0;
  min = max;
  max += 3*_size/16;
  for(n = min; n < max; ++n){
    t = (n-min)/(max-min-1.0);
    _rgba[n*_numElts+0] = (1-t) * r1 + t * r2;
    _rgba[n*_numElts+1] = (1-t) * g1 + t * g2;
    _rgba[n*_numElts+2] = (1-t) * b1 + t * b2;
  }
}

void 
TLUT::rgbCyanMagenta()
{
  float r1 = 1.0;
  float g1 = 0.0;
  float b1 = 1.0;
  float r2 = 0.0;
  float g2 = 1.0;
  float b2 = 1.0;
  float t;
  for(int n = 0; n < _size; ++n){
    t = n / (float) (_size - 1.0);
    _rgba[n*_numElts+0] = (1-t) * r1 + t * r2;
    _rgba[n*_numElts+1] = (1-t) * g1 + t * g2;
    _rgba[n*_numElts+2] = (1-t) * b1 + t * b2;
  }
}

void 
TLUT::rgbRobertsons()
{
  int n;
  float t;
  
  float r1 = 0/255.0;
  float g1 = 0/255.0;
  float b1 = 255/255.0;
  float r2 = 25/255.0;
  float g2 = 25/255.0;
  float b2 = 230/255.0;
  int min = 0;
  int max = _size/6;
  for(n = min; n < max; ++n){
    t = (n-min)/(max-min-1.0);
    _rgba[n*_numElts+0] = (1-t) * r1 + t * r2;
    _rgba[n*_numElts+1] = (1-t) * g1 + t * g2;
    _rgba[n*_numElts+2] = (1-t) * b1 + t * b2;
  }
  
  r1 = r2;  g1 = g2;  b1 = b2;
  r2 = 55/255.0;
  g2 = 55/255.0;
  b2 = 200/255.0;
  min = max;
  max += _size/6;
  for(n = min; n < max; ++n){
    t = (n-min)/(max-min-1.0);
    _rgba[n*_numElts+0] = (1-t) * r1 + t * r2;
    _rgba[n*_numElts+1] = (1-t) * g1 + t * g2;
    _rgba[n*_numElts+2] = (1-t) * b1 + t * b2;
  }
  
  r1 = r2;  g1 = g2;  b1 = b2;
  r2 = 127/255.0;
  g2 = 127/255.0;
  b2 = 127/255.0;
  min = max;
  max += _size/6;
  for(n = min; n < max; ++n){
    t = (n-min)/(max-min-1.0);
    _rgba[n*_numElts+0] = (1-t) * r1 + t * r2;
    _rgba[n*_numElts+1] = (1-t) * g1 + t * g2;
    _rgba[n*_numElts+2] = (1-t) * b1 + t * b2;
  }
  
  r1 = r2;  g1 = g2;  b1 = b2;
  r2 = 200/255.0;
  g2 = 200/255.0;
  b2 = 55/255.0;
  min = max;
  max += _size/6;
  for(n = min; n < max; ++n){
    t = (n-min)/(max-min-1.0);
    _rgba[n*_numElts+0] = (1-t) * r1 + t * r2;
    _rgba[n*_numElts+1] = (1-t) * g1 + t * g2;
    _rgba[n*_numElts+2] = (1-t) * b1 + t * b2;
  }
  
  r1 = r2;  g1 = g2;  b1 = b2;
  r2 = 230/255.0;
  g2 = 230/255.0;
  b2 = 25/255.0;
  min = max;
  max += _size/6;
  for(n = min; n < max; ++n){
    t = (n-min)/(max-min-1.0);
    _rgba[n*_numElts+0] = (1-t) * r1 + t * r2;
    _rgba[n*_numElts+1] = (1-t) * g1 + t * g2;
    _rgba[n*_numElts+2] = (1-t) * b1 + t * b2;
  }
  
  r1 = r2;  g1 = g2;  b1 = b2;
  r2 = 255/255.0;
  g2 = 255/255.0;
  b2 = 0/255.0;
  min = max;
  max += _size/6;
  for(n = min; n < max; ++n){
    t = (n-min)/(max-min-1.0);
    _rgba[n*_numElts+0] = (1-t) * r1 + t * r2;
    _rgba[n*_numElts+1] = (1-t) * g1 + t * g2;
    _rgba[n*_numElts+2] = (1-t) * b1 + t * b2;
  }

  // pad extra cells with final value
  for(n = max; n < _size; ++n){
    _rgba[n*_numElts+0] = r2;
    _rgba[n*_numElts+1] = g2;
    _rgba[n*_numElts+2] = b2;
  }
}

void 
TLUT::rgbMountain()
{
  float r1 = 0.0;
  float g1 = 0.0;
  float b1 = 0.0;
  float r2 = 0.0;
  float g2 = 0.0;
  float b2 = 1.0;
  float t;
  int n;
  for(n = 0; n < _size/4; ++n){
    t = n / (float) (_size/4 - 1.0);
    _rgba[n*_numElts+0] = (1-t) * r1 + t * r2;
    _rgba[n*_numElts+1] = (1-t) * g1 + t * g2;
    _rgba[n*_numElts+2] = (1-t) * b1 + t * b2;
  }
  r1 = r2;  g1 = g2;  b1 = b2;
  r2 = 1.0;
  g2 = 1.0;
  b2 = 1.0;

  float min = _size/4;
  float max = _size;
  for(n = min; n < max; ++n){
    t = (n-min)/(max-min-1.0);
    _rgba[n*_numElts+0] = (1-t) * r1 + t * r2;
    _rgba[n*_numElts+1] = (1-t) * g1 + t * g2;
    _rgba[n*_numElts+2] = (1-t) * b1 + t * b2;
  }

  // pad extra cells with final value
  for(n = max; n < _size; ++n){
    _rgba[n*_numElts+0] = r2;
    _rgba[n*_numElts+1] = g2;
    _rgba[n*_numElts+2] = b2;
  }
}

#define AFFINE(i,x,I,o,O) ( ((double)(O)-(o))*((double)(x)-(i)) \
			    / ((double)(I)-(i)) + (o))

void TLUT::rgbBlackBody()
{
   for(int i=0; i<_size/3; ++i){//ramp up red, down blue
      _rgba[i*_numElts + 0] = (unsigned char) AFFINE(0, i, _size/3, 0, 1);
      _rgba[i*_numElts + 1] = 0;
      _rgba[i*_numElts + 2] = (unsigned char) AFFINE(0, i, _size/3, .4, 0);
   }
   for(i=_size/3; i<2*_size/3; ++i){//ramp up green (yellow)
      _rgba[i*_numElts + 0] = 1;
      _rgba[i*_numElts + 1] = (unsigned char) AFFINE(_size/3, i, 2*_size/3, 0, 1);
      _rgba[i*_numElts + 2] = 0;
   }
   for(i=2*_size/3; i<_size; ++i){//ramp up blue (white)
      _rgba[i*_numElts + 0] = 1;
      _rgba[i*_numElts + 1] = 1;
      _rgba[i*_numElts + 2] = (unsigned char)AFFINE(2*_size/3, i, _size, 0, 1.05);
   }

} 
