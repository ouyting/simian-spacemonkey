//                   ________    ____   ___ 
//                  |        \  /    | /  /
//                  +---+     \/     |/  /
//                  +--+|  |\    /|     < 
//                  |  ||  | \  / |  |\  \ 
//                  |      |  \/  |  | \  \ 
//                   \_____|      |__|  \__\
//                       Copyright  2001 
//                      Joe Michael Kniss
//                   <<< jmk@cs.utah.edu >>>
//               "All Your Base are Belong to Us"
//
// TFWindow.cpp: implementation of the TFWindow class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif

#include "TFWindow.h"
#include <string.h>
#include <iostream.h>
#include "gluvv.h"
#include "glUE.h"
#include "VectorMath.h"
//#include <GL/glut.h>

#define COLOR_MAP_NAME 66666
#define ALPHA_MAP_NAME 66667
#define POINTS_NAME    66668
#define HIQ_SAMP_NAME  66669
#define LOQ_SAMP_NAME  66670

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//--------- functions to make c & c++ callbacks play nice together
void tfwinstaticdisplay(){
	if(gluvv.tf.tfwin){
		gluvv.tf.tfwin->display();
	}
}

void tfwinstaticmotion(int x, int y){
	if(gluvv.tf.tfwin){
		gluvv.tf.tfwin->motion(x,y);
	}
}

void tfwinstaticmouse(int button, int state, int x, int y)
{
	if(gluvv.tf.tfwin){
		gluvv.tf.tfwin->mouse(button, state, x, y);
	}
}

void tfwinstaticreshape(int w, int h)
{
	if(gluvv.tf.tfwin){
		gluvv.tf.tfwin->reshape(w,h);
	}
}

void tfwinstatickey(unsigned char key, int x, int y)
{
	if(gluvv.tf.tfwin){
		gluvv.tf.tfwin->key(key, x, y);
	}
}

void tfwinstaticspecial(int key, int x, int y)
{
	if(gluvv.tf.tfwin){
		gluvv.tf.tfwin->special(key, x, y);
	}
}

void tfwinstaticidle(){
	if(gluvv.tf.tfwin){
		gluvv.tf.tfwin->idle();
	}
}

//--------------- the window class ------------------------------------
TFWindow::TFWindow()
{
	width = 512;
	height = 256;
	picking = 0;

	points.setBin(-99999); //dummy node, never select it!!!

	TFPoint *tp;
	tp = new TFPoint(0, 0, .1);
	tp->cpick.reset(.6,1,.5);
	points.insert(tp);
	tp = new TFPoint(128, .5, 1);
	tp->cpick.reset(.3,1,.5);
	points.insert(tp);
	tp = new TFPoint(255, 1, .1);
	tp->cpick.reset(0,1,.5);
	points.insert(tp);

	deptexName = 0;

	data1  = 0;
  data2  = 0;
	data3  = 0;
  name32 = 0;
  name64 = 0;
  zpick  = 0xffffffff;

	exts[0][0] = -.03;
	exts[0][1] = 1.03;
	exts[1][0] = 1.03;
	exts[1][1] = -.03;
}

TFWindow::~TFWindow()
{

}

void TFWindow::create()
{
	char displaymode[256];
  //char displaymode[] = "1";
  if(gluvv.plat == GPOctane){
    strcpy(displaymode, "rgba=8 depth");
  }
  else if(gluvv.plat == GPOctane2){
    strcpy(displaymode, "rgba=8 double depth");
  }
  else if(gluvv.plat == GPInfinite)
    strcpy(displaymode, "double rgba depth");
  else{
    strcpy(displaymode, "double rgba depth");
  }

  //glutInitDisplayString(displaymode);
  //glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ACCUM);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(200, 400);
  gluvv.tf.tfWindow=
		glutCreateWindow("Vanilla Transfer Function");

	glutDisplayFunc(tfwinstaticdisplay);
	glutReshapeFunc(tfwinstaticreshape);
  glutMouseFunc(tfwinstaticmouse);
  glutMotionFunc(tfwinstaticmotion);
  glutKeyboardFunc(tfwinstatickey);   
	glutSpecialFunc(tfwinstaticspecial);
  glutIdleFunc(tfwinstaticidle);
}

void TFWindow::init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);

  /* Set up lighting. mostly for widgets */
  static float ambient[]             = {0.1, 0.1, 0.1, 1.0};
  static float diffuse[]             = {0.5, 1.0, 1.0, 1.0};
  static float front_mat_shininess[] = {60.0};
  static float front_mat_specular[]  = {0.2, 0.2,  0.2,  1.0};
  static float front_mat_diffuse[]   = {0.5, 0.28, 0.38, 1.0};
  static float lmodel_ambient[]      = {0.2, 0.2,  0.2,  1.0};
  static float lmodel_twoside[]      = {GL_FALSE};
  glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, gluvv.light.pos);
  glEnable(GL_LIGHT0);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,  lmodel_ambient);
  glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
  glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, front_mat_shininess);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  front_mat_specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   front_mat_diffuse);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDisable(GL_CULL_FACE);
  
	
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 1, 0, 1, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();


  GlErr("TFWindow::","init()");

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void TFWindow::destroy()
{
	glutDestroyWindow(gluvv.tf.tfWindow);
	gluvv.tf.tfWindow = 0;
	deptexName = 0;
}

void TFWindow::display()
{
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glColor4f(1,0,0,1);
	//glutSolidSphere(1,10,10);

	glColor4f(1,1,1,1); //put a border around it
	glBegin(GL_LINE_STRIP);{
		glVertex3f(-.01,-.01,0);
		glVertex3f(1.01,-.01,0);
		glVertex3f(1.01,1.01,0);
		glVertex3f(-.01,1.01,0);
		glVertex3f(-.01,-.01,0);
	} glEnd();

	if(gluvv.volren.deptex)
		draw2Dtf();
	else if(gluvv.volren.tlut)
		draw1Dtf();

	glutSwapBuffers();
}


void TFWindow::mouse(int button, int state, int x, int y)
{
	pickPos[0] = x;
	pickPos[1] = height - y;
	butt = button;
	
	if(state == GLUT_DOWN)
		if(pick()){
			//cerr << "data1 " << data1 << " data2 " << data2 << " data3 " << data3 << endl;
		}


	if((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN) &&
		 (data3 == ALPHA_MAP_NAME)){ //insert a newpoint
		//this still isn't quite right!
		float xp = (float)x/(float)width;
		float yp = ((height - y)/(float)(height) - (1 - pntsHeight))/pntsHeight;
		xp = xp<=1 ? (xp>=0 ? xp : 0) : 1; 
		yp = yp<=1 ? (yp>=0 ? yp : 0) : 1; 
		TFPoint *p = new TFPoint(xp*255, xp, yp);
		points.insert(p);
		glutPostRedisplay();
		data3=p->getBin(); //now make the new point the picked point
		data2=POINTS_NAME;
	}

	if(state == GLUT_UP){
		gluvv.volren.sampleRate = gluvv.volren.goodSamp;
		glutSetWindowTitle("Vanilla Transfer Function");
		updateVolume();
	}
}

void TFWindow::motion(int x, int y)
{
	int idx = x - pickPos[0];
	int idy = (height - y) - pickPos[1];
	float fdx = idx/(float)width;
	float fdy = idy/(float)height;
	
	if(data2 == POINTS_NAME){

		if(butt == GLUT_LEFT_BUTTON){
			TFPoint *p = points.getPoint(data3);
		  if(!p) return;
			p->setPosU((*p)[0] + (fdx*(exts[1][0]-exts[0][0])));
			p->setPosU((*p)[0] < 1 ? ((*p)[0] >= 0 ? (*p)[0] : 0) : 1);
			p->setPosV((*p)[1] + (fdy/pntsHeight*(exts[0][1]-exts[1][1])));
			p->setPosV((*p)[1] < 1 ? ((*p)[1] >= 0 ? (*p)[1] : 0) : 1);
			p->setBin(affine(0, (*p)[0], 1, 0, 255));
			data3 = p->getBin();
			points.insert(p);
			updateColor();
			gluvv.volren.loadTLUT = 1;
			glutPostRedisplay();
			gluvv.volren.sampleRate = gluvv.volren.interactSamp;
			updateVolume();
		}
		else if(butt == GLUT_RIGHT_BUTTON){
			TFPoint *p;
			p = points.findPoint(data3);
			if(!p) return;
			p->cpick.updateHL(fdx, fdy);
			p->setBin(data3);
			updateColor();
			gluvv.volren.loadTLUT = 1;
			glutPostRedisplay();
			gluvv.volren.sampleRate = gluvv.volren.interactSamp;
			updateVolume();
		}
		else if(butt == GLUT_MIDDLE_BUTTON){

		}
	} 
	else if(data2 == -1){ //every thing else!
		if(data3 == HIQ_SAMP_NAME){
			char title[256];
			gluvv.volren.goodSamp += fdx*6;
			sprintf(title,"High Quality Sample Rate = %f", gluvv.volren.goodSamp);
			glutSetWindowTitle(title);
			glutPostRedisplay();
			updateVolume();
		}
		else if(data3 == LOQ_SAMP_NAME){
			char title[256];
			gluvv.volren.interactSamp += fdx*6;
			sprintf(title,"Low Quality Sample Rate = %f", gluvv.volren.interactSamp);
			glutSetWindowTitle(title);
			glutPostRedisplay();
			updateVolume();
		}
	}

	pickPos[0] = x;
	pickPos[1] = (height - y);

}


void TFWindow::key(unsigned char key, int x, int y)
{
	if(key == 27){ //escape key
		destroy();
	}
	switch(key){
		case 127: //delete key
			if(data2 == POINTS_NAME){
				TFPoint *p;
				if(p = points.getPoint(data3)){
					delete p;
					updateColor();
					glutPostRedisplay();
					updateVolume();
					data2 = -1;
					data3 = -1;
				}
			}
			break;

	default:
		cerr << "key = " << (unsigned char)key << " == (int)" << (int)key << endl;
	}
}

void TFWindow::special(int key, int x, int y)
{
	cerr << "special key: " << key << endl; 
}

void TFWindow::idle()
{

}

void TFWindow::reshape(int w, int h)
{
	//cerr << "reshaping!" << endl;
	width = w;
	height = h;

	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
	glOrtho(-.03, 1.03, -.03, 1.03, -1, 1);

	glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

	cerr << glutGetWindow() <<" - is current window vs " << gluvv.tf.tfWindow << endl;

	GlErr("TFWindow::","reshape()");
	glutPostRedisplay();
}

void TFWindow::draw2Dtf()
{
	if(!deptexName){
		glGenTextures(1, &deptexName);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, deptexName);
		
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			256,
			256,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			gluvv.volren.deptex);
		glFlush();
	}
	

	//cerr << "drawing 2d tf" << endl;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

	glBindTexture(GL_TEXTURE_2D, deptexName);
	glBegin(GL_QUADS);{
		glTexCoord2f(0,0);
		glVertex3f(0,0,-1);
		glTexCoord2f(1,0);
		glVertex3f(1,0,-1);
		glTexCoord2f(1,1);
		glVertex3f(1,1,-1);
		glTexCoord2f(0,1);
		glVertex3f(0,1,-1);
	} glEnd();

	glDisable(GL_BLEND);

}

void TFWindow::draw1Dtf()
{

	glEnable(GL_DEPTH_TEST);
	
	TLUT *tl = gluvv.volren.tlut;
	if(!tl) return;

	float top    = .2;         //set up dimensions for color band
	float bottom = 0;
	float dx     = 1.0/256.0;

	float *cv = tl->GetRGBA(0);

	glPushName((int)this);
	glPushName(-1);
	glPushName(-1);
	glPushName(-1);

	glEnable(GL_BLEND);        //blend color band
	glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

	glLoadName(COLOR_MAP_NAME); {  //draw color band
		glBegin(GL_QUADS);{
			for(int i =0; i< 256; ++i){
					glColor4fv(cv);
					glVertex3f(i*dx,     bottom, 0);
					glVertex3f((i+1)*dx, bottom, 0);
					glVertex3f((i+1)*dx, top,    0);
					glVertex3f(i*dx,     top,    0);
					cv += 4;
			}
		} glEnd();
	} glLoadName(-1);

	glDisable(GL_BLEND);  //dont blend no more

	top = 1;              //set up dimensions for alpha band
	bottom = .2;
	pntsHeight = top - bottom;
	
	glLoadName(ALPHA_MAP_NAME);{  //draw a back drop for picking
		glBegin(GL_QUADS);{
			glColor4f(0,0,0,1);
			glVertex3f(0, bottom,-.1);
			glVertex3f(1, bottom,-.1);
			glVertex3f(1, top,   -.1);
			glVertex3f(0, top,   -.1);
		} glEnd();
	} glLoadName(-1);

	glPopName();                  //set up point names
	glLoadName(POINTS_NAME);
	glPushName(-1);

	TFPoint *p;
	float v0[3] = {0,bottom,0};   //set up some vertex & color buffers
	float v1[3] = {1,bottom,0};
	float c0[3] = {0,0,0};
	float c1[3] = {0,0,0};
	
	for(int i=0; i<256; ++i){     //check each bin for a point
		if(p = points.findPoint(i)){ //found a point
			glPushMatrix();{
				v1[0] = (*p)[0];
				
				v1[1] = affine(0, (*p)[1], 1, bottom, top);
				
				glColor3f(1,1,1);        //connect points with colored lines
				p->cpick.getColor(c1);
				glBegin(GL_LINES);{
					glColor3fv(c0);
				  glVertex3fv(v0); 
					glColor3fv(c1);
					glVertex3fv(v1);
				} glEnd();

				float ballsz = .01;      //set up ball size
				
				glLoadName(i);{          //point name
					glTranslatef(v1[0], v1[1], v1[2]);  //spot to draw it
					if((data2 == POINTS_NAME) && (data3 == i))
						glColor3f(1,0,0);
					else
						glColor3f(1,1,1);
					glBegin(GL_QUADS);{    //draw box behind it
						glVertex3f(-ballsz*1.2, ballsz*1.2, 0);
						glVertex3f(ballsz*1.2, ballsz*1.2, 0);
						glVertex3f(ballsz*1.2, -ballsz*1.2, 0);
						glVertex3f(-ballsz*1.2, -ballsz*1.2, 0);
					} glEnd();
					glColor3fv(c1);
					glutSolidSphere(ballsz, 10, 10); //draw the ball (colored)
				} glLoadName(-1);

				v0[0]=v1[0]; v0[1]=v1[1]; v0[2]=v1[2]; //fix buffers
				c0[0]=c1[0]; c0[1]=c1[1]; c0[2]=c1[2];
			} glPopMatrix();
		}
	}

	v1[0] = 1;  //set up last line
	v1[1] = bottom;
	c1[0] = c1[1] = c1[2] = 0;
	glPopName();
	glLoadName(-1);
	glPushName(-1);

	glBegin(GL_LINES);{ //draw last line
		glColor3fv(c0);
		glVertex3fv(v0); 
		glColor3fv(c1);
		glVertex3fv(v1);
	} glEnd();

	glPushMatrix();{                  //sample rate sliders!
		glLoadName(LOQ_SAMP_NAME);
		glColor3f(.5,0.,7);
		glTranslatef(gluvv.volren.interactSamp/6, 0, 0);
		glutSolidSphere(.015, 10, 10); //draw the ball (colored)		
	} glPopMatrix();
	
	glPushMatrix();{
		glLoadName(HIQ_SAMP_NAME);
		glColor3f(.7,0.,5);
		glTranslatef(gluvv.volren.goodSamp/6, 0, 0);
		glutSolidSphere(.02, 10, 10); //draw the ball (colored)		
	} glPopMatrix();



}

int TFWindow::pick()
{
  data1  = 0;
  data2  = 0;
	data3  = 0;
  name32 = 0;
  name64 = 0;
  zpick  = 0xffffffff;
  //if(gluvv.pick.prim){
    //gluvv.pick.prim->release();
    //gluvv.pick.prim   = 0;
  //}
	
	//cerr << "picking" << endl;

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
	
  GLfloat projection[16];
  glGetFloatv(GL_PROJECTION_MATRIX, projection);
  
  int pickBufferSize = 128;
  GLuint *pickBuffer = new GLuint[pickBufferSize];
	
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  {
    glLoadIdentity();
    GLdouble pickbox = 4;
    gluPickMatrix((GLdouble)pickPos[0], (GLdouble)pickPos[1], 
			pickbox, pickbox, viewport);
    
    glMultMatrixf(projection);
    glMatrixMode(GL_MODELVIEW);
    glSelectBuffer(pickBufferSize, pickBuffer);
    glRenderMode(GL_SELECT);
		
    glInitNames();
    glPushName(0);                              
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // ---- RENDER -------------------------------
    //cycle through scene graph ------------------
    gluvv.picking = 1;
    draw1Dtf();
    gluvv.picking = 0;
  }
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
	
  glFlush();
	
  // now see what we got!
  GLint hits = glRenderMode(GL_RENDER);
  //cerr << "hits = " << hits;
  GLuint nnames, z, minz = 0xffffffff;
  int gotone = 0;
	
  if(hits > 0){
    
    //i tracks the number of hits, j tracks postion in pick buffer
    int i, j; 
    for(i=0, j=0; (j<pickBufferSize && i<hits); i++){
      //first item on stack is the number of items
      nnames = pickBuffer[j++]; 
      z = pickBuffer[j++];      //second is the z value for this hit
      if(nnames > 1 && z <= minz){
				gotone = 1;
				minz= z;
				gluvv.pick.z = minz;
				
#if (_MIPS_SZPTR == 64) //64 bit
				j += nnames - 5;
				//reconstruct your 'this' pointer
				unsigned long o1 = pickBuffer[j++];
				unsigned long o2 = pickBuffer[j++];
				name64     = (o1<<32)|o2;
				data1      = pickBuffer[j++];
				data2      = pickBuffer[j++];
				data3      = pickBuffer[j++];
#else
				j += nnames - 3;
				name32 = pickBuffer[j++];
				data1  = pickBuffer[j++];
				data2  = pickBuffer[j++];
				data3  = pickBuffer[j++];
#endif
				//cerr << "here: " << gluvv.pick.name32  << " " << gluvv.pick.data1 
				//   << " " << gluvv.pick.data2
				//   << " " << gluvv.pick.data3 << endl;
      }
      else{
				j += nnames + 1;
      }
    }
  }
	
  
  delete[] pickBuffer;
	
  if(gotone) return 1;
	
  return 0;
 

}


void TFWindow::updateColor()
{
	float c0[3] = {0,0,0};
	float a0 = 0;
	float c1[3] = {0,0,0};
	float a1 = 0;
	int last = 0;
	TFPoint *p;
	for(int i=0; i< 256; ++i){
		if(p = points.findPoint(i)){
			p->cpick.getColor(c1);
			a1=(*p)[1];
			gluvv.volren.tlut->alphaRamp(last,i,a0,a1);
			gluvv.volren.tlut->redRamp(last,i,c0[0],c1[0]);
			gluvv.volren.tlut->greenRamp(last,i,c0[1],c1[1]);
			gluvv.volren.tlut->blueRamp(last,i,c0[2],c1[2]);
			c0[0]=c1[0]; c0[1]=c1[1]; c0[2]=c1[2];
			a0=a1;
			last = i;
		}
	}
	c1[0] = c1[1] = c1[2] = a1 = 0;
	gluvv.volren.tlut->alphaRamp(last,255,a0,a1);
	gluvv.volren.tlut->redRamp(last,255,c0[0],c1[0]);
	gluvv.volren.tlut->greenRamp(last,255,c0[1],c1[1]);
	gluvv.volren.tlut->blueRamp(last,255,c0[2],c1[2]);

}

void TFWindow::updateVolume()
{
	glutSetWindow(gluvv.mainWindow);
	glutPostRedisplay();
	glutSetWindow(gluvv.tf.tfWindow);
}


//--------------------- TFPoint class definitions ----------------------

TFPoint::TFPoint()
{ //we are a dummy point;
   bin = -1;
   pos[0] = 0;
   pos[1] = 0;
   pos[2] = 0;
}

TFPoint::TFPoint(int b, float x, float y, float z)
{
   pos[0]=x;
   pos[1]=y;
   pos[2]=z;
   next = prev = 0;
   bin = b;
}


TFPoint::~TFPoint(){
   if(prev) prev->next = 0;
   if(next) delete next;
}

TFPoint *TFPoint::insert(TFPoint *p)
{
	if(bin < p->getBin()){
		if(next){
			next->insert(p);
		}
		else{ //no next
			next = p;
			p->prev = this;
			p->next = NULL;
		}
		return this;
	}
	else{
		p->prev = prev;
		p->next = this;
		if(prev)
			prev->next = p;
		prev = p;
		return p;
	}
}

TFPoint *TFPoint::getPoint(int b)
{
	if(bin == b){
		if(prev){
			prev->next = next;
		}
		if(next){
			next->prev = prev;
		}
		this->prev = 0;
		this->next = 0;
		return this;
	}
	else{
		if(next)
			return next->getPoint(b);
		else 
			return 0;
	}
}


TFPoint *TFPoint::findPoint(int b)
{
	if(bin == b){
		return this;
	}
	else{
		if(next)
			return next->findPoint(b);
		else 
			return NULL;
	}
}



