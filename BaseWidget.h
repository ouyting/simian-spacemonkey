//------------------------------------------------------------------------
//   BaseWidget.h
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

#ifndef __BASE_WIDGET_H
#define __BASE_WIDGET_H

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#ifndef WIN32
#include <X11/keysym.h>
#endif

//=========================================================================
// Please READ:
//Rules for Picking Widgets: (in your (widget's) draw function)
//  First(1) push 'I_AM_A_WIDGET' to indecate what you are.
//  Next(2)  push your 'this' pointer, (1&2 are coverd by 'pushName()') 
//  Finaly(3) push two integers.
//  note: for 64 bit builds, the 'this' pointer is 2 integers. 
//  your pickcb will be called first, if it returns non zero
//    mouse and keyboard state should be sent to you via respective
//    callbacks
//  If the app takes back controll of events (un-picks you), your
//    release() function will be called
//  These rules only work if the event manager respects them.
//  see the Widget::pushName() and Widget::popNames() functions below
//=========================================================================

//---- Widget names --------------------------------------------------
// names to push during a pick, part one (1) above
enum{
   I_AM_A_WIDGET        = 0x98765432,
   I_AM_A_WIDGET_GROUP  = 0x98765111,
   EMPTY_WIDGET_UINT    = 0x12345678
};

//---- Mouse State enums ---------------------------------------------
// mouse button enums for callbacks
typedef enum{
   WdgMouseUnknown,
   WdgM_BLeft,
   WdgM_BMiddle,
   WdgM_BRight
} WdgMouseButton;

typedef enum{
   WdgM_SUnknown,
   WdgM_SUp,
   WdgM_SDown
} WdgMouseState;

//---- axis enums for default glyph renderers ------------------------
typedef enum{
   WdgAxisUnknown,
   WdgXAxis,
   WdgYAxis,
   WdgZAxis
} WdgRotAxis;

//============================================================================
//------- THE WIDGET !!! -----------------------------------------------------
//============================================================================

class Widget
{

public:
   Widget();
                                     //timing is everyting! 
                                     //(after OpenGL has been inited)
   void widgetInit(); 
                                     //Memory leaks are bad!!
   void widgetFree(); 
   ~Widget(){}
  
   //-----------------------------------------------------------------
   //callbacks should return 0 when the object wants to be released---

                                     //draw function should be over 
                                     // ridden for any widget, since it
                                     // will be drawn independant of its
                                     // pick status, a return value is
                                     // not necessary.
   virtual void  draw();
                                     //this object has been picked at
                                     // {x,y,z}, if z = 0, picked in
                                     //  screen space, needs transformation
   virtual int   pickcb(int data1,       // this is the data pushed on
			int data2,       // after your name in a pick
			float x, float y, float z = 0);
                                     //this object has moved to
                                     // {x,y,z}, if z = 0, move happened
                                     //  in screen space 
   virtual int   movecb(float x, float y, float z = 0);
                                     //standard keyboard press, send key
                                     // only if the object has been selected
   virtual int   keycb(char key);
                                     //X windows keyboard callback, send key
                                     //  only if the object has been selected
   virtual int   Xkeycb(unsigned int ks);
                                     //this object has been selected by the
                                     // mouse with buttion, and click state
                                     //  see enums above
   virtual int   mousecb(WdgMouseButton button, 
			 WdgMouseState state, 
			 float x, float y, float z);
                                     //widgets need to know the size of the
                                     // view port if the mouse is being used
   virtual void  resize(int x, int y);
                                     //The application is releasing the 
                                     //widget, it is no longer picked
   virtual int   releasecb();

   //end callbacks----------------------------------------------------
   //-----------------------------------------------------------------

   //-----------------------------------------------------------------
   //--------General utils -------------------------------------------
                                     //set the position of the widget 
                                     //in world space
   inline void   SetPos(float x, float y, float z);
                                     //get the position of the widget
   inline float *GetPos();
   
                                     //set the transform of the widget
   inline void   SetXform(GLfloat Mat[16],     //4x4 transformation matrix
			  GLfloat Trans[3],    //translation can be {0,0,0}
			  GLfloat Scale = 1.0);//can set a global scale   

                                     //standard material for widgets
                                     // sets color, specular, diffuse etc..
   inline void defaultMaterial();    
   inline void ident(GLfloat m[16]); //matrix identity function (vectorMath.h)

                                     //pushes your TYPE & 'this' pointer 
                                     //for you, don't forget to push two more
                                     //integer values on the stack as well
   inline void pushName();       
                                     //pops ALL of the required names 
                                     //off the stack, including 2 datas
   inline void popNames();       
   
                                     //this should move to ::TWidget
   //inline void setName(WidgetGroup *n){ name = n; }

                                     //set clipping planes based on bounds 
                                     //-- should this move too?? 
   inline void setClips(GLfloat m[16], GLfloat trans[3], GLfloat scale);
                                     //unset ALL clip planes
   inline void killClips();
   
   //---- draw some standard Widget parts -----------------------------
   inline void drawBar(float rot, WdgRotAxis axis,  
		       float len, float pos[3], unsigned int name);
   inline void drawSphere(float pos[3], unsigned int name);

   inline void drawCone(float rot, WdgRotAxis axis, float len, 
			float pos[3], unsigned int name);

   inline void drawSlider(float rot, WdgRotAxis axis, 
			  float pos[3], unsigned int name);

   //---- draw styles -------------------------------------------------
   void        transparent(); // realy just silhoette style
   void        fill();

   //cheap and dirty mutex, realy needs to be fixed!
   bool            busy;

protected:
   //WidgetGroup    *name;

   //--- global widget values -----------
   GLfloat         position[3];
   GLfloat         orient[16];

   //--- global widget display properties
   GLUquadricObj  *qobj;
   float           barRad;
   float           barSlice, barStack;
   float           ballRad;
   float           ballSlice;
   float           coneRad; //NOTE: cone uses barSlice & barStacks rendering

   //--- environment variables ----------
   int             winWidth;
   int             winHeight;
   GLfloat         xform[16];
   GLfloat         trans[3];
   GLfloat         scale;

   //--- global widget bounds ----------- 
   //assume axis aligned for now! for cliping to volumes
   float           LLbound[3];  //Lower Left Bound  [0,0,0]
   float           URbound[3];  //Upper right Bound [1,1,1] 
   float           maxBound[3]; //global Upper Right bound 
   float           minBound[3]; //global Lower Left bound

   //--- selection stuff ----------------
   int             pickedObj;
   float           lastPos[3];
   WdgMouseButton  but;
   WdgMouseState   m_state;

private:


};


//=============================================================================
//-------- inline methods -----------------------------------------------------
//=============================================================================

//--------------------------------------------------------------------------
inline void Widget::SetPos(float x, float y, float z)
{
  position[0] = x; position[1] = y; position[2] = z; 
}

//--------------------------------------------------------------------------
inline float *Widget::GetPos()
{ 
  return position;
}

//--------------------------------------------------------------------------
//set the transform of the widget
inline void Widget::SetXform(GLfloat Mat[16], GLfloat Trans[3], GLfloat Scale)
{
  for(int i=0; i<16; ++i) xform[i] = Mat[i];
  for(i=0; i<3; ++i) trans[i] = Trans[i];
  scale = Scale;
}

//--------------------------------------------------------------------------
inline void Widget::defaultMaterial()
{
   GLfloat wamb[] = { (float)0.3, (float)0.3, (float)0.3, (float)1.0 };
   GLfloat wspec[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat wdiff[] = { 0.5, 0.5, 0.5, 1.0 };
   GLfloat wshine[] = { 50.0 };
   
   glEnable(GL_COLOR_MATERIAL);
   glMaterialfv(GL_FRONT, GL_AMBIENT, wamb);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, wdiff);
   glMaterialfv(GL_FRONT, GL_SPECULAR, wspec);
   glMaterialfv(GL_FRONT, GL_SHININESS, wshine);

   glColor4f(1,1,1,1);
}

//--------------------------------------------------------------------------
inline void Widget::ident(GLfloat m[16])
{
   m[0]=1; m[4]=0; m[8]= 0; m[12]=0;
   m[1]=0; m[5]=1; m[9]= 0; m[13]=0;
   m[2]=0; m[6]=0; m[10]=1; m[14]=0;
   m[3]=0; m[7]=0; m[11]=0; m[15]=1;
}
//--------------------------------------------------------------------------
inline void Widget::pushName()
{
  //if(name)
  //  glPushName(I_AM_A_WIDGET_GROUP);
  //else
  glPushName(I_AM_A_WIDGET);

#if (_MIPS_SZPTR == 64) //64 Bit
   unsigned long o;

   //if(name) o=(unsigned long)name;
   //else     
   o=(unsigned long)this;

   unsigned int o1=(o>>32)&0xffffffff;
   unsigned int o2=o&0xffffffff;
   glPushName(o1);
   glPushName(o2);

#else //32 bit
   //   if(name)
   // glPushName((GLuint)name);
   //else 
      glPushName((GLuint)this);
#endif

}
//--------------------------------------------------------------------------
inline void Widget::popNames()
{
#if (_MIPS_SZPTR == 64) //64 Bit
   glPopName();
#endif
   glPopName();
   glPopName();
   glPopName();
   glPopName();
}
//--------------------------------------------------------------------------
inline void Widget::drawBar(float rot, WdgRotAxis axis,  
			    float len, float pos[3], unsigned int name)
{
   glPushMatrix(); { //bottom front bar
      glLoadName((GLuint) name);
      glTranslatef(pos[0], pos[1], pos[2]);
      
      if(axis == WdgXAxis) glRotatef(rot, 1, 0, 0);
      else if(axis == WdgYAxis) glRotatef(rot, 0, 1, 0);
      else glRotatef(rot, 0, 0, 1);
      
      gluCylinder(qobj, barRad, barRad, len, (int)barSlice, (int)barStack);
   } 
   glPopMatrix();
}
//--------------------------------------------------------------------------
inline void Widget::drawSphere(float pos[3], unsigned int name)
{
   glPushMatrix();{
      glLoadName((GLuint) name);
      glTranslatef(pos[0], pos[1], pos[2]);
      
      gluSphere(qobj, ballRad, (int)ballSlice, (int)ballSlice);
   } 
   glPopMatrix();
}
//--------------------------------------------------------------------------
inline void Widget::drawCone(float rot, WdgRotAxis axis,
			     float len, float pos[3], unsigned int name)
{
   glPushMatrix();{
      glLoadName((GLuint) name);
      glTranslatef(pos[0], pos[1], pos[2]);

      if(axis == WdgXAxis) glRotatef(rot, 1, 0, 0);
      else if(axis == WdgYAxis) glRotatef(rot, 0, 1, 0);
      else glRotatef(rot, 0, 0, 1);

      gluCylinder(qobj, 0, coneRad, len, (int)barSlice, (int)barStack);
   }
   glPopMatrix();
}
//--------------------------------------------------------------------------
inline void Widget::drawSlider(float rot, WdgRotAxis axis,
			       float pos[3], unsigned int name)
{
   glPushMatrix();{
      glLoadName((GLuint) name);
      glTranslatef(pos[0], pos[1], pos[2]);
      
      if(axis == WdgXAxis) glRotatef(rot, 1, 0, 0);
      else if(axis == WdgYAxis) glRotatef(rot, 0, 1, 0);
      else glRotatef(rot, 0, 0, 1);

      gluCylinder(qobj, ballRad*1.1, ballRad*1.1, ballRad*1.1, (int)barSlice, 2);
      gluDisk(qobj, 0, ballRad*1.1, (int)barSlice, 1);
      glTranslatef((float)0, (float)0, (float)(ballRad*1.1));
      gluDisk(qobj, 0, ballRad*1.1, (int)barSlice, 1);
   }
   glPopMatrix();
}
//--------------------------------------------------------------------------
inline void Widget::setClips(GLfloat m[16], GLfloat trans[3], GLfloat scale)
{   
   //this will set clipping planes based on the upper right and lower left
   // bounds.  if the bounds of a sub regon are inside the max and min
   // bounds the geometry will be clipped, other wise not.  This is useful
   // for parallel compositing.  This widget may only belong to a small sub-
   // region, it should be clipped where it abutts other sub-regions
   GLdouble yup[4] = { 0, 1, 0,0};
   GLdouble ydn[4] = { 0,-1, 0,0};
   GLdouble xup[4] = { 1, 0, 0,0};
   GLdouble xdn[4] = {-1, 0, 0,0};
   GLdouble zup[4] = { 0, 0, 1,0};
   GLdouble zdn[4] = { 0, 0,-1,0};

   glPushMatrix();{
      glTranslatef(trans[0], trans[1], trans[2]);
      glMultMatrixf(m);
      glScalef(scale, scale, scale);
      glTranslatef(-.5, -.5, -.5 );//querk of the data representation

      if(LLbound[0] > minBound[0]){
	 glEnable(GL_CLIP_PLANE1);
	 glPushMatrix();{
	    glTranslatef(LLbound[0], 0, 0);
	    glClipPlane(GL_CLIP_PLANE1, xup);
	 };
	 glPopMatrix();
      }

      if(URbound[0] < maxBound[0]){
	 glEnable(GL_CLIP_PLANE2);
	 glPushMatrix();{
	    glTranslatef(URbound[0], 0, 0);
	    glClipPlane(GL_CLIP_PLANE2, xdn);
	 };
	 glPopMatrix();
      }

      if(LLbound[1] > minBound[1]){
	 glEnable(GL_CLIP_PLANE3);
	 glPushMatrix();{
	    glTranslatef(0, LLbound[1], 0);
	    glClipPlane(GL_CLIP_PLANE3, yup);
	 };
	 glPopMatrix();
      }

      if(URbound[1] < maxBound[1]){
	 glEnable(GL_CLIP_PLANE4);
	 glPushMatrix();{
	    glTranslatef(0, URbound[1], 0);
	    glClipPlane(GL_CLIP_PLANE4, ydn);
	 };
	 glPopMatrix();
      }

      if(LLbound[2] > minBound[2]){
	 glEnable(GL_CLIP_PLANE5);
	 glPushMatrix();{
	    glTranslatef(0, 0, LLbound[2]);
	    glClipPlane(GL_CLIP_PLANE5, zup);
	 };
	 glPopMatrix();
      }

      if(URbound[2] < maxBound[2]){
	 glEnable(GL_CLIP_PLANE0);
	 glPushMatrix();{
	    glTranslatef(0, 0, URbound[2]);
	    glClipPlane(GL_CLIP_PLANE0, zdn);
	 };
	 glPopMatrix();
      }

   }
   glPopMatrix();
}
//--------------------------------------------------------------------------
inline void Widget::killClips()
{
   glDisable(GL_CLIP_PLANE1);
   glDisable(GL_CLIP_PLANE2);
   glDisable(GL_CLIP_PLANE3);
   glDisable(GL_CLIP_PLANE4);
   glDisable(GL_CLIP_PLANE5);
   glDisable(GL_CLIP_PLANE0);
}




#endif






