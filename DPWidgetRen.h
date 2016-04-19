//-------------------------------------------------------------------------
//
//   Joe Kniss
//    Data Probe widget
//  DPWidgetRen.h  Querys volume
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


#ifndef __DPWIDGETREN_DOT_H
#define __DPWIDGETREN_DOT_H

#include "gluvvPrimitive.h"
#include "BaseWidget.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "Trackball.h"

struct gluvvGlobal;

class DPWidgetRen : public gluvvPrimitive, public Widget{
 public:
  DPWidgetRen();
  ~DPWidgetRen();

  void draw();
  
  int  pick(int data1, int data2, int data3,
	    float x, float y, float z);
  int  pick();
  int  key(unsigned char key, int x, int y);
  int  mouse(int button, int state,
	     int x, int y);
  int  move(int x, int y);
  int  release();

  int  DPWidgetRenGlErr(char *c);

 private:
  void              update_pos();
  inline void       move_pos();

  enum{          //names
    dpwrUnknown,
      mainbar,
      topball,
      slider1,
      slider2,
      point,
  };

  //gluvvGlobal      *g;
  
  float             pos[3];
  float             length;
  float             sliderpos;

  Trackball         tball;
  
  int               pickedObj;

  int               winpos[2];
  int               lastwinpos[2];

  float             pntfac;

  float             xform[16];
};

#endif
