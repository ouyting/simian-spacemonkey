//-------------------------------------------------------------------------
//
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


#ifndef __TFWIDGETREN_DOT_H
#define __TFWIDGETREN_DOT_H

#include "gluvvPrimitive.h"
#include "BaseWidget.h"
#include "LevWidget.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "Trackball.h"

struct gluvvGlobal;
extern unsigned char don[69]; // "don greenburg" color bind color map 23*(rgb)
                              // the don[0] value is black, for the background
                              // don[1]=blue, don[11,12]=white, don[22]=orange

class TFWidgetRen : public gluvvPrimitive, public Widget{
 public:
  TFWidgetRen(gluvvGlobal *gluvv);
  ~TFWidgetRen();

  void draw();

  int  pick(int data1, int data2, int data3,
	    float x, float y, float z);
  int  pick();
  int  key(unsigned char key, int x, int y);
  int  mouse(int button, int state,
	     int x, int y);
  int  move(int x, int y);
  int  release();

  int  TFWidgetRenGlErr(char *c);


 private:
  void draw_widget();
  void draw_probe();
  void draw_mouseprobe();
  void mouse2plane(int x, int y);
  void clearPtex();
  void clearPaintex();
  void loadPtex();
  void triLerpVGH(float val[3], float voxels[8][3], float pos[3]); 

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
      beslider
  };

  enum{           //modes
    TFmodeUnknown,
      color,
      colorBlend,
      colorBlendPaintS,   //blended paint square..
      levoy
  };

  float   frust[4];     // frustum, local copy of information

  float   width;        //  width of widget (world space)
  float   screenwidth;  // percent of screen width realastate [0..1]
  float   height;       //  height of widget (world space)
  float   screenheight; // percent of screen height realastate [0..1]
  
  float   screenball;   // size of ball/bar in screen space
  float   screenbar;

  float   pos[3];       // at lower left hand corner
  float   screenpos[2]; // position in screen space [0..1]
  float   mousepnt[3];  // position of mouse click on tf plane

  int     pickedObj;    // which sub object is selected
  int     pickedTri;    // if a tri was picked, which one
  int     mode;         //  what mode is the transfer fuction? see mode enum
  int     lastwinpos[2];//  the last mouse position in screen space (int)
  float   lastppos[3];  // the last position of probe

  LevWidget *tris;      // levoy triangle widgets
  LevWidget *brush;     // levoy square brush
  float lwbs;           // levoy widget ball size
  
};

#endif
