//   Joe Kniss
//    Transfer Function widget
//  TFWidgetRen.h  Handles 2&3D transfer functions 
//     3-14-01
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
// TFWidgetRen1.h: interface for the TFWidgetRen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TFWIDGETREN1_H__8A510952_00E9_46DD_A272_3380AA75370D__INCLUDED_)
#define AFX_TFWIDGETREN1_H__8A510952_00E9_46DD_A272_3380AA75370D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseWidget.h"
#include "gluvvPrimitive.h"
#include "LevWidget.h"


extern unsigned char don[69]; // "don greenburg" color bind color map 23*(rgb)
                              // the don[0] value is black, for the background
                              // don[1]=blue, don[11,12]=white, don[22]=orange

class TFWidgetRen : 
	public Widget, 
	public gluvvPrimitive  
{
public:
	TFWidgetRen();
	virtual ~TFWidgetRen();

	void draw();
	void init();

  int  pick(int data1, int data2, int data3,
	          float x, float y, float z);
  int  pick();
  int  key(unsigned char key, int x, int y);
  int  mouse(int button, int state,
	           int x, int y);
  int  move(int x, int y);
  int  release();

private:
	void  drawFrame();
	void  drawProbe();
	void  mouse2plane(int x, int y);
	void  clearPtex();
	void  clearPtex(unsigned char *ptex);
	void  copyPtex(unsigned char *ptexout, unsigned char *ptexin);
	void  rasterizevgH(unsigned char *ptex); //rasterize the H portion of vgh ptex
	void  loadPtex();
	void  loadHist2D();

	void  triLerpV3(float val[3], float voxels[8][3], float pos[3]);

	enum{            //subobject names
    TFobjUnknown,
      tlball,      //top left ball
      trball,      //top right ball
      blball,      //bottom left ball
      brball,      //bottom right ball
      lbar,        //left bar
      rbar,        //right bar
      tbar,        //top bar
      bbar,        //bottom bar
      tfsheet0,    //transfer function sheet 0
      tfsheet1,
      tfsheet2,
      dpoint,       //data point from pick (probe or clip click)
			gslide,       //good sample rate slider
			islide,       //interactive sample rate slider
			aslide,       //alpha scale slider (gamma)
			a3slide,      //3rd axis emphasis slider
			a3slidehi,    //ditto
			a3slidelo,    //ditto
      beslider
  };

	float width, height;
	float screenwidth, screenheight;
	float screenpos[2];
	float screenbar, screenball;
	float   mousepnt[3];  // position of mouse click on tf plane
	float   lastppos[3];  // last probe position in volume

	int   lastwinpos[2];

	float lastSlide1Pos[2]; //one for hi and lo

	LevWidget *tris;      // levoy widgets
	LevWidget *shadTris;  // levoy widget for a shadow transfer function
  int        pickedTri; //
	LevWidget *brush;     // levoy square brush
	float lwbs;           // levoy widget ball size
	
	unsigned char *paintex; //the painted transfer function

  unsigned char *histo;        //histogram of the volume
	unsigned int   histName;     //texture name of the histogram

};

#endif // !defined(AFX_TFWIDGETREN1_H__8A510952_00E9_46DD_A272_3380AA75370D__INCLUDED_)
