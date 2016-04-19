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
//
// LTWidgetRen.h: interface for the LTWidgetRen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LTWIDGETREN_H__5D197401_527C_4120_92A7_468EFD534CE9__INCLUDED_)
#define AFX_LTWIDGETREN_H__5D197401_527C_4120_92A7_468EFD534CE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseWidget.h"
#include "gluvvPrimitive.h"
#include "Trackball.h"

class LTWidgetRen : 
	public gluvvPrimitive, 
	public Widget  
{
public:
	LTWidgetRen();
	virtual ~LTWidgetRen();

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

	int  onoff(); // toggle this widget on and off
	void  genXForm();

private:
	float screenpos[2];
	float screenrad;

	float position[3];
	float lastpos[3];

	Trackball tball;

	int lastwinpos[2];

	int drawOn;
};

#endif // !defined(AFX_LTWIDGETREN_H__5D197401_527C_4120_92A7_468EFD534CE9__INCLUDED_)
