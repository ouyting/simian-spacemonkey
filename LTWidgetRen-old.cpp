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
// LTWidgetRen.cpp: implementation of the LTWidgetRen class.
//   Light Widget Renderer
//////////////////////////////////////////////////////////////////////

#include "LTWidgetRen.h"
#include "gluvv.h"
#include "glUE.h"

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#include "VectorMath.h"

#include <iostream.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LTWidgetRen::LTWidgetRen()
{
	screenpos[0] = -.4;
	screenpos[1] = -.1;
	screenrad = .05;

	position[2] = -1;

	drawOn = 0;
}

LTWidgetRen::~LTWidgetRen()
{

}

//=============================================================== draw
//====================================================================
void LTWidgetRen::draw()
{
	if(gluvv.reblend) return;
	if(!drawOn) return;


	static int start = 1;
	if(start){
		glLightfv(GL_LIGHT0, GL_POSITION, gluvv.light.pos);
		start = 0;
	}

	float dist = position[2] - gluvv.env.eye[2];

	position[0] = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	               dist/gluvv.env.clip[0]) * screenpos[0];
  position[1] = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	               dist/gluvv.env.clip[0]) * screenpos[1];

	float ballRad = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
  	               dist/gluvv.env.clip[0]) * screenrad;

	gluvvPushName();
  glPushName(22);
  glPushName(222);
  glPushName(2222);

	glPushMatrix();{
		glTranslatef(position[0], position[1], position[2]);
		glutSolidSphere(ballRad, 20, 20);
	} glPopMatrix();

	gluvvPopNames();


	GlErr("TFWidgetRen","draw()");
}

//=============================================================== init
//====================================================================
void LTWidgetRen::init()
{
	float lxf[16];
	tball.buildRotMatrix((float (*)[4])lxf);
	translateV3(gluvv.light.pos, lxf, gluvv.light.startpos);
	glLightfv(GL_LIGHT0, GL_POSITION, gluvv.light.pos);
	copyV3(lastpos,gluvv.light.pos);
	tball.clear();
	genXForm();
}

//=============================================================== pick
//====================================================================
int  LTWidgetRen::pick(int data1, int data2, int data3,
					float x, float y, float z)
{

	return 1;
}

//=============================================================== pick
//====================================================================
int  LTWidgetRen::pick()
{

	return 1;
}

//================================================================ key
//====================================================================
int  LTWidgetRen::key(unsigned char key, int x, int y)
{

	return 0;
}

//============================================================== mouse
//====================================================================
int  LTWidgetRen::mouse(int button, int state,
					 int x, int y)
{
	if(state == GLUT_DOWN){
		lastwinpos[0] = x;
		lastwinpos[1] = y;
		if(button == GLUT_LEFT_BUTTON){
			float sx = (float)((gluvv.win.width - x)/(float)gluvv.win.width - .5 - screenpos[0])/screenrad;
			float sy = (float)((gluvv.win.height- y)/(float)gluvv.win.width - .5 - screenpos[1])/screenrad;
			sx = -sx*.5;
			sy = -sy*.5;
			tball.start(sx,sy);
	    glutSetWindowTitle("Light Widget - rotate");
		} else if(button == GLUT_RIGHT_BUTTON) {
			glutSetWindowTitle("Light Widget - translate");
		} else if(button == GLUT_MIDDLE_BUTTON) {
			glutSetWindowTitle("Light Widget - radius");
		}
	} else {
		if(button == GLUT_LEFT_BUTTON){
			gluvv.light.gload = 1;
		}
		gluvv.volren.sampleRate = gluvv.volren.goodSamp;
    glutPostRedisplay();
    glutSetWindowTitle("Light Widget");
	}

	return 1;
}

//============================================================ release
//====================================================================
int  LTWidgetRen::move(int x, int y)
{

	float sdx = (float)(x - lastwinpos[0])/(float)gluvv.win.width;
  float sdy = (float)(y - lastwinpos[1])/(float)gluvv.win.height;

	if(gluvv.mouse.button == GLUT_LEFT_BUTTON){
		x = x>(int)gluvv.win.width ? gluvv.win.width : (x<0 ? 0 : x);
		y = y>(int)gluvv.win.height ? gluvv.win.height : (y<0 ? 0 : y);
		float sx = (float)((gluvv.win.width - x)/(float)gluvv.win.width - .5 - screenpos[0])/screenrad;
		float sy = (float)((gluvv.win.height- y)/(float)gluvv.win.width - .5 - screenpos[1])/screenrad;
		sx = -sx * .5;
		sy = -sy * .5;
		tball.update(sx,sy);
		float lxf[16];
		tball.buildRotMatrix((float (*)[4])lxf);
		normalizeV3(gluvv.light.startpos);
		scaleV3(normV3(gluvv.light.pos), gluvv.light.startpos);
		translateV3(gluvv.light.pos, lxf, gluvv.light.startpos);
		glLightfv(GL_LIGHT0, GL_POSITION, gluvv.light.pos);
		genXForm();
		
		gluvv.volren.sampleRate = gluvv.volren.interactSamp;
		glutSetWindowTitle("Light Widget: Rotate light");
		glutPostRedisplay();
	}
	else if(gluvv.mouse.button == GLUT_RIGHT_BUTTON){
		screenpos[0]  -= sdx;
		screenpos[1]  -= sdy;
		gluvv.volren.sampleRate = gluvv.volren.interactSamp;
		glutSetWindowTitle("Light Widget: MOVE");
		glutPostRedisplay();
	} else if(gluvv.mouse.button == GLUT_MIDDLE_BUTTON){
		if(sdy < 0){
			scaleV3(1.0+sdy, gluvv.light.pos);
		} else {
			scaleV3(1.0-sdy, gluvv.light.pos);
		}
		glutSetWindowTitle("Light Widget: RADIUS");
		cerr << "light rad = " << normV3(gluvv.light.pos);
		genXForm();
		glutPostRedisplay();
	}
	
	lastwinpos[0] = x;
	lastwinpos[1] = y;

	return 1;
}

//============================================================ release
//====================================================================
int  LTWidgetRen::release()
{

	return 0;
}

int  LTWidgetRen::onoff()
{
	if(drawOn) drawOn = 0;
	else drawOn = 1;
	return drawOn;
}

//============================================= generate the Transform
//====================================================================
void  LTWidgetRen::genXForm()
{
	float d0 = normV3(gluvv.light.pos);

	//cerr << "norm " << d0 << " div " << 1.0/(d0) << endl;
	//projection matrix
		gluvv.light.pj[0] = 1;
		gluvv.light.pj[1] = 0;
		gluvv.light.pj[2] = 0;
		gluvv.light.pj[3] = 0;
		gluvv.light.pj[4] = 0;
		gluvv.light.pj[5] = 1;
		gluvv.light.pj[6] = 0;
		gluvv.light.pj[7] = 0;
		gluvv.light.pj[8] = 0;
		gluvv.light.pj[9] = 0;
		gluvv.light.pj[10] = 0;
		gluvv.light.pj[11] = 1.0/(d0);
		gluvv.light.pj[12] = 0;
		gluvv.light.pj[13] = 0;
		gluvv.light.pj[14] = 0;
		gluvv.light.pj[15] = 1;
		//cerr << "projection:" << endl;
		//printMatrix(gluvv.light.pj);
	//look at/ modelview
		glPushMatrix();{ //joe, why are you so damn lazy??? I will never know
			glLoadIdentity();
			float ltdir[3];
			copyV3(ltdir, gluvv.light.pos);
			negateV3(ltdir);
			normalizeV3(ltdir);

			gluLookAt(ltdir[0],  //eye
								ltdir[1], 
								ltdir[2],  
								0,         //at
								0, 
								0,  
						    0,         //up
								1, 
						    0);          
			glGetFloatv(GL_MODELVIEW_MATRIX, gluvv.light.mv);  //save modelview matrix
			gluvv.light.mv[14] = -gluvv.light.mv[14];
		}glPopMatrix();
	//cerr << "lookat" << endl;
	//printMatrix(gluvv.light.mv);

	//the whole transform
		matrixMult(gluvv.light.xf,gluvv.light.pj, gluvv.light.mv);

	gluvv.light.load = 1;

	//cerr << "the transform:" << endl;
	//printMatrix(gluvv.light.xf);
}

