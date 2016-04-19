// <html><a href="../src/TLUT.cpp">source</a> <plaintext>
// ============================================================
//                          === TRex ===
// $Date: 2001/06/27 19:27:39 $
// $Revision: 1.2 $
// Authors: T. Alan Keahey, J. Kniss 
//
// TODO : This is probably general enough that it can be
//        moved into libviz.
//
// ============================================================
//
#ifndef TLUT_h
#define TLUT_h

class TLUT{
public:
  typedef enum{
    _numElts = 4 // width of the table (i.e. number of channels)
  }TLUTEnums;

  // constructors and destructors
  TLUT(const int size =256); // 256 = 8 bit, 4096 = 12 bit
  ~TLUT();

  // methods for loading TLUT into OpenGL tables
  // *** these assume that they are in a valid OpenGL context ***
  // *** they also assume some SGI-specific OpenGL extensions ***
  void loadTransferTableRGBA();
  void loadTransferTableALPHA();

  // copy and scale some external table
  void copyTable(float *cmap, float scale = 1.0);

  // generic methods for setting individual channels
  void channelConstant(const int channelIndex, const float alpha);
  void channelRamp(const int channelIndex, 
		   const int startIndex, const int stopIndex,  
		   const float startValue, const float stopValue);

   // method for scaling the alpha value based on the sample rate 
   // ie..sample rate per unit volume (voxel)
   // this method assumes that you want to use the TLUT right away so...
   // it reloads itself, ie. calls loadTransferTableRGBA() for you.
   void scaleAlpha(float sampleRate);
   
   // this method scales alpha lineary, ie applies the scale directly to it
   // it also reloads itself
   void linearScaleAlpha(float scale);

  // modify individual channels
  void redConstant(const float alpha) 
    { channelConstant(0, alpha); }
  void redRamp(const int startIndex, const int stopIndex, 
	       const float startValue, const float stopValue)
    { channelRamp(0, startIndex, stopIndex, startValue, stopValue); }
  void greenConstant(const float alpha) 
    { channelConstant(1, alpha); }
  void greenRamp(const int startIndex, const int stopIndex, 
		 const float startValue, const float stopValue)
    { channelRamp(1, startIndex, stopIndex, startValue, stopValue); }
  void blueConstant(const float alpha) 
    { channelConstant(2, alpha); }
  void blueRamp(const int startIndex, const int stopIndex, 
		const float startValue, const float stopValue)
    { channelRamp(2, startIndex, stopIndex, startValue, stopValue); }
  void alphaConstant(const float alpha) 
    { channelConstant(3, alpha); }
  void alphaRamp(const int startIndex, const int stopIndex, 
		 const float startValue, const float stopValue)
    { channelRamp(3, startIndex, stopIndex, startValue, stopValue); }

  // these load predefined colormaps ( RGB color values *ONLY* )
   void rgbConstant(float r, float g, float b);
   void rgbGrayScaleRamp();
   void rgbRedScaleRamp();
   void rgbSpectral(); //rainbow
   void rgbCyanMagenta();
   void rgbRobertsons(); //blue and yellow?
   void rgbMountain(); //blue to white 
   void rgbBlackBody(); //flame, black(blueish)->red->orange->yellow->white
   
  // methods for accessing color map elements
  inline int GetSize() const;

  // assumes t normalized to [0,1]
  inline float *GetRGBA(const float t) const;
  inline void   GetRGBA(const float t, float *r, float *g, 
			float *b, float *a) const;

  // GetRGBA(0) can be used to access entire map for 1D texture mapping
  // (i.e. returns a pointer to first element in array)
  inline float *GetRGBA(const int n) const;

  inline void   SetRGBA(const int n, float r, float g, 
			float b, float a);
  inline void   SetRGB(const int n, float r, float g, float b);
  inline void   SetAlpha(const int n, float a);

  // for the bitwise methods, "n" represents the index of
  // the bit to look for. starting with zero as the least
  // signicant bit
  inline void   SetBitwiseRGBA(const int n, float r, float g, 
			       float b, float a);
  inline void   SetBitwiseRGB(const int n, float r, float g, float b);
  inline void   SetBitwiseAlpha(const int n, float a);

protected:

  float *theTable;
  float *_rgba;
  int    _size;
  float  _alpha;
  float  lastSampleRate;
  float  lastAlphaScale;
};

//***************************************************************
// ELEMENT ACCESSORS AND  MODIFIERS
//***************************************************************
inline int
TLUT::GetSize() const 
{ 
  return _size; 
}

inline float *
TLUT::GetRGBA(const float t) const  
{
  int offset = (int)(t * (_size - 1));
  return &(_rgba[offset*_numElts]);
}

inline void 
TLUT::GetRGBA(const float t, float *r, float *g, float *b, float *a) const  
{
  int offset = (int)(t * (_size - 1) * _numElts);
  *r = _rgba[offset];
  *g = _rgba[offset+1];
  *b = _rgba[offset+2];
  *a = _rgba[offset+3];
}

inline float *
TLUT::GetRGBA(const int n) const
{
  return &(_rgba[n*_numElts]);
}

inline void   
TLUT::SetRGBA(const int n, float r, float g, float b, float a)
{
  SetRGB(n, r, g, b);
  SetAlpha(n, a);
}
inline void   
TLUT::SetRGB(const int n, float r, float g, float b)
{
  int offset = n * _numElts;
  _rgba[offset] = r;
  _rgba[offset+1] = g;
  _rgba[offset+2] = b;
} 
inline void   
TLUT::SetAlpha(const int n, float a)
{
  int offset = n * _numElts;
  _rgba[offset+3] = a;
}

inline void   
TLUT::SetBitwiseRGBA(const int n, float r, float g, float b, float a)
{
  SetBitwiseRGB(n, r, g, b);
  SetBitwiseAlpha(n, a);
}
inline void   
TLUT::SetBitwiseRGB(const int n, float r, float g, float b)
{
  unsigned int mask = 0x1 << n;
  for(unsigned int k = 0; k < (unsigned int)_size; ++k){
    if(k & mask){
      int offset = k * _numElts;
      _rgba[offset] = r;
      _rgba[offset+1] = g;
      _rgba[offset+2] = b;
    }
  }
} 
inline void   
TLUT::SetBitwiseAlpha(const int n, float a)
{
  unsigned int mask = 0x1 << n;
  for(unsigned int k = 0; k < (unsigned int)_size; ++k){
    if(k & mask){
      int offset = k * _numElts;
      _rgba[offset+3] = a;
    }
  }
}


#endif
