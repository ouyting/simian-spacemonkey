//-------------------------------------------------------------------------
//
//   Joe Kniss
//    Data Probe widget
//  DPWidgetRen.cpp  Probes volumetric data
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

#include <iostream.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#include "VectorMath.h"
#include "gluvv.h"
#include "DPWidgetRen.h"

//================================================= DPWidgetRen()
//===============================================================
DPWidgetRen::DPWidgetRen()
{
  //g = gluvv;
  widgetInit();
  transparent();
  pos[0] = 0.0;
  pos[1] = 0.0;
  pos[2] = 0.0;

  length = .5;
  sliderpos = .8;
	gluvv.probe.slider = sliderpos;

  pntfac = .1;

  pickedObj = 0;

  identityMatrix(xform);

	if(gluvv.mv)
    update_pos();
}

//================================================ ~DPWidgetRen()
//===============================================================
DPWidgetRen::~DPWidgetRen()
{

}

//========================================================== Draw
//===============================================================
void DPWidgetRen::draw()
{
	if(!gluvv.mv) return;

  if(gluvv.picking) fill();

  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  //glDisable(GL_TEXTURE_3D_EXT);

	if(gluvv.reblend == GB_NONE){
		glDisable(GL_BLEND);
		glDisable(GL_LIGHTING);
	} 
	else if(gluvv.reblend == GB_UNDER){
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
		glDepthFunc(GL_EQUAL);
	}


  defaultMaterial();

  glPushMatrix();{
    
    gluvvPushName();
    glPushName(11);
    glPushName(111);
    glPushName(1111);

    move_pos();
    glTranslatef(pos[0], pos[1], pos[2]);
    glMultMatrixf(xform);
    glRotatef(-90, 1, 0, 0);
    //glDrawBuffer(GL_BACK);

    float spos[3] = {0,0,length+length*pntfac};
	
		if(!gluvv.reblend)
			glColor4f(0,0,0,0);
		else
	    glColor4f(.1, .07, .57, 1);                   //balls
    drawSphere(spos, topball);
    float slpos[3] = {0, 0, length*pntfac + sliderpos*(length - ballRad)};
    drawSlider(0, WdgZAxis, slpos, slider1);      //slider
    float bpos[3] = {0, 0, length*pntfac};
		if(!gluvv.reblend)
			glColor4f(0,0,0,0);
		else
	    glColor4f(.09, .34, .55, 1);                  //bar
    drawBar(0, WdgZAxis, length, bpos, mainbar);
    float cpos[3] = {0,0,0};
    drawCone(0, WdgZAxis, length*pntfac, cpos, point);

    //glDrawBuffer(GL_FRONT);
    //glColor4f(.1, .07, .57, 1);                   //ball
    //drawSphere(spos, topball);
    //drawSlider(0, WdgZAxis, slpos, slider1);      //slider
    //glColor4f(.09, .34, .55, 1);                  //bars
    //drawBar(0, WdgZAxis, length, bpos, mainbar);
    //drawCone(0, WdgZAxis, length*pntfac, cpos, point);

    gluvvPopNames();
  } glPopMatrix();

  
	glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);
  DPWidgetRenGlErr("draw");

  if(gluvv.picking) transparent();
}

//========================================================== pick
//===============================================================
int  DPWidgetRen::pick(int data1, int data2, int data3,
		       float x, float y, float z)
{
  fill();
  pickedObj = data3;
  update_pos();
  glutSetWindowTitle("Data Probe Widget");
  gluvv.mprobe = 0;
  return 1;
}

int  DPWidgetRen::pick()
{
  fill();
  pickedObj = gluvv.pick.data3;
  update_pos();
  glutSetWindowTitle("Data Probe Widget");
  gluvv.mprobe = 0;
  return 1;
}

//===================================================================== key
//=========================================================================
int DPWidgetRen::key(unsigned char key, int x, int y)
{
  if(key == 32){ //space bar
    gluvv.tf.paintme = 1;
    glutPostRedisplay();
    return 1; //key used
  }
  return 0;   //key not used
}

//========================================================= mouse
//===============================================================
int  DPWidgetRen::mouse(int button, int state,
			int x, int y)
{
  //cerr << "DPWidget moused" << endl;
  if(state == GLUT_DOWN){
    winpos[0] = lastwinpos[0] = x;
    winpos[1] = lastwinpos[1] = y;
    switch(button){
    case GLUT_LEFT_BUTTON:
      switch(pickedObj){
      case topball:
	tball.start((2.0 * x - gluvv.win.width)  / gluvv.win.width,
		    (2.0 * y - gluvv.win.height) / gluvv.win.height);
      }
      break;
    case GLUT_MIDDLE_BUTTON:
      
      break;
    case GLUT_RIGHT_BUTTON:
      
      break;
    }
    
  }
  else{ //state is _UP
    gluvv.volren.sampleRate = gluvv.volren.goodSamp;
    glutSetWindowTitle("Data Probe Widget");
    glutPostRedisplay();
  }
 
  

  return 1;
}

//========================================================== move
//===============================================================
int  DPWidgetRen::move(int x, int y)
{
  
  float dx = (float)(x - lastwinpos[0])/(float)gluvv.win.width;
  float dy = (float)(y - lastwinpos[1])/(float)gluvv.win.height;
  float d[3];
  subV3(d, pos, gluvv.env.eye);
  float dist = normV3(d);
  dx *= (gluvv.env.frustum[1]-gluvv.env.frustum[0]) * dist/gluvv.env.clip[0];
  dy *= (gluvv.env.frustum[3]-gluvv.env.frustum[2]) * dist/gluvv.env.clip[0];

  if((gluvv.mouse.state == GLUT_DOWN) &&             //left 
     (gluvv.mouse.button == GLUT_LEFT_BUTTON) &&     //down
     (pickedObj == topball)){                        //topball
    gluvv.volren.sampleRate = gluvv.volren.interactSamp;
    tball.update((2.0 * x - gluvv.win.width) /gluvv.win.width,
		 (2.0 * y - gluvv.win.height) /gluvv.win.height);
    tball.buildRotMatrix((float (*)[4])xform);
    glutPostRedisplay();
  }
  else if((gluvv.mouse.state == GLUT_DOWN) &&        //left 
     (gluvv.mouse.button == GLUT_LEFT_BUTTON) &&     //down
     (pickedObj == slider1)){                        //slider1
    sliderpos += ((float)(x - lastwinpos[0])/(float)gluvv.win.width -
		  (float)(y - lastwinpos[1])/(float)gluvv.win.height);
    sliderpos = (sliderpos > 1) ? 1 : ((sliderpos < 0) ? 0 : sliderpos);
		gluvv.probe.slider = sliderpos;
		gluvv.tf.loadme = 1;
    gluvv.volren.sampleRate = gluvv.volren.interactSamp;
    glutPostRedisplay();    
  }
  else if((gluvv.mouse.state == GLUT_DOWN) &&        //left
	  (gluvv.mouse.button == GLUT_LEFT_BUTTON)){ //down
    pos[0] -= dx;                                    //any
    pos[1] -= dy;
    gluvv.volren.sampleRate = gluvv.volren.interactSamp;
    glutPostRedisplay();
  }
  else if((gluvv.mouse.state == GLUT_DOWN) &&         //middle
	  (gluvv.mouse.button == GLUT_MIDDLE_BUTTON)){//down
    pos[0] -= dx;                                     //any
    pos[2] += dy;
    gluvv.volren.sampleRate = gluvv.volren.interactSamp;
    glutPostRedisplay();
  }
  else if((gluvv.mouse.state == GLUT_DOWN) &&         //right
	  (gluvv.mouse.button == GLUT_RIGHT_BUTTON)){ //down
    pos[2] -= dx;                                     //any
    pos[1] -= dy;
    gluvv.volren.sampleRate = gluvv.volren.interactSamp;
    glutPostRedisplay();
  }

  update_pos();

  lastwinpos[0] = x;
  lastwinpos[1] = y;
  
  return 1;
}

//======================================================= Release
//===============================================================
int  DPWidgetRen::release()
{
  transparent();
  //cerr << "DPWidget released" << endl;
  return 0;
}

void DPWidgetRen::update_pos()
{
  //move the point tip to volume space
  float  m1[16];
  identityMatrix(m1);
  m1[12] = gluvv.rinfo.trans[0];
  m1[13] = gluvv.rinfo.trans[1];
  m1[14] = gluvv.rinfo.trans[2];
  float m2[16];
  matrixMult(m2, m1, gluvv.rinfo.xform);
  float scale[16];
  identityMatrix(scale);
  scale[0] = gluvv.rinfo.scale;
  scale[5] = gluvv.rinfo.scale;
  scale[10]= gluvv.rinfo.scale;
  float m3[16];
  matrixMult(m3, m2, scale);
  float m4[16];
  identityMatrix(m4);
  m4[12] = -.5 * gluvv.mv->xfSize;
  m4[13] = -.5 * gluvv.mv->yfSize;
  m4[14] = -.5 * gluvv.mv->zfSize;
  float m5[16];
  matrixMult(m5, m3, m4);
  float m6[16] = {gluvv.mv->xfSize, 0,              0,              0,
		  0,              gluvv.mv->yfSize, 0,              0,
		  0,              0,              gluvv.mv->zfSize, 0,
		  0,              0,              0,              1};
  float volm[16];
  matrixMult(volm, m5, m6);
  float invVolm[16];
  inverseMatrix(invVolm, volm);

  float vpos[3];
  translateV3W(vpos, invVolm, pos);
  gluvv.probe.vpos[0] = vpos[0];
  gluvv.probe.vpos[1] = vpos[1];
  gluvv.probe.vpos[2] = vpos[2];
  //cerr << "pos = " << vpos[0] << " " << vpos[1] << " " << vpos[2] << endl;

}

inline
void DPWidgetRen::move_pos()
{
  float  m1[16];
  identityMatrix(m1);
  m1[12] = gluvv.rinfo.trans[0];
  m1[13] = gluvv.rinfo.trans[1];
  m1[14] = gluvv.rinfo.trans[2];
  float m2[16];
  matrixMult(m2, m1, gluvv.rinfo.xform);
  float scale[16];
  identityMatrix(scale);
  scale[0] = gluvv.rinfo.scale;
  scale[5] = gluvv.rinfo.scale;
  scale[10]= gluvv.rinfo.scale;
  float m3[16];
  matrixMult(m3, m2, scale);
  float m4[16];
  identityMatrix(m4);
  m4[12] = -.5*gluvv.mv->xfSize;
  m4[13] = -.5*gluvv.mv->yfSize;
  m4[14] = -.5*gluvv.mv->zfSize;
  float m5[16];
  matrixMult(m5, m3, m4);
  float m6[16] = {gluvv.mv->xfSize, 0,              0,              0,
		              0,              gluvv.mv->yfSize, 0,              0,
		              0,              0,              gluvv.mv->zfSize, 0,
		              0,              0,              0,              1};
  float volm[16];
  matrixMult(volm, m5, m6);
  translateV3W(pos, volm, gluvv.probe.vpos);

}

//============================================== DPWidgetRenGlErr
//===============================================================
int DPWidgetRen::DPWidgetRenGlErr(char *place)
{
  GLenum errCode;
  const GLubyte *errString;
  
  if((errCode = glGetError()) != GL_NO_ERROR){
    errString = gluErrorString(errCode);
    cerr << endl << "OpenGL error : DPWidgetRen::" << place << " : " 
	 << (char *) errString << endl << endl;
    return 1;
  }
  return 0;
}


