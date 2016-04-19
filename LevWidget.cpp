//-------------------------------------------------------------------------
//
//   Joe Kniss
//    Levoy Triangle for transfer function widget
//  LevWidget.cpp  Handles 2&3D transfer functions 
//                  also renders as a square
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
#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#include "LevWidget.h"
#include "gluvv.h"
#include "VectorMath.h"


static int ids = 0;

//============================================================ LevWidget
//======================================================================
LevWidget::LevWidget(gluvvGlobal *g)
  :cpick(g)
{
  on = 1;
  drawOn = 1;
  next = 0;
  widgetInit();
  
  id = ++ids;
  
  for(int i=0; i<3; ++i)
    for(int j=0; j<2; ++j)
      verts[i][j] = 0;
  
  pickedObj = -1;
  picked = 0;
  depth = 0;
  
  cpick.getColor(color);

  mode = LWclearMode;
  type = LWtriangle;

  alpha = .5;
  
  scatter = .5;

  be = 1;
}

LevWidget::LevWidget(LevWidget &lw)
:cpick(lw.cpick)
{
	on = 1;
	drawOn = 1;
	next = 0;
	widgetInit();
	id = ++ids;

	for(int i=0; i<3; ++i)
    for(int j=0; j<2; ++j)
      verts[i][j] = lw.verts[i][j];

	pickedObj = -1;
	picked = 0;
	depth = lw.depth;

	cpick.getColor(color);

	mode = lw.mode;
	type = lw.type;

	alpha = lw.alpha;
	be = lw.be;

	thresh[0] = lw.thresh[0];
	thresh[1] = lw.thresh[1];
}


//=========================================================== ~LevWidget
//======================================================================
LevWidget::~LevWidget()
{

}

//================================================================= draw
//======================================================================
void LevWidget::draw()
{
  if(!on){
    if(next) next->draw();
    return;
  }
	
	if(gluvv.picking){
		if(next)
		  next->draw();
	}

	if((gluvv.dmode == GDM_V1)||(gluvv.dmode == GDM_VGH_V)){
		verts[0][1] = 0;
		verts[1][1] = 1;
		verts[2][1] = 1;
		type = LW1d;
	}

  defaultMaterial();
  if(type == LWtriangle) drawTriangle();
  if((type == LWsquare)||(type == LWdef)||(type == LW1d))   drawSquare();

	if(!gluvv.picking){
		if(next)
			next->draw();
	}
  LevWidgetGlErr("draw");
}

//======================================================== draw triangle
//======================================================================
void LevWidget::drawTriangle()
{
  glDisable(GL_BLEND);
  //glDisable(GL_TEXTURE_3D_EXT);
  glEnable(GL_DEPTH_TEST);
  
  float bpos[3] = {verts[0][0]*width,verts[0][1]*height,0};
  float lpos[3] = {verts[1][0]*width,verts[1][1]*height,0};
  float rpos[3] = {verts[2][0]*width,verts[2][1]*height,0};

  float tval = thresh[1]/verts[1][1];
  
  float tpos[3] = {(verts[0][0]+
		      (verts[2][0]-verts[0][0])*tval)*width,
		   (verts[0][1]+
		    (verts[2][1]-verts[0][1])*tval)*height,
		   0};
  float tleft = (verts[0][0]+(verts[1][0]-verts[0][0])*tval)*width;
  
  if(drawOn){
    glColor4f(.1, .07, .57, 1);                  //balls
    LWdrawSphere(bpos, bball);                     //bottom
    LWdrawSphere(rpos, rball);                     //right
  
    if(picked)
      glColor4f(.7, .34, .55, 1);                  //bars
    else
      glColor4f(.09, .34, .55, 1);                  //bars
    LWdrawBar(90, WdgYAxis, (verts[2][0]-verts[1][0])*width, lpos, tbar);
    float tmpb = ballsz;
    ballsz=barsz;
    LWdrawSphere(lpos, tbar);
    ballsz = tmpb;
    
    glColor4f(.5, .07, .57, 1);                     //threshold color
    glEnable(GL_LIGHTING);
    tpos[0] += ballsz; //offset ball pos right by ball size
    LWdrawSphere(tpos, slider);                     //threshold ball
    tpos[0] -= ballsz;
    
    glDisable(GL_LIGHTING);
    glPopName();                                    //messy names!!!
    glPopName();
    glLoadName((GLuint) id);
    glPushName((GLuint) tbar);                      //same as top bar
    glPushName((GLuint) LEVWIDGNAME);
    glBegin(GL_LINES);{                             //threshold
      glVertex3f(tpos[0], tpos[1], tpos[2]);
      glVertex3f((verts[0][0]+(verts[1][0]-verts[0][0])*tval)*width,
		 tpos[1], tpos[2]);
    } glEnd();
    
  } //if draw on

  if(drawOn || gluvv.picking){
    glPopName();                                    //messy names!!!
    glPopName();
    glLoadName((GLuint) id);
    glPushName((GLuint) trap);
    glPushName((GLuint) LEVWIDGNAME);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if(mode == LWcolorMode)
      glColor4f(color[0], color[1], color[2], 1);
    else
      glColor4f(1, 1, 1, 0);
    glBegin(GL_QUADS);{                             //trapizoid
      glVertex3f(tpos[0], tpos[1], tpos[2]);
      glVertex3f(tleft, tpos[1], tpos[2]);
      glVertex3f(lpos[0], lpos[1], lpos[2]);
      glVertex3f(rpos[0], rpos[1], rpos[2]);
      
    } glEnd();
    glDisable(GL_BLEND);
  }

  if(drawOn){
    glColor4f(.09, .34, .55, 1);                  //bars
    glBegin(GL_LINE_STRIP);{                        //triangle boundry
      glVertex3d(lpos[0], lpos[1], lpos[2]);
      glVertex3d(bpos[0], bpos[1], bpos[2]);
      glVertex3d(rpos[0], rpos[1], rpos[2]);
    } glEnd();
  }
    
  glEnable(GL_LIGHTING);
}

//========================================================== draw square
//======================================================================
void LevWidget::drawSquare()
{
  glDisable(GL_BLEND);
  //glDisable(GL_TEXTURE_3D_EXT);
  glEnable(GL_DEPTH_TEST);

  float lpos[3] = {verts[1][0]*width,verts[1][1]*height+barsz,0};
  float rpos[3] = {verts[2][0]*width,verts[2][1]*height+barsz,0};
  float bpos[3] = {verts[2][0]*width,verts[0][1]*height,0};
  float tpos[3] = {thresh[0]*width, thresh[1]*height, rpos[2]}; 

  if(drawOn){
    glColor4f(.1, .07, .57, 1);                  //balls
    //rpos[1] += ballsz;
    LWdrawSphere(rpos, rball);                     //right
    //rpos[1] -= ballsz;
    LWdrawSphere(bpos, bball);                     //bottom

    if(picked)
      glColor4f(.7, .34, .55, 1);
    else
      glColor4f(.09, .34, .55, 1);                  //top bar
    //lpos[1] += barsz;
    LWdrawBar(90, WdgYAxis, (verts[2][0]-verts[1][0])*width, lpos, tbar);
    float tmpb = ballsz;
    ballsz=barsz;
    LWdrawSphere(lpos, tbar);
    ballsz = tmpb;
    //lpos[1] -=barsz;
    
    glColor4f(.5, .07, .57, 1);                     //threshold color
    glEnable(GL_LIGHTING);
    LWdrawSphere(tpos, slider);                     //threshold ball
    
		glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glPopName();                                    //messy names!!!
    glPopName();
    glLoadName((GLuint) id);
    glPushName((GLuint) tbar);                      //same as top bar
    glPushName((GLuint) LEVWIDGNAME);
    glColor4f(.09, .34, .55, 1);                    //box
    glBegin(GL_LINE_STRIP);{                       //threshold
      glVertex3f(rpos[0], rpos[1], rpos[2]);
      glVertex3f(rpos[0], bpos[1], bpos[2]);
      glVertex3f(lpos[0], bpos[1], bpos[2]);
      glVertex3f(lpos[0], lpos[1], lpos[2]);
    } glEnd();

  }//if draw on
  if(drawOn || gluvv.picking){
    glPopName();                                    //messy names!!!
    glPopName();
    glLoadName((GLuint) id);
    glPushName((GLuint) trap);
    glPushName((GLuint) LEVWIDGNAME);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    if(mode == LWcolorMode)
      glColor4f(color[0], color[1], color[2], 1);
    else
      glColor4f(1, 1, 1, 0);
    glBegin(GL_QUADS);{                             //trapizoid
      glVertex3f(rpos[0], bpos[1], bpos[2]);
      glVertex3f(lpos[0], bpos[1], bpos[2]);
      glVertex3f(lpos[0], lpos[1], lpos[2]);
      glVertex3f(rpos[0], rpos[1], rpos[2]);

    } glEnd();
    glDisable(GL_BLEND);
  }

  glEnable(GL_LIGHTING);

}

//========================================================= LWdrawSphere
//======================================================================
void LevWidget::LWdrawSphere(float pos[3], GLuint name)
{
  glPushMatrix();{
    glPopName();
    glPopName();
    glLoadName((GLuint) id);
    glPushName((GLuint) name);
    glPushName((GLuint) LEVWIDGNAME);
    glTranslatef(pos[0], pos[1], pos[2]);
    gluSphere(qobj, ballsz, ballSlice, ballSlice);
  } 
  glPopMatrix();
}

//============================================================ LWdrawBar
//======================================================================
void LevWidget::LWdrawBar(float rot, WdgRotAxis axis,  
			  float len, float pos[3], unsigned int name)
{
  glPushMatrix(); { //bottom front bar
    glPopName();
    glPopName();
    glLoadName((GLuint) id);
    glPushName((GLuint) name);
    glPushName((GLuint) LEVWIDGNAME);
    glTranslatef(pos[0], pos[1], pos[2]);
    
    if(axis == WdgXAxis) glRotatef(rot, 1, 0, 0);
    else if(axis == WdgYAxis) glRotatef(rot, 0, 1, 0);
    else glRotatef(rot, 0, 0, 1);
    
    gluCylinder(qobj, barsz, barsz, len, 
		barSlice, barStack);
  } 
  glPopMatrix();
}

//============================================================ LWdrawBar
//======================================================================
void LevWidget::LWdrawSlider(float rot, WdgRotAxis axis,
			     float pos[3], unsigned int name)
{
  glPushMatrix();{
    glPopName();
    glPopName();
    glLoadName((GLuint) id);
    glPushName((GLuint) name);
    
    glTranslatef(pos[0], pos[1], pos[2]);
    
    if(axis == WdgXAxis) glRotatef(rot, 1, 0, 0);
    else if(axis == WdgYAxis) glRotatef(rot, 0, 1, 0);
    else glRotatef(rot, 0, 0, 1);
    
    gluCylinder(qobj, ballRad*1.1, ballRad*1.1, ballRad*1.1, barSlice, 2);
    gluDisk(qobj, 0, ballRad*1.1, barSlice, 1);
    glTranslatef(0, 0, ballRad*1.1);
    gluDisk(qobj, 0, ballRad*1.1, barSlice, 1);
  }
  glPopMatrix();
}

//================================================================= pick
//======================================================================
int  LevWidget::pick(int data1, int data2, int data3,
		     float x, float y, float z)
{
  if(data1 != id){
    if(next) return next->pick(data1, data2, data3, x,y,z);
    return 0;
  }
  pickedObj = data2;
  picked = 1;
  //cerr << "picked levoy #" << id << endl;
  return 1;
}

int  LevWidget::pick()
{
  if(gluvv.pick.data1 != id){
    if(next) return next->pick();
    return 0;
  }
  pickedObj = gluvv.pick.data2;
  picked = 1;
  //cerr << "picked levoy #" << id << " , " << pickedObj <<  endl;
  return 1;
}

//================================================================ mouse
//======================================================================
int  LevWidget::mouse(int button, int state,
		      int x, int y)
{
  if(!picked)
    if(next) return next->mouse(button, state, x, y);


  lastwinpos[0] = x;
  lastwinpos[1] = y;

  if((button == GLUT_LEFT_BUTTON)&&
     (state == GLUT_DOWN)&&
     (glutGetModifiers() && gluvv.mouse.shift)){
    if(type == LWtriangle){
      type = LWsquare;
      verts[0][1] = thresh[1];
      thresh[1] = verts[0][1] + (verts[2][1]-verts[0][1])/2;
      thresh[0] = verts[1][0] + (verts[2][0]-verts[1][0])/2;
      gluvv.tf.loadme = 1;
    }
    else if(type == LWsquare){ 
      type = LWdef;
      gluvv.tf.loadme = 1;
    }
    else if(type == LWdef){
      type = LW1d;
      gluvv.tf.loadme = 1;
    }
    else if(type == LW1d){
      type = LWtriangle;
      verts[0][0] = verts[1][0] + (verts[2][0]-verts[1][0])/2.0;
      thresh[1] = verts[0][1];
      verts[0][1] = 0;
      gluvv.tf.loadme = 1;
    }
  }

  if((button == GLUT_LEFT_BUTTON)&&
     (state == GLUT_DOWN) &&
     (pickedObj == trap)  &&
     (!gluvv.mouse.ctrl)){
    mode = LWcolorMode;
    cerr << "mode " << mode << endl;
  }
  if((button == GLUT_MIDDLE_BUTTON)&&
     (state == GLUT_DOWN) &&
     (pickedObj == trap)  &&
     (!gluvv.mouse.ctrl)){
    mode = LWscatterMode;
    cerr << "mode " << mode << endl;
  }
  if((button == GLUT_RIGHT_BUTTON)&&
     (state == GLUT_DOWN) &&
     (pickedObj == trap)){
    mode = LWclearMode;
    cerr << "mode " << mode << endl;
  }

  if(state == GLUT_UP){
    gluvv.volren.sampleRate = gluvv.volren.goodSamp;
    glutPostRedisplay();
    glutSetWindowTitle("Transfer Function Widget");
    mode = LWclearMode;
  }

  return 1;
}

//================================================================= move
//======================================================================
int  LevWidget::move(int x, int y)
{
  if(!picked){
    if(next) return next->move(x, y);
    return 0;
  }
	
  float sdx = (float)(x - lastwinpos[0])/(float)gluvv.win.width;
  float sdy = (float)(lastwinpos[1] - y)/(float)gluvv.win.height;
  //had to change this, should be smarter
  float d[3];
  //HACK AFTER HACK
  float pos[3] = {0,0,depth};
  subV3(d, pos, gluvv.env.eye);
  //float dist = normV3(d);
  float dist = pos[2] - gluvv.env.eye[2];
  float dx = sdx *
    (gluvv.env.frustum[1]-gluvv.env.frustum[0]) * dist/gluvv.env.clip[0];
  float dy = sdy *
    (gluvv.env.frustum[3]-gluvv.env.frustum[2]) * dist/gluvv.env.clip[0];
  //--------------------------------------------------------------------
  if((pickedObj == rball) &&                           //right ball
		(gluvv.mouse.button == GLUT_LEFT_BUTTON) &&       //left
		(gluvv.mouse.state == GLUT_DOWN)){                //down
    float tmpwr = verts[2][0];  //save width just in case
    float tmpwl = verts[1][0];
    verts[2][0] += dx/width;                           //move both balls
    verts[2][0] = verts[2][0] < 1 ?                    //in oposite dirs
      (verts[2][0]>verts[1][0] ? verts[2][0] : verts[1][0]) : 1;
    verts[1][0] -= dx/width;
    verts[1][0] = verts[1][0] > 0 ? 
      (verts[1][0]<verts[2][0] ? verts[1][0] : verts[2][0]) : 0;
    if((type == LWsquare)||(type == LWdef)||(type == LW1d)){
      float tmph = verts[2][1];  //save height to adjust thresh
      verts[2][1] += dy/height;
      verts[2][1] = verts[2][1] < 1 ? 
				(verts[2][1]>thresh[1] ? verts[2][1] : thresh[1]) : 1;
      verts[1][1] = verts[2][1];  //move both balls verticaly
      thresh[1] = verts[0][1] +   //move threshold proportionaly 
				(thresh[1]-verts[0][1])/(tmph-verts[0][1])*(verts[2][1]-verts[0][1]); 
      thresh[0] = verts[1][0] + 
				(thresh[0]-tmpwl)/(tmpwr-tmpwl)*(verts[2][0]-verts[1][0]); 
    }
    
    gluvv.volren.sampleRate = gluvv.volren.interactSamp;
    glutSetWindowTitle("Transfer Function Widget: MOVE");
    glutPostRedisplay();
    gluvv.tf.loadme = 1;
  }//-------------------------------------------------------------------
  else if((pickedObj == bball) &&                      //bottom ball
		(gluvv.mouse.button == GLUT_LEFT_BUTTON) &&  //left
		(gluvv.mouse.state == GLUT_DOWN)){           //down
    
    float tmpwr = verts[2][0];  //save width just in case
    float tmpwl = verts[1][0];
    verts[0][0] += dx/width;                     //update height
    verts[0][0] = verts[0][0] < 1.0 ?            //clamp it
      (verts[0][0] > 0 ? verts[0][0] : 0) : 1.0;
    verts[2][0] += dx/width;
    verts[2][0] = verts[2][0] < 1 ? 
      (verts[2][0]>verts[1][0] ? verts[2][0] : verts[1][0]) : 1;
    
    if(type == LWtriangle){
      verts[1][0] += dx/width;                     //move top bar horisontal
      verts[1][0] = verts[1][0] > 0 ?              //clamp width
				(verts[1][0]<verts[2][0] ? verts[1][0] : verts[2][0]) : 0;
    }
		
    if((type == LWsquare)||(type == LWdef)||(type == LW1d)){
      float tmp = verts[0][1];
      verts[0][1] += dy/height;
      verts[0][1] = verts[0][1]>0 ? 
				(verts[0][1] < thresh[1] ? verts[0][1] : thresh[1]) : 0;
      // t = bottom + (t-oldb)/(top - oldb) * (top - newb)
      thresh[1] = verts[0][1] + 
				(thresh[1]-tmp)/(verts[1][1]-tmp)*(verts[1][1] - verts[0][1]);
      thresh[0] = verts[1][0] + 
				(thresh[0]-tmpwl)/(tmpwr-tmpwl)*(verts[2][0]-verts[1][0]); 
    }
    
    gluvv.volren.sampleRate = gluvv.volren.interactSamp;
    //glutSetWindowTitle("Transfer Function Widget: Primary Value");
    char str[255];
    sprintf(str, "Transfer Function Widget: Primary Value %f", verts[0][0]);
    glutSetWindowTitle(str);
    glutPostRedisplay();
    gluvv.tf.loadme = 1;
  }//----------------------------------------------------------------
  else if((pickedObj == tbar) &&                       //topbar
		(gluvv.mouse.button == GLUT_LEFT_BUTTON) &&  //left
		(gluvv.mouse.state == GLUT_DOWN)){           //down
    float w = (verts[2][0]-verts[1][0])/2.0;
    float h = verts[2][1];
		
    verts[1][0] += dx/width;                     //move left ball
    verts[1][1] += dy/height;
    verts[1][1] = verts[1][1] < 1.0 ?            //clamp left height
      (verts[1][1] > thresh[1] ? verts[1][1] : thresh[1]) : 1.0;
    verts[2][0] += dx/width;                     //move right ball
    verts[2][1] += dy/height;
    verts[2][1] = verts[2][1] < 1.0 ?            //clamp right height
      (verts[2][1] > thresh[1] ? verts[2][1] : thresh[1]) : 1.0;
		
    if((type == LWsquare)||(type == LWdef)||(type == LW1d)){
      verts[0][1] += dy/height;
      verts[0][1] = verts[0][1]>0 ? 
				(verts[0][1] < thresh[1] ? verts[0][1] : thresh[1]) : 0;
    }
		
    float W = verts[2][1] * (w/h);               //scale width of bar
		
    if(type == LWtriangle){
      verts[1][0] -= W-w;
      verts[2][0] += W-w;
    }
		
    verts[1][0] = verts[1][0] < 1.0 ?            //clamp left width
      (verts[1][0] > 0 ? verts[1][0] : 0) : 1.0;
    verts[2][0] = verts[2][0] < 1.0 ?            //clamp right width
      (verts[2][0] > 0 ? verts[2][0] : 0) : 1.0;
    //if square, thresh moves with bar
    if((type == LWsquare)||(type == LWdef)||(type == LW1d)){      
      thresh[1] += dy/height;
      thresh[1] = thresh[1] > 0 ?                        //clamp threshhold
				(thresh[1] < verts[2][1] ? thresh[1] : verts[2][1]) : 0;
      thresh[0] += dx/width;
      thresh[0] = thresh[0] > verts[1][0] ?
				(thresh[0] < verts[2][0] ? thresh[0] : verts[2][0]) : verts[1][0];
    }
		
    gluvv.volren.sampleRate = gluvv.volren.interactSamp;
    glutSetWindowTitle("Transfer Function Widget: MOVE");
    glutPostRedisplay();
    gluvv.tf.loadme = 1;
  }//-----------------------------------------------------------------
  else if((pickedObj == slider) &&                     //thresh slider
		(gluvv.mouse.button == GLUT_LEFT_BUTTON) &&  //left
		(gluvv.mouse.state == GLUT_DOWN)){           //down
    thresh[1] += dy/height;
    thresh[1] = thresh[1] < verts[1][1] ? 
      (thresh[1] > verts[0][1] ? thresh[1] : verts[0][1]) : verts[1][1];
    thresh[0] += dx/width;
    thresh[0] = thresh[0] > verts[1][0] ?
      (thresh[0] < verts[2][0] ? thresh[0] : verts[2][0]) : verts[1][0];
		
    gluvv.volren.sampleRate = gluvv.volren.interactSamp;
    glutSetWindowTitle("Transfer Function Widget: Threshold");
    glutPostRedisplay();
    gluvv.tf.loadme = 1;
  }//------------------------------------------------------------------
  else if((pickedObj == trap) &&                       //trapizoid
		(gluvv.mouse.button == GLUT_LEFT_BUTTON) &&  //left
		(gluvv.mouse.state == GLUT_DOWN)){           //down
    cpick.updateHL((x-lastwinpos[0])/(float)gluvv.win.width, 
			(lastwinpos[1]-y)/(float)gluvv.win.height);
    cpick.getColor(color);                             //color
    gluvv.volren.sampleRate = gluvv.volren.interactSamp;
    //char str[255];
    //sprintf(str, "",h,s,v);
    glutSetWindowTitle("Transfer Function Widget: Color");
    glutPostRedisplay();
    gluvv.tf.loadme = 1;
  }//-----------------------------------------------------------------
  else if((pickedObj == trap) &&                       //trapizoid
		(gluvv.mouse.button == GLUT_RIGHT_BUTTON) &&       //right
		(gluvv.mouse.state == GLUT_DOWN) &&                //down
    (!gluvv.mouse.ctrl)){                              //NO ctrl
    alpha += (lastwinpos[1]-y)/(float)gluvv.win.height;
    alpha = alpha > 1 ? 1 : (alpha<0 ? 0 : alpha);
    gluvv.volren.sampleRate = gluvv.volren.interactSamp;
    char str[255];
    sprintf(str, "Transfer Function Widget: Alpha %f", alpha);
    glutSetWindowTitle(str);
    glutPostRedisplay();
    gluvv.tf.loadme = 1;
  }//----------------------------------------------------------------
    else if((pickedObj == trap) &&                       //trapizoid
		(gluvv.mouse.button == GLUT_RIGHT_BUTTON) &&       //right
		(gluvv.mouse.state == GLUT_DOWN) &&                //down
    (gluvv.mouse.ctrl)){                              //NO ctrl
    scatter += (lastwinpos[1]-y)/(float)gluvv.win.height;
    scatter = scatter > 1 ? 1 : (scatter<0 ? 0 : scatter);
    gluvv.volren.sampleRate = gluvv.volren.interactSamp;
    char str[255];
    sprintf(str, "Transfer Function Widget: Albedo %f", scatter);
    glutSetWindowTitle(str);
    glutPostRedisplay();
    gluvv.tf.loadme = 1;
  }//----------------------------------------------------------------
  lastwinpos[0] = x;
  lastwinpos[1] = y;
	
  return 1;
}

//============================================================== release
//======================================================================
int  LevWidget::release()
{
  picked = 0;
  pickedObj = -1;
  if(next) return next->release();

  return 0;
}

//============================================================ rasterize
//======================================================================
void LevWidget::rasterize(unsigned char *tex, unsigned char *auxTex)  
{
  if(auxTex) cerr << "aux tex getting rasterized" << endl;
	if((gluvv.dmode == GDM_V1)||(gluvv.dmode == GDM_VGH_V)){
		verts[0][1] = 0;
		verts[1][1] = 1;
		verts[2][1] = 1;
		type = LW1d;
	}

  if(!on){ 
    if(next) next->rasterize(tex);
    return;
  }
	if(next) next->rasterize(tex);

  //for rasterizing triangles
  int sh = gluvv.tf.ptexsz[2];                    //sizes along dimensions
  int sg = gluvv.tf.ptexsz[1];
  int sv = gluvv.tf.ptexsz[0];
  int H = (int)(verts[1][1]*sg)-1;                //[0..height-1]
	
  int sth = sg*sv*4;                              //strides
  int stg = sv*4;    
  int stv = 4;       //rgba
	
  int base = (int)(thresh[1]*sg);

	//--------------------- triangle -------------------------------------
	//--------------------------------------------------------------------
  if(type == LWtriangle)
  {
    for(int k=0; k<sh; ++k)
    {    //for each sheet of h
      float alphaScale;
      if(k!=1) alphaScale = be;
      else alphaScale = 1;
      for(int i= base; i<H+1; ++i)
      {    //for each scan line in sheet
				//start = upper left + proportion of distance to center v/s height
				// x = (y-b)/m
				int start = (int)((verts[0][0] + (i/(float)sg)*
					(verts[1][0]-verts[0][0])/verts[1][1]) * sv);  
				int fin = (int)((verts[0][0] + (i/(float)sg)*
					(verts[2][0]-verts[0][0])/verts[1][1]) * sv) + 1;  
				
				fin -= start; //just the width please
				
				for(int j=0; j<fin; ++j)
        {    //for each pixel in line
					
					int offset = k*sth + i*stg + (start+j)*stv; //the kth sheet, 
					//ith line,
					//jth pixel
					//NOTE:right here would change if blend operation changed
					float tmpa = affine(-1, j, (fin), -1, 1);     //rampval
					tmpa = tmpa < 0 ? (1.0 + tmpa) : (1.0 - tmpa);
					float cs = tmpa;
					tmpa *= alpha; //new alpha value
          if(gluvv.shade == gluvvShadeFaux)
          {
						float tmpta = tex[offset + 3]/255.0;						
						tex[offset+0] = (tmpta*tex[offset + 0]/255.0 + (tmpa*cs)*color[0])/(tmpta+tmpa)*255;
						tex[offset+1] = (tmpta*tex[offset + 1]/255.0 + (tmpa*cs)*color[1])/(tmpta+tmpa)*255;
						tex[offset+2] = (tmpta*tex[offset + 2]/255.0 + (tmpa*cs)*color[2])/(tmpta+tmpa)*255;
          } 
          else 
          {
						float tmpta = tex[offset + 3]/255.0;						
						tex[offset+0] = (tmpta*tex[offset + 0]/255.0 + (tmpa)*color[0])/(tmpta+tmpa)*255;
						tex[offset+1] = (tmpta*tex[offset + 1]/255.0 + (tmpa)*color[1])/(tmpta+tmpa)*255;
						tex[offset+2] = (tmpta*tex[offset + 2]/255.0 + (tmpa)*color[2])/(tmpta+tmpa)*255;
          }
          if(auxTex)
          {
            //albedo, sOut = s1 * (a1/(a1+a2)) + s2 * (a2/(a1+a2)) 
            auxTex[offset + 0] = 255;//((scatter * tmpa/(tmpa + tex[offset+3]/255.0)) + 
            auxTex[offset + 1] = 255;//((scatter * tmpa/(tmpa + tex[offset+3]/255.0)) + 
            auxTex[offset + 2] = 255;//((scatter * tmpa/(tmpa + tex[offset+3]/255.0)) + 
            auxTex[offset + 3] = 255;//((scatter * tmpa/(tmpa + tex[offset+3]/255.0)) + 
              //(auxTex[offset + 0]/255.0 * tex[offset+3]/(tmpa*255 + tex[offset + 3]))) *255;
          }
					tex[offset+3] = MAX(tmpa*255,tex[offset+3]) * alphaScale;
						//(tmpa*255 + (1.0-tmpa)*tex[offset+3])*alphaScale;
				}
      }
    }
  }
	//--------------------- ellipse -------------------------------------
	//--------------------------------------------------------------------
  else if((type == LWsquare)){ //elipse inside square
    //clean this shit up!!!!
    int W = (verts[2][0]-verts[1][0])*sv;   //width
    int h = (verts[0][1]-verts[1][1])*sg;   //real height
    int hc = (thresh[0] - verts[1][0])*sv;  //horisontal center
    int VC = (thresh[1])*sg;  //vertical center
    float maxd;
    float scaleh;
    float scalew;
    if(W*W < h*h){
      maxd = (W/2)*(W/2);
      scalew = 1.0;
      scaleh = (W/2*W/2)/(float)(h/2*h/2);
    }
    else{
      maxd = (h/2)*(h/2);
      scaleh = 1.0;
      scalew = (h/2*h/2)/(float)(W/2*W/2);
    }
		
    for(int k=0; k < sh; ++k){
      float alphaScale;
      if(k!=1) alphaScale = be;
      else     alphaScale = 1;
      for(int i=verts[0][1]*sg; i< (int)(thresh[1]*sg); ++i){
				for(int j=0; j< hc; ++j){
					int offset = k*sth + i*stg + ((int)(verts[1][0]*sv) + j)*stv;
					float d = ((i-VC) * (i-VC)*scaleh) + ((j-hc) * (j-hc)*scalew);
					float tmpa = (affine(0, d, maxd, 1, 0));
					float cs = tmpa;
					tmpa = tmpa > 0 ? (tmpa < 1 ? tmpa*tmpa*alpha : alpha) : 0;
					if(gluvv.shade == gluvvShadeFaux){
						float tmpta = tex[offset + 3]/255.0;						
						tex[offset+0] = (tmpta*tex[offset + 0]/255.0 + (tmpa*cs)*color[0])/(tmpta+tmpa)*255;
						tex[offset+1] = (tmpta*tex[offset + 1]/255.0 + (tmpa*cs)*color[1])/(tmpta+tmpa)*255;
						tex[offset+2] = (tmpta*tex[offset + 2]/255.0 + (tmpa*cs)*color[2])/(tmpta+tmpa)*255;
					} else {
						float tmpta = tex[offset + 3]/255.0;						
						tex[offset+0] = (tmpta*tex[offset + 0]/255.0 + (tmpa)*color[0])/(tmpta+tmpa)*255;
						tex[offset+1] = (tmpta*tex[offset + 1]/255.0 + (tmpa)*color[1])/(tmpta+tmpa)*255;
						tex[offset+2] = (tmpta*tex[offset + 2]/255.0 + (tmpa)*color[2])/(tmpta+tmpa)*255;
          }
          if(auxTex)
          {
            //albedo, sOut = s1 * (a1/(a1+a2)) + s2 * (a2/(a1+a2)) 
            auxTex[offset + 0] = ((scatter * tmpa/(tmpa + tex[offset+3]/255.0)) + 
              (auxTex[offset + 0]/255 * tex[offset+3]/(tmpa*255 + tex[offset + 3])))*255;
          }
					tex[offset+3] = 
						(tmpa*255 + (1.0-tmpa)*tex[offset+3])*alphaScale;
				}
				for(j=hc; j< W; ++j){
					int offset = k*sth + i*stg + ((int)(verts[1][0]*sv) + j)*stv;
					float d = ((i-VC) * (i-VC) *scaleh) + ((j-hc) * (j-hc)*scalew);
					float tmpa = (affine(0, d, maxd, 1, 0));
					float cs = tmpa;
					tmpa = tmpa > 0 ? (tmpa < 1 ? tmpa*tmpa*alpha : alpha) : 0;
					if(gluvv.shade == gluvvShadeFaux){
						float tmpta = tex[offset + 3]/255.0;						
						tex[offset+0] = (tmpta*tex[offset + 0]/255.0 + (tmpa*cs)*color[0])/(tmpta+tmpa)*255;
						tex[offset+1] = (tmpta*tex[offset + 1]/255.0 + (tmpa*cs)*color[1])/(tmpta+tmpa)*255;
						tex[offset+2] = (tmpta*tex[offset + 2]/255.0 + (tmpa*cs)*color[2])/(tmpta+tmpa)*255;
					} else {
						float tmpta = tex[offset + 3]/255.0;						
						tex[offset+0] = (tmpta*tex[offset + 0]/255.0 + (tmpa)*color[0])/(tmpta+tmpa)*255;
						tex[offset+1] = (tmpta*tex[offset + 1]/255.0 + (tmpa)*color[1])/(tmpta+tmpa)*255;
						tex[offset+2] = (tmpta*tex[offset + 2]/255.0 + (tmpa)*color[2])/(tmpta+tmpa)*255;
          }
          if(auxTex)
          {
            //albedo, sOut = s1 * (a1/(a1+a2)) + s2 * (a2/(a1+a2)) 
            auxTex[offset + 0] = ((scatter * tmpa/(tmpa + tex[offset+3]/255.0)) + 
              (auxTex[offset + 0]/255.0 * tex[offset+3]/(tmpa*255 + tex[offset + 3])))*255;
          }
					tex[offset+3] = 
						(tmpa*255 + (1.0-tmpa)*tex[offset+3])*alphaScale;
				}
      }
      int start = (int)(thresh[1]*sg);
      //if(i == start) 
      //start += 1;
      for(i=start; i< H+1; ++i){
				for(int j=0; j<hc; ++j){
					int offset = k*sth + i*stg + ((int)(verts[1][0]*sv) + j)*stv;
					float d = ((i-VC) * (i-VC) *scaleh) + ((j-hc) * (j-hc) *scalew);
					float tmpa = (affine(0, d, maxd, 1, 0));
					float cs = tmpa;
					tmpa = tmpa > 0 ? (tmpa < 1 ? tmpa*tmpa*alpha : alpha) : 0;
					if(gluvv.shade == gluvvShadeFaux){
						float tmpta = tex[offset + 3]/255.0;						
						tex[offset+0] = (tmpta*tex[offset + 0]/255.0 + (tmpa*cs)*color[0])/(tmpta+tmpa)*255;
						tex[offset+1] = (tmpta*tex[offset + 1]/255.0 + (tmpa*cs)*color[1])/(tmpta+tmpa)*255;
						tex[offset+2] = (tmpta*tex[offset + 2]/255.0 + (tmpa*cs)*color[2])/(tmpta+tmpa)*255;
					} else {
						float tmpta = tex[offset + 3]/255.0;						
						tex[offset+0] = (tmpta*tex[offset + 0]/255.0 + (tmpa)*color[0])/(tmpta+tmpa)*255;
						tex[offset+1] = (tmpta*tex[offset + 1]/255.0 + (tmpa)*color[1])/(tmpta+tmpa)*255;
						tex[offset+2] = (tmpta*tex[offset + 2]/255.0 + (tmpa)*color[2])/(tmpta+tmpa)*255;
          }
          if(auxTex)
          {
            //albedo, sOut = s1 * (a1/(a1+a2)) + s2 * (a2/(a1+a2)) 
            auxTex[offset + 0] = ((scatter * tmpa/(tmpa + tex[offset+3]/255.0)) + 
              (auxTex[offset + 0]/255.0 * tex[offset+3]/(tmpa*255 + tex[offset + 3])))*255;
          }
					tex[offset+3] = 
						(tmpa*255 + (1.0-tmpa)*tex[offset+3])*alphaScale;
				}
				for(j=hc; j<W; ++j){
					int offset = k*sth + i*stg + ((int)(verts[1][0]*sv) + j)*stv;
					float d = ((i-VC) * (i-VC) * scaleh) + ((j-hc) * (j-hc) * scalew);
					float tmpa = (affine(0, d, maxd, 1, 0));
					float cs = tmpa;
					tmpa = tmpa > 0 ? (tmpa < 1 ? tmpa*tmpa*alpha : alpha) : 0;
					if(gluvv.shade == gluvvShadeFaux){
						float tmpta = tex[offset + 3]/255.0;						
						tex[offset+0] = (tmpta*tex[offset + 0]/255.0 + (tmpa*cs)*color[0])/(tmpta+tmpa)*255;
						tex[offset+1] = (tmpta*tex[offset + 1]/255.0 + (tmpa*cs)*color[1])/(tmpta+tmpa)*255;
						tex[offset+2] = (tmpta*tex[offset + 2]/255.0 + (tmpa*cs)*color[2])/(tmpta+tmpa)*255;
					} else {
						float tmpta = tex[offset + 3]/255.0;						
						tex[offset+0] = (tmpta*tex[offset + 0]/255.0 + (tmpa)*color[0])/(tmpta+tmpa)*255;
						tex[offset+1] = (tmpta*tex[offset + 1]/255.0 + (tmpa)*color[1])/(tmpta+tmpa)*255;
						tex[offset+2] = (tmpta*tex[offset + 2]/255.0 + (tmpa)*color[2])/(tmpta+tmpa)*255;
          }
          if(auxTex)
          {
            //albedo, sOut = s1 * (a1/(a1+a2)) + s2 * (a2/(a1+a2)) 
            auxTex[offset + 0] = ((scatter * tmpa/(tmpa + tex[offset+3]/255.0)) + 
              (auxTex[offset + 0]/255 * tex[offset+3]/(tmpa*255 + tex[offset + 3])))*255;
          }
					tex[offset+3] = 
						(tmpa*255 + (1.0-tmpa)*tex[offset+3])*alphaScale;
				}
      }
    }
  }
	//--------------------- 1D -------------------------------------------
	//--------------------------------------------------------------------
  else if(type == LW1d){  //1D style tf
    int W = (verts[2][0]-verts[1][0])*sv;   //width
    int h = (verts[0][1]-verts[1][1])*sg;   //real height
    int hc = (thresh[0] - verts[1][0])*sv;  //horisontal center
		float vthresh = (thresh[1]-verts[0][1])/(verts[1][1]-verts[0][1]);
		
		cerr << "vthresh = " << vthresh << "   hthresh = " << hc << endl;
		
    for(int k=0; k<sh; ++k){    //for each sheet of h
      for(int i= verts[0][1]*sg; i<H+1; ++i){    //for each scan line in sheet
				int start = verts[1][0]*sv;
				int dist   = verts[2][0]*sv - start;

				int hc0 = hc * (1.0 - vthresh) + 1;

				for(int j=0; j<hc0; ++j){    //for each pixel in line (first part)
					
					int offset = k*sth + i*stg + (start+j)*stv; //the kth sheet, 
					//ith line,
					//jth pixel
					//NOTE:right here would change if blend operation changed
					float tmpa = affine(0, j, (hc0), 0, 1);     //rampval
					//tmpa = tmpa < 0 ? (1.0 + tmpa) : (1.0 - tmpa);
					float cs = tmpa;
					tmpa *= alpha; //new alpha value
					if(tmpa > 1) cerr << "bang" << tmpa << endl; //debug check (removeme)
					if(gluvv.shade == gluvvShadeFaux){
						float tmpta = tex[offset + 3]/255.0;						
						tex[offset+0] = (tmpta*tex[offset + 0]/255.0 + (tmpa*cs)*color[0])/(tmpta+tmpa)*255;
						tex[offset+1] = (tmpta*tex[offset + 1]/255.0 + (tmpa*cs)*color[1])/(tmpta+tmpa)*255;
						tex[offset+2] = (tmpta*tex[offset + 2]/255.0 + (tmpa*cs)*color[2])/(tmpta+tmpa)*255;
					} else {
						float tmpta = tex[offset + 3]/255.0;						
						tex[offset+0] = (tmpta*tex[offset + 0]/255.0 + (tmpa)*color[0])/(tmpta+tmpa)*255;
						tex[offset+1] = (tmpta*tex[offset + 1]/255.0 + (tmpa)*color[1])/(tmpta+tmpa)*255;
						tex[offset+2] = (tmpta*tex[offset + 2]/255.0 + (tmpa)*color[2])/(tmpta+tmpa)*255;
          }
          if(auxTex)
          {
            //albedo, sOut = s1 * (a1/(a1+a2)) + s2 * (a2/(a1+a2)) 
            auxTex[offset + 0] = ((scatter * tmpa/(tmpa + tex[offset+3]/255.0)) + 
              (auxTex[offset + 0]/255.0 * tex[offset+3]/(tmpa*255 + tex[offset + 3])))*255;
          }
					tex[offset+3] = 
						(tmpa*255 + (1.0-tmpa)*tex[offset+3]);
				}

				int hc1 = dist - (dist - hc) * (1.0 - vthresh) + 1;
				
				for(j=hc0; j<hc1; ++j){    //for each pixel in line (middle part)
					
					int offset = k*sth + i*stg + (start+j)*stv; //the kth sheet, 
					//ith line,
					//jth pixel
					//NOTE:right here would change if blend operation changed
					float tmpa = 1;     //rampval
					//tmpa = tmpa < 0 ? (1.0 + tmpa) : (1.0 - tmpa);
					float cs = tmpa;
					tmpa *= alpha; //new alpha value
					if(tmpa > 1) cerr << "bang" << tmpa << endl; //debug check (removeme)
					if(gluvv.shade == gluvvShadeFaux){
						float tmpta = tex[offset + 3]/255.0;						
						tex[offset+0] = (tmpta*tex[offset + 0]/255.0 + (tmpa*cs)*color[0])/(tmpta+tmpa)*255;
						tex[offset+1] = (tmpta*tex[offset + 1]/255.0 + (tmpa*cs)*color[1])/(tmpta+tmpa)*255;
						tex[offset+2] = (tmpta*tex[offset + 2]/255.0 + (tmpa*cs)*color[2])/(tmpta+tmpa)*255;
					} else {
						float tmpta = tex[offset + 3]/255.0;						
						tex[offset+0] = (tmpta*tex[offset + 0]/255.0 + (tmpa)*color[0])/(tmpta+tmpa)*255;
						tex[offset+1] = (tmpta*tex[offset + 1]/255.0 + (tmpa)*color[1])/(tmpta+tmpa)*255;
						tex[offset+2] = (tmpta*tex[offset + 2]/255.0 + (tmpa)*color[2])/(tmpta+tmpa)*255;
          }
          if(auxTex)
          {
            //albedo, sOut = s1 * (a1/(a1+a2)) + s2 * (a2/(a1+a2)) 
            auxTex[offset + 0] = ((scatter * tmpa/(tmpa + tex[offset+3]/255.0)) + 
              (auxTex[offset + 0]/255.0 * tex[offset+3]/(tmpa*255 + tex[offset + 3])))*255;
          }
					tex[offset+3] = 
						(tmpa*255 + (1.0-tmpa)*tex[offset+3]);
				}


				for(j=hc1; j<dist; ++j){    //for each pixel in line (second part)
					
					int offset = k*sth + i*stg + (start+j)*stv; //the kth sheet, 
					//ith line,
					//jth pixel
					//NOTE:right here would change if blend operation changed
					float tmpa = affine(hc1, j, (dist), 1, 0);     //rampval
					//tmpa = tmpa < 0 ? (1.0 + tmpa) : (1.0 - tmpa);
					float cs = tmpa;
					tmpa *= alpha; //new alpha value
					if(tmpa > 1) cerr << "bang" << tmpa << endl; //debug check (removeme)
					if(gluvv.shade == gluvvShadeFaux){
						float tmpta = tex[offset + 3]/255.0;						
						tex[offset+0] = (tmpta*tex[offset + 0]/255.0 + (tmpa*cs)*color[0])/(tmpta+tmpa)*255;
						tex[offset+1] = (tmpta*tex[offset + 1]/255.0 + (tmpa*cs)*color[1])/(tmpta+tmpa)*255;
						tex[offset+2] = (tmpta*tex[offset + 2]/255.0 + (tmpa*cs)*color[2])/(tmpta+tmpa)*255;
					} else {
						float tmpta = tex[offset + 3]/255.0;						
						tex[offset+0] = (tmpta*tex[offset + 0]/255.0 + (tmpa)*color[0])/(tmpta+tmpa)*255;
						tex[offset+1] = (tmpta*tex[offset + 1]/255.0 + (tmpa)*color[1])/(tmpta+tmpa)*255;
						tex[offset+2] = (tmpta*tex[offset + 2]/255.0 + (tmpa)*color[2])/(tmpta+tmpa)*255;
          }
          if(auxTex)
          {
            //albedo, sOut = s1 * (a1/(a1+a2)) + s2 * (a2/(a1+a2)) 
            auxTex[offset + 0] = ((scatter * tmpa/(tmpa + tex[offset+3]/255.0)) + 
              (auxTex[offset + 0]/255.0 * tex[offset+3]/(tmpa*255 + tex[offset + 3])))*255;
          }
					tex[offset+3] = 
						(tmpa*255 + (1.0-tmpa)*tex[offset+3]);
				}
      }
    }
		
  }
	//--------------------- default --------------------------------------
	//--------------------------------------------------------------------
  else if(type == LWdef){
    int W = (verts[2][0]-verts[1][0])*sv;   //width
    int h = (verts[0][1]-verts[1][1])*sg;   //real height
    int hc = (thresh[0] - verts[1][0])*sv;  //horisontal center
		
    float m = (thresh[1]-verts[0][1])/(verts[1][1]-verts[0][1]);
		
    for(int k=0; k<sh; ++k){    //for each sheet of h
      float alphaScale;
      if(k!=1) alphaScale = be;
      else     alphaScale = 1;
      
      int start = verts[1][0]*sv;
      int fin = verts[2][0]*sv;
      fin -= start; //just the width please
			
      float dc = 1/((verts[1][0]-verts[2][0])*sv-1);
			
      for(int i= verts[0][1]*sg; i<H+1; ++i){    //for each scan line in sheet
				float tmpa = (((i/255.0)-verts[0][1])/(m + (i/255.0)-verts[0][1]));
				tmpa *= alpha; //new alpha value
				tmpa = tmpa > 1 ? 1 :(tmpa < 0 ? 0 : tmpa);
			  float cs = tmpa;
				if(tmpa > 1) cerr << "bang" << tmpa << endl; //debug check (removeme)
				cpick.reset(0, 1, .5);//reset the color
				
				for(int j=0; j<fin; ++j){    //for each pixel in line
					float cl[3];
					cpick.updateHL(dc, 0);
					cpick.getColor(cl);
					int offset = k*sth + i*stg + (start+j)*stv; //the kth sheet, 
					//ith line,
					//jth pixel
					//NOTE:right here would change if blend operation changed
					float tmpta = tex[offset + 3]/255.0;						
					tex[offset+0] = (tmpta*tex[offset + 0]/255.0 + (tmpa)*cl[0])/(tmpta+tmpa)*255;
					tex[offset+1] = (tmpta*tex[offset + 1]/255.0 + (tmpa)*cl[1])/(tmpta+tmpa)*255;
					tex[offset+2] = (tmpta*tex[offset + 2]/255.0 + (tmpa)*cl[2])/(tmpta+tmpa)*255;
          if(auxTex)
          {
            //albedo, sOut = s1 * (a1/(a1+a2)) + s2 * (a2/(a1+a2)) 
            auxTex[offset + 0] = ((scatter * tmpa/(tmpa + tex[offset+3]/255.0)) + 
              (auxTex[offset + 0]/255.0 * tex[offset+3]/(tmpa + tex[offset + 3]/255.0)))*255;
          }
					tex[offset+3] = 
						(tmpa*255 + (1.0-tmpa)*tex[offset+3])*alphaScale;
				}
      }
    }
		
  }
  //if(next) next->rasterize(tex);
}

//================================================== set ball & bar size
//======================================================================
void LevWidget::setBallBar(float ballsize, float barsize)
{
  ballsz = ballsize;
  barsz  = barsize;
  if(next)
    next->setBallBar(ballsize, barsize);
}

//=================================================== set width & height
//======================================================================
void LevWidget::setWH(float width, float height)
{
  this->width = width;
  this->height = height;
  if(next)
    next->setWH(width, height);
}

//============================================================== set pos
//======================================================================
void LevWidget::setPos(float bottom[2], float left[2], float right[2], 
		       float tw, float th)
{
  verts[0][0] = bottom[0] > 0 ? 
    (bottom[0] < 1 ? bottom[0] : 1) : 0;
  verts[0][1] = bottom[1] > 0 ?
    (bottom[1] < 1 ? bottom[1] : 1) : 0;
  verts[1][0] = left[0] > 0 ?
    (left[0] < 1 ? left[0] : 1) : 0;
  verts[1][1] = left[1] < 1 ? 
    (left[1] > 0 ? left[1] : 0) : 1;
  verts[2][0] = right[0] > 0 ?
    (right[0] < 1 ? right[0] : 1) : 0;
  verts[2][1] = right[1] < 1 ?
    (right[1] > 0 ? right[1] : 0) : 1;
  if(th == -10){
    thresh[1] = verts[0][1] + (verts[1][1] - verts[0][1])/2;
  } else {
    thresh[1] = th > verts[0][1] ? 
      (th < verts[1][1] ? th : verts[1][1]) : verts[0][1];
  }
  if(tw == -10){
    thresh[0] = verts[1][0] + (verts[2][0] - verts[1][0])/2;
  } else {
    thresh[0] = tw < verts[2][0] ?
      (tw > verts[1][0] ? tw : verts[1][0]) : verts[2][0];
  }
}

//============================================================ set depth
//======================================================================
void LevWidget::setDepth(float d){
  depth = d;
  if(next) next->setDepth(d);
}

//==================================================== get boundary emph
//======================================================================
float LevWidget::getBE()
{
  return be;
}

//==================================================== set boundary emph
//======================================================================
void LevWidget::setBE(float boundEm)
{
  be = boundEm > 0 ? (boundEm < 1 ? boundEm : 1) : 0;
}

//=============================================================== insert
//======================================================================
int LevWidget::insert(LevWidget *n)
{
  if(next){
    n->next = next;
    next = n;
  }
  else
    next = n;
  return id;
}

//================================================================== get
//======================================================================
LevWidget *LevWidget::get(int ident)
{
  if(id == ident)
    return this;
  else{
    if(next){
      return next->get(ident);
    }
    else return 0;
  }
}

//=============================================================== remove
//======================================================================
LevWidget *LevWidget::remove(int ident)
{
  if(next){
    if(next->id == ident){
      LevWidget *tmp = next;
      next = next->next;
      return tmp;
    }
    else{
      return next->remove(ident);
    }
  }
  else return 0;
}

//============================================================ set alpha
//======================================================================
void LevWidget::setAlpha(float a)
{
  alpha = a;
}

//============================================================ alphaMode
//======================================================================
void LevWidget::alphaMode()
{
  mode = LWalphaMode;
  if(next) next->alphaMode();
}
//============================================================ colorMode
//======================================================================
void LevWidget::colorMode()
{
  mode = LWcolorMode;
  if(next) next->colorMode();
}
//============================================================ clearMode
//======================================================================
void LevWidget::clearMode()
{
  mode = LWclearMode;
  if(next) next->clearMode();
}

//======================================================== triangle type
//======================================================================
void LevWidget::triangleType()
{
  type = LWtriangle;
}

//========================================================== square type
//======================================================================
void LevWidget::squareType()
{
  type = LWsquare;
}

//========================================================== square type
//======================================================================
void LevWidget::oneDType()
{
	type = LW1d;
}

//========================================================= Gl error chk
//======================================================================
int  LevWidget::LevWidgetGlErr(char *place)
{
  GLenum errCode;
  const GLubyte *errString;
  
  if((errCode = glGetError()) != GL_NO_ERROR){
    errString = gluErrorString(errCode);
    cerr << endl << "OpenGL error : LevWidget::" << place << " : " 
	 << (char *) errString << endl << endl;
    return 1;
  }
  return 0;
}




