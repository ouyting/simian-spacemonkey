//////////////////////////////////////////////////////////////////////
//                   ________    ____   ___ 
//                  |        \  /    | /  /
//                  +---+     \/     |/  /
//                  +--+|  |\    /|     < 
//                  |  ||  | \  / |  |\  \ 
//                  |      |  \/  |  | \  \ 
//                   \_____|      |__|  \__\
//                       Copyright  2001 
//                      Joe Michael Kniss
//                    <<<jmk@cs.utah.edu>>>
//               "All Your Base are Belong to Us"
//
//  nVidia geForce3 volume renderer - 2D textures!
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOLUMERENDERER2_H__EA45FCD6_2CD8_42CF_9FE9_F3543E3500F1__INCLUDED_)
#define AFX_VOLUMERENDERER2_H__EA45FCD6_2CD8_42CF_9FE9_F3543E3500F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "glUE.h"
#include "gluvvPrimitive.h"
#include "gluvv.h"
#include "pbuffer.h"

class NV20VolRen : public gluvvPrimitive 
{
public:
	NV20VolRen();
	virtual ~NV20VolRen();

	void init();
	void draw();

	void drawVolume();
	void drawSlices();
	void interpSlices();

	void create2DTextures(MetaVolume *mv);
	void create2DDepTex();
	void createTempTex();

private:

	void MakeCurrent();
	VolRenMajorAxis  getMajorAxis(GLdouble mv[16]);
	inline void interpSlice(unsigned int tname,     //temperary texture to write to 
		                      int width, int height,  //size of textures
								        	unsigned int atname,    //A texture
									        unsigned int btname,    //B texture
									        float weights[4]);      //compute: AW + (1-W)B ->tname
	void drawClip(float dotv);
	void setupShading();

	void renXPos();
	void renXNeg();
	void renYPos();
	void renYNeg();
	void renZPos();
	void renZNeg();

	void scaleAlpha(float nsr, float osr, unsigned char *dtex);
	void copyScale(float sr, unsigned char *dtex);
	
	void loadDepTex(unsigned char *dtex);

	PBuffer *pbuff;

	unsigned int  *texNames2D;
	unsigned int  *shadeNames;
	unsigned int   deptexName;
	unsigned char *deptex;
	unsigned char *gDeptex;
	unsigned char *iDeptex;
	unsigned int   temptexNames[3]; //one for each of the axies [z-y-x]
		
	int sx, sy, sz;
	float lastSamp;
	float lastGoodSamp;
	float lastInteSamp;

	double mv0[16];

	float  vcoords[8][3];  //vertex coordinates of volume
	float  ccoords[8][3];  //clip space coords

	float  texts[3][2];    //extents for *small volume* and ortho clipping
	float  vexts[3][2]; 
};

#endif // !defined(AFX_VOLUMERENDERER2_H__EA45FCD6_2CD8_42CF_9FE9_F3543E3500F1__INCLUDED_)
