//------------------------------------------------------------------------
//
//   Joe Kniss
//    A glut implementation of the Volume Renderer
//  gluvvPrimitive.cpp  virual class for rendering and widgets
//     3-4-01
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

#include <string.h>
#include "gluvvPrimitive.h"
#ifdef WIN32
#include <windows.h>		//  must be included before any OpenGL
#endif
#include <GL/glu.h>
#include <iostream.h>

//========================================== gluvvPrimitive
//=========================================================
gluvvPrimitive::gluvvPrimitive()
{
  next = 0;
  name = 0;
}

//========================================= ~gluvvPrimitive
//=========================================================
gluvvPrimitive::~gluvvPrimitive()
{

}

//========================================= init
//==============================================
void gluvvPrimitive::init()
{

}

//========================================= draw
//==============================================
void gluvvPrimitive::draw()
{
}

//========================================= key
//=============================================
 
int gluvvPrimitive::key(unsigned char k, int x, int y)
{
  return 0;
}

int gluvvPrimitive::special(int k, int x, int y)
{
	return 0;
}


//============================================ pick
//=================================================
int gluvvPrimitive::pick(int data1, int data2, int data3,
			 float x, float y, float z)
{
  return 1;
}

int gluvvPrimitive::pick()
{
  return 1;
}

//=========================================== mouse
//=================================================
int gluvvPrimitive::mouse(int button, int state, 
			  int x, int y)
{
  return 1;
}

//============================================ move
//=================================================
int gluvvPrimitive::move(int x, int y)
{
  return 1;
}

//========================================= release
//=================================================
int gluvvPrimitive::release()
{
  return 0;
}

//========================================= setNext
//=================================================
void gluvvPrimitive::setNext(gluvvPrimitive *p)
{
  p->next = next;
  next = p;
}

//========================================= getNext
//================================================= 
gluvvPrimitive *gluvvPrimitive::getNext()
{
  return next;
}

//========================================= remove
//================================================
void gluvvPrimitive::remove(gluvvPrimitive *p)
{
  if(next == p)
    next = p->next;
  else 
    next->remove(p);
}
 
//========================================= setName
//================================================= 
void gluvvPrimitive::setName(const char *PName)
{
  name = strdup(PName);
}

//======================================== pushName
//================================================= 
void gluvvPrimitive::gluvvPushName()
{
#if (_MIPS_SZPTR == 64) //64 Bit
  unsigned long o;
  o=(unsigned long)this;
  unsigned int o1=(o>>32)&0xffffffff;
  unsigned int o2=o&0xffffffff;
  glPushName(o1);
  glPushName(o2);
#else                   //32 bit
  glPushName((GLuint)this);
#endif
}       

//======================================== popNames
//================================================= 
void gluvvPrimitive::gluvvPopNames()
{
#if (_MIPS_SZPTR == 64) //64 Bit
   glPopName();
#endif
   glPopName();
   glPopName();
   glPopName();
   glPopName();
}

