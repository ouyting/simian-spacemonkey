//------------------------------------------------------------------------
//   BaseWidget.cpp
//   Joe Kniss
//    base class for direct manipulation widgets
//     2-27-01
//                   ________    ____   ___ 
//                  |        \  /    | /  /
//                  +---+     \/     |/  /
//                  +--+|  |\    /|     < 
//                  |  ||  | \  / |  |\  \ 
//                  |      |  \/  |  | \  \ 
//                   \_____|      |__|  \__\
//                       Copyright  2001 
//                      Joe Michael Kniss
//-------------------------------------------------------------------------

#include "BaseWidget.h"

//
//=======================================================================
//------ Widget Standard constructior -----------------------------------
//=======================================================================
//


Widget::Widget()
{
  //name = 0;
   busy = false;
}

//=======================================================================
//------ Initialize the basics ------------------------------------------

void Widget::widgetInit()
{
   barRad = (float).015;
   barSlice = 10;
   barStack = 2;
   ballRad = (float)(barRad * 1.50);
   ballSlice = 10;

   coneRad = barRad;

   qobj = gluNewQuadric();
   gluQuadricDrawStyle(qobj, GLU_FILL);
   gluQuadricNormals(qobj, GLU_SMOOTH);
}

//=======================================================================
//------ Free the basics ------------------------------------------------

void Widget::widgetFree()
{
   gluDeleteQuadric(qobj);
}

//=======================================================================
//------ Default draw func ----------------------------------------------

void Widget::draw()
{
  /*I don't do anything, yet*/
}

//=======================================================================
//----- callbacks should return 0 when the object wants to be released---
//-----------------------------------------------------------------------
int Widget::pickcb(int data1, 
		   int data2, 
		   float x, float y, float z)
{
  return 0;
}
//-----------------------------------------------------------------------
int Widget::movecb(float x, float y, float z)
{
  return 1;
}
//-----------------------------------------------------------------------
int Widget::keycb(char key)
{
  return 1;
}
//-----------------------------------------------------------------------
int Widget::Xkeycb(unsigned int ks)
{
  return 1;
}
//-----------------------------------------------------------------------
int Widget::mousecb(WdgMouseButton button, 
		    WdgMouseState state, 
		    float x, float y, float z)
{
  return 1;
}
//-----------------------------------------------------------------------
void Widget::resize(int x, int y)
{
  winWidth = x; 
  winHeight = y;
}
//-----------------------------------------------------------------------
int Widget::releasecb()
{
  return 0;
}
//-----------------------------------------------------------------------
void Widget::transparent()
{
  gluQuadricDrawStyle(qobj, GLU_SILHOUETTE);
  ballSlice = 5;
  barSlice = 3;
}
//-----------------------------------------------------------------------
void Widget::fill()
{
  gluQuadricDrawStyle(qobj, GLU_FILL);
  ballSlice = 10;
  barSlice = 10;

}
