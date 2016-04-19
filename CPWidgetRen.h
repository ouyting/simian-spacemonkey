//-------------------------------------------------------------------------
//
//   Joe Kniss
//    Cliping Plane widget
//  CPWidgetRen.h  Handles an arbitrary Clipping Plane 
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


#ifndef __CPWIDGETREN_DOT_H
#define __CPWIDGETREN_DOT_H

#include "gluvvPrimitive.h"
#include "BaseWidget.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "Trackball.h"

struct gluvvGlobal;

class CPWidgetRen : public gluvvPrimitive, public Widget
{
 public:
  CPWidgetRen();
  ~CPWidgetRen();

  void draw();

  
  int  pick(int data1, int data2, int data3,
	    float x, float y, float z);
  int  pick();
  int  key(unsigned char key, int x, int y);
  int  mouse(int button, int state,
	     int x, int y);
  int  move(int x, int y);
  int  release();

  int  CPWidgetRenGlErr(char *c);


 private:
  void draw_widget();
  void set_info();
	void update_pos();
  void mouse2plane(int x, int y);
  
  enum{
    cpObjectUnknown,
      cpbar,
      cpball,
      alphaslide,
      plane,
  };

  float pos[3];
	float vpos[3];
  float xform[16];

  float mousepnt[3];

  int lastwinpos[3];
  int pickedObj;

  float width;
  float screenwidth;
  float height;
  float screenheight;

  float screenbar;
  float screenball;

  float aslider;

  Trackball tball;

};

#endif
