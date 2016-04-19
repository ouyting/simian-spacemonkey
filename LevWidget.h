//-------------------------------------------------------------------------
//
//   Joe Kniss
//    Levoy Triangle for transfer function widget
//  LevWidget.h  Handles 2&3D transfer functions 
//     3-19-01
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


#ifndef __LEVWIDGET_DOT_H
#define __LEVWIDGET_DOT_H

#include "gluvvPrimitive.h"
#include "BaseWidget.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "HSLPickWidget.h"

#define LEVWIDGNAME 22223222

struct gluvvGlobal;

class LevWidget : public gluvvPrimitive, public Widget{
 public:
  LevWidget(gluvvGlobal *gluvv);
	LevWidget(LevWidget &lw);
  ~LevWidget();

  void draw();

  int  pick(int data1, int data2, int data3,
	    float x, float y, float z);
  int  pick();
  int  mouse(int button, int state,
	     int x, int y);
  int  move(int x, int y);
  int  release();

  void rasterize(unsigned char *tex, unsigned char *auxTex = 0);//set transfer function!!!!!!!!!!!!!!!

  void setBallBar(float ballsize, float barsize);
  void setWH(float width, float height);
  
	//  left +-----o right         left ------o right
	//       |  o  |  <--thresh         \    /
	//       +-----o bottom              \--o thresh
	//                                     o bottom

  void setPos(float bottom[2],  
	      float left[2],         //left pos
	      float right[2],        //right pos
	      float threshl = -10,   //threshold pos left
	      float threshb = -10);  //threshold pos bottom

  void setDepth(float depth);

  int insert(LevWidget *n);
  LevWidget *get(int id);
  LevWidget *remove(int id);

  void setAlpha(float alpha);

  float getBE();
  void  setBE(float boundaryEmphasis);

  void triangleType();
  void squareType();
	void oneDType();

  void alphaMode();
  void colorMode();
  void clearMode();

  int   on;     //rasterize widget
  int   drawOn; //draw widget
  int   id;     //this widgets id/name

 private:
  int  LevWidgetGlErr(char *c);

  void drawTriangle();
  void drawSquare();
  void LWdrawSphere(float pos[3], GLuint name);
  void LWdrawBar(float rot, WdgRotAxis axis,  
		 float len, float pos[3], unsigned int name);
  void LWdrawSlider(float rot, WdgRotAxis axis,
		    float pos[3], unsigned int name);

  enum{
    objUnknown,
      bball,
      lball,
      rball,
      tbar,
      slider,
      trap
  };
  enum{
    modeUnknown,
      LWcolorMode,
      LWscatterMode,
      LWalphaMode,
      LWclearMode
  };
  enum{
    typeUknown,
      LWtriangle,  //levoy triangle
      LWsquare,    //square, actualy ellipse
      LW1d,        //1d tf style
      LWdef,       //default tf style
      LWcircle
  };

  int            mode;
  int            type;

  float          depth;
  float          verts[3][2];
  float          thresh[2];

  float          ballsz;
  float          barsz;
  float          width;
  float          height;

  int            pickedObj;
  int            picked;

  HSLPickWidget  cpick;
  float          color[3];
  float          scatter;
  float          alpha;
  float          be;

  LevWidget     *next;

  int            lastwinpos[2];

};
#endif



