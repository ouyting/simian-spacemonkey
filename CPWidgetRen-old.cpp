//-------------------------------------------------------------------------
//
//   Joe Kniss
//    Cliping Plane widget
//  CPWidgetRen.cpp  Handles an arbitrary Clipping Plane 
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


#ifdef WIN32
#include <windows.h>
#endif
#include <gl/glut.h>
#include "CPWidgetRen.h"
#include "gluvv.h"
#include "VectorMath.h"
#include <stdio.h>

//============================================================= CPWidgetRen
//=========================================================================
CPWidgetRen::CPWidgetRen()
{

  vpos[0] = .5;
  vpos[1] = .5;
  vpos[2] = .5;
  
	pos[0] = 0;
	pos[1] = 0;
	pos[2] = 0;
  
	identityMatrix(xform);
  matrixEqual(gluvv.clip.xform, xform);

  screenwidth  = .5;
  screenheight = .5;

  float d[3];
  subV3(d, pos, gluvv.env.eye);
  float dist = normV3(d);
  width  = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	    dist/gluvv.env.clip[0]) * screenwidth;
  height = ((gluvv.env.frustum[3]-gluvv.env.frustum[2]) * 
	    dist/gluvv.env.clip[0]) * screenheight;

  screenbar  = .01;
  screenball = screenbar*1.5;

  glEnable(GL_DEPTH_TEST);
  widgetInit();
  transparent();

  aslider = .3;
  gluvv.clip.pname = plane;

	if(gluvv.mv) set_info();
}

//============================================================ ~CPWidgetRen
//=========================================================================
CPWidgetRen::~CPWidgetRen()
{

}

//==================================================================== draw
//=========================================================================
void CPWidgetRen::draw()
{
	if(!gluvv.mv) return;
	
	update_pos();
	set_info();

  float dist = pos[2] - gluvv.env.eye[2];
  
  width  = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	    dist/gluvv.env.clip[0]) * screenwidth;
  height = ((gluvv.env.frustum[3]-gluvv.env.frustum[2]) * 
	    dist/gluvv.env.clip[0]) * screenheight;

  barRad = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
  	    dist/gluvv.env.clip[0]) * screenbar;

  ballRad = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	     dist/gluvv.env.clip[0]) * screenball;

  //glDisable(GL_TEXTURE_3D_EXT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);

  defaultMaterial();
  gluvvPushName();
  glPushName(22);
  glPushName(222);
  glPushName(2222);


  glPushMatrix(); {
    glTranslatef(pos[0], pos[1], pos[2]);
    glMultMatrixf(xform);
    glTranslatef(-width/2.0, -height/2.0, 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    
    //glDrawBuffer(GL_BACK);
    draw_widget();
    
    //glDrawBuffer(GL_FRONT);
    //draw_widget();
  } glPopMatrix();

  if(pickedObj == plane){
    glPushMatrix();{
      glColor4f(0,1,0,1);
      ballRad = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
				dist/gluvv.env.clip[0]) * screenball/5;
      drawSphere(mousepnt, plane);                     //pick point
      glTranslatef(mousepnt[0], mousepnt[1], mousepnt[2]);
      glColor4f(1,1,1,1);
      glBegin(GL_LINES);{
				glVertex3f(-10, 0, 0);
				glVertex3f(10, 0, 0);
				glVertex3f(0, -10, 0);
				glVertex3f(0, 10, 0);
      } glEnd();
    } glPopMatrix();
  }
  gluvvPopNames();
}

//============================================================= draw widget
//=========================================================================
void CPWidgetRen::draw_widget()
{
	if(!gluvv.clip.on) return;
  
	if(gluvv.picking){
    fill();
    glLoadName(plane);
    glBegin(GL_QUADS);{
      glVertex3f(0,0,0);
      glVertex3f(width, 0, 0);
      glVertex3f(width, height, 0);
      glVertex3f(0, height, 0);
    } glEnd();
  }
  
  glColor4f(.1, .07, .57, 1);                   //----balls
  float spos[3] = {0,0,0};
  drawSphere(spos, cpball);                     //bottom left
  spos[0] = width;
  drawSphere(spos, cpball);                     //bottom right
  spos[1] = height;
  drawSphere(spos, cpball);                     //top right
  spos[0] = 0;
  drawSphere(spos, cpball);                     //top left

	glColor4f(.7, .07, .57, 1);                   //----slider
  float slpos[3] = {ballRad +(width-2*ballRad)*aslider, 0, 0};
  drawSlider(90, WdgYAxis, slpos, alphaslide);   //slider


  glColor4f(.09, .34, .55, 1);                  //----bars
  float bpos[3] = {0,0,0};
  drawBar(-90, WdgXAxis, height, bpos, cpbar);  //left
  drawBar(90, WdgYAxis, width, bpos, cpbar);    //bottom
  bpos[1] = height;
  drawBar(90, WdgYAxis, width, bpos, cpbar);    //top
  bpos[0] = width;
  bpos[1] = 0;
  drawBar(-90, WdgXAxis, height, bpos, cpbar);  //right

  if(gluvv.picking){
    transparent();
  }

}

//================================================================ set info
//=========================================================================
void CPWidgetRen::set_info()
{
  gluvv.clip.alpha = aslider;
  
  copyV3(gluvv.clip.pos, pos);
  matrixEqual(gluvv.clip.xform, xform);
  float dir[3] = {0,0,1};
  translateV3(gluvv.clip.dir, xform, dir);
	
  //now put the corners in volume space
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
  float m6[16];
  identityMatrix(m6);
  float volm[16];
  matrixMult(volm, m5, m6);
  float invm[16];
  inverseMatrix(invm, volm);
	
  
  float m7[16] = {1/gluvv.mv->xfSize, 0,                0,                0,
		              0,                1/gluvv.mv->yfSize, 0,                0,
			            0,                0,                1/gluvv.mv->zfSize, 0,
			            0,                0,                0,                1};
	
  float invVolm[16];
  matrixMult(invVolm, m7, invm);
	
	
	if(pickedObj == plane)	
	  translateV3W(gluvv.clip.mousepnt, invVolm, mousepnt);
  
  float cliptrans[16];
  identityMatrix(cliptrans);
  cliptrans[12] = pos[0];
  cliptrans[13] = pos[1];
  cliptrans[14] = pos[2];
  float clip2wld[16];
  matrixMult(clip2wld, cliptrans, xform);


  float m8[16] = {gluvv.mv->xfSize, 0,              0,              0,
		  0,              gluvv.mv->yfSize, 0,              0,
			0,              0,              gluvv.mv->zfSize, 0,
			0,              0,              0,              1};
	

  float invVolm2[16];
  matrixMult(invVolm2, m8, invVolm);
	
  float clip2vol[16];
  matrixMult(clip2vol, invVolm2, clip2wld);
	
  float v[3] = {-width/2, -height/2, 0};
  translateV3W(gluvv.clip.corners[0], clip2vol, v);
  v[0] += width;
  translateV3W(gluvv.clip.corners[1], clip2vol, v);
  v[1] += height;
  translateV3W(gluvv.clip.corners[2], clip2vol, v);
  v[0] -= width;
  translateV3W(gluvv.clip.corners[3], clip2vol, v);

}

//============================================================== update_pos
//=========================================================================
void CPWidgetRen::update_pos()
{
	if(gluvv.clip.ortho){
		float axis[3];
		float xf[16];
		identityMatrix(xf);
		switch(gluvv.clip.oaxis){
		case VolRenAxisXPos:
    case VolRenAxisXNeg:
			axis[0] = 0; axis[1] = 1; axis[2] = 0;
			axis2Rot(xf, axis, V_PI/2.0);
			break;
    case VolRenAxisYPos:
    case VolRenAxisYNeg:
			axis[0] = 1; axis[1] = 0; axis[2] = 0;
			axis2Rot(xf, axis, V_PI/2.0);
			break;
    case VolRenAxisZPos:
    case VolRenAxisZNeg:

			break;
		}

		//update orientation
		matrixMult(gluvv.clip.xform, gluvv.rinfo.xform, xf);
		matrixMult(xform, gluvv.rinfo.xform, xf);

		//update position
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
//			float m6[16] = {gluvv.mv->xfSize, 0,              0,              0,
//				              0,              gluvv.mv->yfSize, 0,              0,
//				              0,              0,              gluvv.mv->zfSize, 0,
//				              0,              0,              0,              1};
			float volm[16];
//			matrixMult(volm, m5, m6);
			matrixEqual(volm,m5);
			float tpos[3];
			//cerr << "pos0  " << vpos[0] <<  " " << vpos[1] << " " << vpos[2] << endl;
			translateV3W(tpos, volm, vpos);
			copyV3(pos, tpos);
			copyV3(gluvv.clip.vpos, vpos);
			//cerr << "pos1  " << pos[0] <<  " " << pos[1] << " " << pos[2] << endl;

	}
}

//==================================================================== pick
//=========================================================================
int  CPWidgetRen::pick(int data1, int data2, int data3,
		       float x, float y, float z)
{
  pickedObj = gluvv.pick.data3;
  fill();
  glutSetWindowTitle("Clipping Plane Widget");
  return 1;
}


int CPWidgetRen::pick()
{
  pickedObj = gluvv.pick.data3;
  glutSetWindowTitle("Clipping Plane Widget");
  fill();
  return 1;
}

//===================================================================== key
//=========================================================================
int CPWidgetRen::key(unsigned char key, int x, int y)
{
  if(key == 32){ //space bar
    gluvv.tf.paintme = 1;
    glutPostRedisplay();
    return 1; //key used
  }
	switch(key){
		case '1':
			gluvv.clip.oaxis = VolRenAxisXPos;
			glutPostRedisplay();
		  return 1;
		case '2':
			gluvv.clip.oaxis = VolRenAxisXNeg;
			glutPostRedisplay();
			return 1;
		case '3':
			gluvv.clip.oaxis = VolRenAxisYPos;
			glutPostRedisplay();
			return 1;
		case '4':
			gluvv.clip.oaxis = VolRenAxisYNeg;
			glutPostRedisplay();
			return 1;
		case '5':
			gluvv.clip.oaxis = VolRenAxisZPos;
			glutPostRedisplay();
			return 1;
		case '6':
			gluvv.clip.oaxis = VolRenAxisZNeg;
			glutPostRedisplay();
			return 1;
		case 'o':
		case 'O':
			if(gluvv.clip.ortho) gluvv.clip.ortho = 0;
			else gluvv.clip.ortho = 1;
			glutPostRedisplay();
			return 1;
	}

  return 0;   //key not used
}

//=================================================================== mouse
//=========================================================================
int  CPWidgetRen::mouse(int button, int state,
			int x, int y)
{
	if(state == GLUT_DOWN){
    lastwinpos[0] = x;
    lastwinpos[1] = y;
    switch(button){
    case GLUT_LEFT_BUTTON:
      switch(pickedObj){
      case cpball:
				if(gluvv.clip.ortho) return 0;
				glutSetWindowTitle("Clipping Plane Widget: ROTATE");
				tball.start((2.0 * x - gluvv.win.width)  / gluvv.win.width,
					(2.0 * y - gluvv.win.height) / gluvv.win.height);
				break;
      case cpbar:
				glutSetWindowTitle("Clipping Plane Widget: TRANSLATE {X,Y}");
				break;
      case plane:
				mouse2plane(x,y);
				gluvv.mprobe = 1;
				gluvv.volren.sampleRate = gluvv.volren.interactSamp;
				glutPostRedisplay();
				glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);
				break;
      }
			
      break;
			case GLUT_MIDDLE_BUTTON:
				
				break;
			case GLUT_RIGHT_BUTTON:
				
				break;
    }
    
  }
  else{ //state is _UP
    glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
    gluvv.volren.sampleRate = gluvv.volren.goodSamp;
    glutSetWindowTitle("Clipping Plane Widget");
    glutPostRedisplay();
  }
  return 1;
}

//==================================================================== move
//=========================================================================
int  CPWidgetRen::move(int x, int y)
{
  float dx = (float)(x - lastwinpos[0])/(float)gluvv.win.width;
  float dy = (float)(y - lastwinpos[1])/(float)gluvv.win.height;
  float d[3];
  subV3(d, pos, gluvv.env.eye);
  float dist = normV3(d);
  dx *= (gluvv.env.frustum[1]-gluvv.env.frustum[0]) * dist/gluvv.env.clip[0];
  dy *= (gluvv.env.frustum[3]-gluvv.env.frustum[2]) * dist/gluvv.env.clip[0];
  
  if((gluvv.mouse.state == GLUT_DOWN) &&            //left
     (gluvv.mouse.button == GLUT_LEFT_BUTTON) &&    //down
     (pickedObj == cpball)){                        //ball
		if(gluvv.clip.ortho) return (int)0;
		gluvv.volren.sampleRate = gluvv.volren.interactSamp;
    tball.update((2.0 * x - gluvv.win.width) /gluvv.win.width,
		 (2.0 * y - gluvv.win.height) /gluvv.win.height);
    tball.buildRotMatrix((float (*)[4])xform);
    glutPostRedisplay();
    
  }
  else if((gluvv.mouse.state == GLUT_DOWN) &&         //left
	  (gluvv.mouse.button == GLUT_LEFT_BUTTON) && //down
	  (pickedObj == cpbar)){                      //bar
    gluvv.volren.sampleRate = gluvv.volren.interactSamp;
    
		if(gluvv.clip.ortho){
			float nv[3];
			float ov[3] = {dx, dy, 0};
			float imv[16];
			inverseMatrix(imv, gluvv.rinfo.xform);
			translateV3(nv, imv, ov);
			switch(gluvv.clip.oaxis){
			case VolRenAxisXPos:
			case VolRenAxisXNeg:
				vpos[1] -= nv[1];
				vpos[2] -= nv[2];
				break;
			case VolRenAxisYPos:
			case VolRenAxisYNeg:
				vpos[0] -= nv[0];
				vpos[2] -= nv[2];
				break;
			case VolRenAxisZPos:
			case VolRenAxisZNeg:
				vpos[0] -= nv[0];
				vpos[1] -= nv[1];
				break;			
			}
		} else {
			pos[0] -= dx;
			pos[1] -= dy;
		}
    glutPostRedisplay();
  }
  else if((gluvv.mouse.state == GLUT_DOWN) &&         //left
	  (gluvv.mouse.button == GLUT_RIGHT_BUTTON) &&      //down
		(pickedObj == cpbar)){                            //bar
    gluvv.volren.sampleRate = gluvv.volren.interactSamp;
    
		if(gluvv.clip.ortho){
			//float nv[3];
			//float ov[3] = {dx, dy, 0};
			//float imv[16];
			//inverseMatrix(imv, gluvv.rinfo.xform);
			//translateV3(nv, imv, ov);
			switch(gluvv.clip.oaxis){
			case VolRenAxisXPos:
			case VolRenAxisXNeg:
				vpos[0] += dy; 
				break;
			case VolRenAxisYPos:
			case VolRenAxisYNeg:
				vpos[1] += dy;
				break;
			case VolRenAxisZPos:
			case VolRenAxisZNeg:
				vpos[2] += dy;
				break;			
			} 
		}
		else {
			pos[2] += dy;
		}
    glutPostRedisplay();
  }
  else if((gluvv.mouse.state == GLUT_DOWN) &&         //left
	  (gluvv.mouse.button == GLUT_LEFT_BUTTON) && //down
	  (pickedObj == alphaslide)){                 //alpha slider
    gluvv.volren.sampleRate = gluvv.volren.interactSamp;
    aslider -= dy/width;
    aslider = aslider < 0 ? 0 : (aslider > 1 ? 1 : aslider);
    glutPostRedisplay();
    char str[255];
    sprintf(str, "Clipping Plane Widget: ALPHA = %f", aslider);
    glutSetWindowTitle(str);
    
  }
  else if((gluvv.mouse.state == GLUT_DOWN) &&         //left
	  (gluvv.mouse.button == GLUT_LEFT_BUTTON) &&   //down
	  (pickedObj == plane)){                        //plane
    mouse2plane(x,y);
    set_info();
    char str[255];
    sprintf(str, "Clipping Plane Widget: Volume Position {%-1.3f, %-1.3f, %-1.3f}",
	    gluvv.clip.mousepnt[0], gluvv.clip.mousepnt[1], 
	    gluvv.clip.mousepnt[2]);
    glutSetWindowTitle(str);
    gluvv.volren.sampleRate = gluvv.volren.interactSamp;
    glutPostRedisplay();
  }

  lastwinpos[0] = x;
  lastwinpos[1] = y;

  return 1;
}

//================================================================= release
//=========================================================================
int  CPWidgetRen::release()
{
  transparent();
	pickedObj = -1;	
  return 1;
}

//============================================================= mouse2plane
//=========================================================================
void CPWidgetRen::mouse2plane(int x, int y)
{
  // assume we are always aligned with z axis
  // should be based on up vector and (at-eye)xup vector!!!!!
  float fx = 1 - (float)x/(float)gluvv.win.width;
  float fy = (float)(gluvv.win.height - y)/(float)gluvv.win.height;
  float vdir[3];
  subV3(vdir, gluvv.env.at, gluvv.env.eye);
  normalizeV3(vdir);
  scaleV3(gluvv.env.clip[0], vdir);
  float fcp[3];
  addV3(fcp, gluvv.env.eye, vdir);
  float cpnt[3];
  cpnt[0] = ((fcp[0] + gluvv.env.frustum[0]) + 
	     (gluvv.env.frustum[1] - gluvv.env.frustum[0]) * fx);
  cpnt[1] = ((fcp[1] + gluvv.env.frustum[2]) + 
	     (gluvv.env.frustum[3] - gluvv.env.frustum[2]) * fy);
  cpnt[2] = fcp[2];
  mousepnt[0] = cpnt[0];
  mousepnt[1] = cpnt[1];
  mousepnt[2] = cpnt[2];
  float ep[3];
  subV3(ep, gluvv.env.eye, pos);
  float me[3];
  subV3(me, gluvv.env.eye, cpnt);
  float t = dotV3(gluvv.clip.dir, ep)/dotV3(gluvv.clip.dir, me);
  negateV3(me);
  scaleV3(t,me);
  addV3(mousepnt, me, gluvv.env.eye);
  set_info();
}

//============================================================ GL Error Chk
//=========================================================================
int  CPWidgetRen::CPWidgetRenGlErr(char *place)
{
  GLenum errCode;
  const GLubyte *errString;
  
  if((errCode = glGetError()) != GL_NO_ERROR){
    errString = gluErrorString(errCode);
    cerr << endl << "OpenGL error : CPWidgetRen::" << place << " : " 
	 << (char *) errString << endl << endl;
    return 1;
  }
  return 0;
}








