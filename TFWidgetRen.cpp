//-------------------------------------------------------------------------
//
//   Joe Kniss
//    Transfer Function widget
//  TFWidgetRen.cpp  Handles 2&3D transfer functions 
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
//#include <NrrdC.h>
#include "gluvv.h"
#include "VectorMath.h"
#include "TFWidgetRen.h"

/* for hacktmp */
#include <math.h>



//=================================================== TFWidgetRen
//===============================================================
TFWidgetRen::TFWidgetRen(gluvvGlobal *gl)
{
  pos[0] = 0;
  pos[1] = 0;
  pos[2] = -4;
  screenwidth = .9;
  screenheight = .2;
  screenpos[0] = .45;
  screenpos[1] = -.46;

  mode = colorBlend;
 
  float d[3];
  subV3(d, pos, gluvv.env.eye);
  float dist = normV3(d);

  width  = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	    dist/gluvv.env.clip[0]) * screenwidth;
  height = ((gluvv.env.frustum[3]-gluvv.env.frustum[2]) * 
	    dist/gluvv.env.clip[0]) * screenheight;

  pos[0] = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	    dist/gluvv.env.clip[0]) * screenpos[0];
  pos[1] = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	    dist/gluvv.env.clip[0]) * screenpos[1];

  screenbar = .01;
  screenball = screenbar * 1.5;

  barRad = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	    dist/gluvv.env.clip[0]) * screenbar;

  ballRad = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	     dist/gluvv.env.clip[0]) * screenball;

  //-------- Levoy triangles -----------
  tris = new LevWidget(&gluvv);
  tris->setWH(width-ballRad*2, height-ballRad*2);
  float b[2] = {.5,0}; float l[2]={.3,.7}; float r[2] = {.7,.7};
  tris->setPos(b,l,r);
  tris->setBallBar(ballRad/2.0, barRad/2.0);
  tris->setDepth(pos[2]);
  tris->squareType();
  tris->on = 0;  //turn root triangle off

  brush= new LevWidget(&gluvv);
  brush->setWH(width-ballRad*2, height-ballRad*2);
  brush->setPos(b,l,r);
  brush->setBallBar(ballRad/2.0, barRad/2.0);
  brush->setDepth(pos[2]);
  brush->setAlpha(.2);
  brush->squareType();
  brush->on = 1;     //turn paint brush on
  brush->drawOn = 0; //but dont actualy draw the widget

  widgetInit();

  transparent();

  gluvv.tf.loadme = 1;
  gluvv.tf.paintme = 0;
  //make space for pixel texture (memory copy)
  gluvv.tf.ptexsz[0] = 256;//value
  gluvv.tf.ptexsz[1] = 256;//grad
  gluvv.tf.ptexsz[2] = 4;  //hess, four panes
  gluvv.tf.numelts   = 4;  //rgba
  glGenTextures(1, &gluvv.tf.ptextex);  //drawable texture
  //glGenTextures(1, &gluvv.tf.ptexname); //actual pixel texture
  //cerr << "the pixel texture name " << gluvv.tf.ptexname << endl;
  
  gluvv.tf.qnptexname = 0;
  if(gluvv.tf.ptex) delete[] gluvv.tf.ptex;
  gluvv.tf.ptex = new unsigned char[(gluvv.tf.ptexsz[0]*
				     gluvv.tf.ptexsz[1]*
				     gluvv.tf.ptexsz[2]*
				     gluvv.tf.numelts)];
  gluvv.tf.paintex = new unsigned char[(gluvv.tf.ptexsz[0]*
					gluvv.tf.ptexsz[1]*
					gluvv.tf.ptexsz[2]*
					gluvv.tf.numelts)];
  gluvv.tf.shadePtex = new unsigned char[(gluvv.tf.ptexsz[0]*
					  gluvv.tf.ptexsz[1]*
					  gluvv.tf.ptexsz[2]*
					  gluvv.tf.numelts)];
  clearPaintex();
  gluvv.tf.ptex[0 + 0] = 0;
  gluvv.tf.ptex[0 + 1] = 0;
  gluvv.tf.ptex[0 + 2] = 0;
  gluvv.tf.ptex[0 + 3] = 0;
  
}

//================================================== ~TFWidgetRen
//===============================================================
TFWidgetRen::~TFWidgetRen()
{

}

//========================================================== draw
//===============================================================
void TFWidgetRen::draw()
{
  if(gluvv.picking) fill();

  if(gluvv.tf.paintme){
    brush->rasterize(gluvv.tf.paintex);
    gluvv.tf.paintme = 0;
  }

  float dist = pos[2] - gluvv.env.eye[2];

  width  = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	    dist/gluvv.env.clip[0]) * screenwidth;
  height = ((gluvv.env.frustum[3]-gluvv.env.frustum[2]) * 
	    dist/gluvv.env.clip[0]) * screenheight;

  tris->setWH(width-ballRad*2, height-ballRad*2);
  brush->setWH(width-ballRad*2, height-ballRad*2);

  pos[0] = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	    dist/gluvv.env.clip[0]) * screenpos[0];
  pos[1] = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	    dist/gluvv.env.clip[0]) * screenpos[1];

  barRad = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
  	    dist/gluvv.env.clip[0]) * screenbar;

  ballRad = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	     dist/gluvv.env.clip[0]) * screenball;

  tris->setBallBar(ballRad/2.0, barRad/2.0);
  brush->setBallBar(ballRad/2.0, barRad/2.0);
  
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);

  defaultMaterial();

  gluvvPushName();
  glPushName(22);
  glPushName(222);
  glPushName(2222);
  
  glPushMatrix();{

    glTranslatef(pos[0], pos[1], pos[2]);
    glRotatef(180, 0, 1, 0);
    glDrawBuffer(GL_BACK);
    draw_widget();
    glDrawBuffer(GL_FRONT);
    draw_widget();

  } glPopMatrix();

  gluvvPopNames();
  TFWidgetRenGlErr("draw");

  glFlush();

  if(gluvv.picking) transparent();
}

//=================================================== draw widget
//===============================================================
void TFWidgetRen::draw_widget()
{

  glColor4f(.1, .07, .57, 1);                  //balls
  float spos[3] = {0,0,0};
  drawSphere(spos, blball);                    //bottom left
  spos[0] = width;
  drawSphere(spos, brball);                    //bottom right
  spos[1] = height;
  drawSphere(spos, trball);                    //top    right
  spos[0] = 0;
  drawSphere(spos, tlball);                    //top    left

  glColor4f(.09, .34, .55, 1);                 //bars
  float bpos[3] = {0,0,0};
  drawBar(-90, WdgXAxis, height, bpos, lbar);  //left
  drawBar(90, WdgYAxis, width, bpos, bbar);    //bottom
  bpos[1] = height;
  drawBar(90, WdgYAxis, width, bpos, tbar);    //top
  bpos[0] = width;
  bpos[1] = 0;
  drawBar(-90, WdgXAxis, height, bpos, rbar);  //right
  

  glColor4f(1,1,1,0);                          //transfer function map depth
  glLoadName(tfsheet1);
  //glEnable(GL_TEXTURE_3D_EXT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDisable(GL_LIGHTING);
  //glBindTexture(GL_TEXTURE_3D_EXT, gluvv.tf.ptextex);
  glBegin(GL_QUADS);{  //draw the transfer function
    glTexCoord3f(0, 0, .33333);   
    glVertex3f(barRad*1.5,    barRad*1.5,     0);
    glTexCoord3f(1, 0, .33333);   
    glVertex3f(width-barRad*1.5, barRad*1.5,     0);
    glTexCoord3f(1, 1, .33333);   
    glVertex3f(width-barRad*1.5, height-barRad*1.5, 0);
    glTexCoord3f(0, 1, .33333);   
    glVertex3f(barRad*1.5,    height-barRad*1.5,0);
  } glEnd();
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_3D_EXT);
  glEnable(GL_LIGHTING);                       //---------------------------

  draw_probe();                                //draw probe data value

  //-------------- load new pixel texture ----------------------------------
  if(gluvv.tf.loadme){                         
    clearPtex();
    tris->rasterize(gluvv.tf.ptex);
    brush->rasterize(gluvv.tf.ptex);
    loadPtex();
    gluvv.tf.loadme = 0;
  } //----------------------------------------------------------------------

  if(pickedTri){                               //boundary emphasis slider
    LevWidget * tt;
    if(tt = tris->get(pickedTri)){
      float slider = tt->getBE();
      float bepos[3] = {(width - 2*ballRad)*slider + ballRad, height, 0};
      drawSlider(90, WdgYAxis, bepos, beslider);
    }
  }

  glColor4f(1,1,1,1);                          //transfer function map color
  glLoadName(tfsheet1);
  glEnable(GL_TEXTURE_3D_EXT);
  if(mode == colorBlend){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  else 
    glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_EQUAL);
  glDisable(GL_LIGHTING);
  glBindTexture(GL_TEXTURE_3D_EXT, gluvv.tf.ptextex);
  glBegin(GL_QUADS);{  //draw the transfer function
    glTexCoord3f(0, 0, .5);   
    glVertex3f(barRad*1.5,    barRad*1.5,     0);
    glTexCoord3f(1, 0, .5);   
    glVertex3f(width-barRad*1.5, barRad*1.5,     0);
    glTexCoord3f(1, 1, .5);   
    glVertex3f(width-barRad*1.5, height-barRad*1.5, 0);
    glTexCoord3f(0, 1, .5);   
    glVertex3f(barRad*1.5,    height-barRad*1.5,0);
  } glEnd();
  glDepthFunc(GL_LESS);
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_3D_EXT);
  glEnable(GL_LIGHTING);                       //----------------------------

  glPushMatrix();{                             //draw levoy triangles
    glTranslatef(ballRad, ballRad, ballRad);
    tris->draw();
  } glPopMatrix();

  if(gluvv.picking) brush->draw();
}

//==================================================== draw probe
//===============================================================
void TFWidgetRen::draw_probe()
{
#if 0
  if(gluvv.nrrd){
    int xi = gluvv.nrrd->getSizeD(1);   //size of volume (axies)
    int yi = gluvv.nrrd->getSizeD(2);
    int zi = gluvv.nrrd->getSizeD(3);
    int px, py, pz;
    float fpos[3];
    if(!gluvv.mprobe){
      px = gluvv.probe.vpos[0] * xi;  //integer position in volume
      py = gluvv.probe.vpos[1] * yi;  //(least)
      pz = gluvv.probe.vpos[2] * zi;
      fpos[0] = gluvv.probe.vpos[0] * xi;  //floating point position in volume
      fpos[1] = gluvv.probe.vpos[1] * yi;  //
      fpos[2] = gluvv.probe.vpos[2] * zi;
      if((lastppos[0] != gluvv.probe.vpos[0])||
				(lastppos[1] != gluvv.probe.vpos[1])||
				(lastppos[2] != gluvv.probe.vpos[2]))
				gluvv.tf.loadme = 1;
      lastppos[0] = gluvv.probe.vpos[0];
      lastppos[1] = gluvv.probe.vpos[1];
      lastppos[2] = gluvv.probe.vpos[2];
    } else {
      px = gluvv.clip.mousepnt[0] * xi;  //integer position in volume
      py = gluvv.clip.mousepnt[1] * yi;  //(least)
      pz = gluvv.clip.mousepnt[2] * zi;
      fpos[0] = gluvv.clip.mousepnt[0] * xi;//floating point position in volume
      fpos[1] = gluvv.clip.mousepnt[1] * yi;  
      fpos[2] = gluvv.clip.mousepnt[2] * zi;
      if((lastppos[0] != gluvv.clip.mousepnt[0])||
				(lastppos[1] != gluvv.clip.mousepnt[1])||
				(lastppos[2] != gluvv.clip.mousepnt[2]))
				gluvv.tf.loadme = 1;
      lastppos[0] = gluvv.clip.mousepnt[0];
      lastppos[1] = gluvv.clip.mousepnt[1];
      lastppos[2] = gluvv.clip.mousepnt[2];
    }
    
    if((px < 1) || (px > (xi-2)) ||     //are we within the bounds 
			(py < 1) || (py > (yi-2)) ||     //of the volume???
			(pz < 1) || (pz > (zi-2))){
      float tmpp[2] = {0,0};
      brush->setPos(tmpp, tmpp, tmpp);
      //gluvv.tf.loadme = 1;
      return; //no, nothing to do here
    }
    int sx = gluvv.nrrd->getSizeD(0);   //strides through volume
    int sy = sx*gluvv.nrrd->getSizeD(1);
    int sz = sy*gluvv.nrrd->getSizeD(2);
		
    unsigned char *np = (unsigned char*)gluvv.nrrd->getData();
    float vpnts[8][3];  //voxel corners, tf space
    float vvals[8][3];  //voxel values, [0..1]
		
    float hacktmp;
		//get voxel corner values (position)
    for(int i=0; i<2; ++i)      //z = i
      for(int j=0; j<2; ++j)    //y = j
				for(int k=0; k<2; ++k){ //x = k
					vpnts[i*4 + j*2 + k][0] = ballRad +                //get value
						np[((pz+i)*sz) + ((py+j)*sy) + ((px+k)*sx) + 0]/
						255.0 * (width - 2*ballRad);
					vvals[i*4 + j*2 + k][0] = 
						np[((pz+i)*sz) + ((py+j)*sy) + ((px+k)*sx) + 0]/255.0;
					
					vpnts[i*4 + j*2 + k][1] = ballRad +                //get grad
						np[((pz+i)*sz) + ((py+j)*sy) + ((px+k)*sx) + 1]/
						255.0 * (height - 2*ballRad);;
					vvals[i*4 + j*2 + k][1] = 
						np[((pz+i)*sz) + ((py+j)*sy) + ((px+k)*sx) + 1]/255.0;
					//get hessian
					// vpnts[i*4 + j*2 + k][2] = 
					//   np[((pz+i)*sz) + ((py+j)*sy) + ((px+k)*sx) + 2]/255.0 * 3/2.0;
					hacktmp = np[((pz+i)*sz) + ((py+j)*sy) + ((px+k)*sx) + 2]/85.0-1;
					hacktmp = hacktmp > 0 ? sqrt(hacktmp) : -sqrt(-hacktmp);
					vpnts[i*4 + j*2 + k][2] = (hacktmp+1)/2.0;
					vvals[i*4 + j*2 + k][2] = 
						np[((pz+i)*sz) + ((py+j)*sy) + ((px+k)*sx) + 0]/169.0;
				}    
				//done getting voxel corner values
				
				if(!gluvv.picking){
					glDisable(GL_LIGHTING);
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glDisable(GL_DEPTH_TEST);
					glColor4f(1,1,1,.3);
					
					glBegin(GL_LINE_STRIP);{
						glVertex3f(vpnts[0][0], vpnts[0][1], 0);
						glVertex3f(vpnts[1][0], vpnts[1][1], 0);
						glVertex3f(vpnts[3][0], vpnts[3][1], 0);
						glVertex3f(vpnts[2][0], vpnts[2][1], 0);
						glVertex3f(vpnts[0][0], vpnts[0][1], 0);
						glVertex3f(vpnts[4][0], vpnts[4][1], 0);
						glVertex3f(vpnts[5][0], vpnts[5][1], 0);
						glVertex3f(vpnts[7][0], vpnts[7][1], 0);
						glVertex3f(vpnts[6][0], vpnts[6][1], 0);
						glVertex3f(vpnts[4][0], vpnts[4][1], 0);
					} glEnd();
					
					glBegin(GL_LINES);{
						glVertex3f(vpnts[1][0], vpnts[1][1], 0);
						glVertex3f(vpnts[5][0], vpnts[5][1], 0);
						
						glVertex3f(vpnts[3][0], vpnts[3][1], 0);
						glVertex3f(vpnts[7][0], vpnts[7][1], 0);
						
						glVertex3f(vpnts[2][0], vpnts[2][1], 0);
						glVertex3f(vpnts[6][0], vpnts[6][1], 0);
					} glEnd();
					
					glEnable(GL_DEPTH_TEST);
					glDisable(GL_BLEND);
				} //if not picking
				//draw interpolated value in data domain
				//find interp'ed value
				float val[3];
				triLerpVGH(val, vvals, fpos);             //trilinear interp for values
				
				//------------ Paint Brush ---------------------------------------
				float plpos[2] = {val[0]-.08, val[1]+.08};//paint positions
				float prpos[2] = {val[0]+.08, val[1]+.08};
				float pbpos[2] = {val[0]+.08, val[1]-.08};
				brush->setPos(pbpos, plpos, prpos, val[0], val[1]);
				glPushMatrix();{                             //draw levoy triangles
					glTranslatef(ballRad, ballRad, ballRad);
					brush->draw();
				} glPopMatrix();
				//----------------------------------------------------------------
				if(!gluvv.picking){
					//position of point in tf space:
					glPushMatrix();{
						glTranslatef(ballRad + val[0]*(width - 2*ballRad),
							ballRad + val[1]*(height - 2*ballRad),
							0);
						glColor4f(1, 0, 0, 1);
						glLoadName(dpoint);
						glutSolidSphere(ballRad/2.5, 20, 20);
					} glPopMatrix();
					//draw voxel points in data domain
					for(i=0; i<8; ++i){
						glPushMatrix();{
							glTranslatef(vpnts[i][0], 
								vpnts[i][1], 
								0);
							float bcol[3]= {don[((int)(vpnts[i][2]*22) + 1)*3 + 0]/255.0,
								don[((int)(vpnts[i][2]*22) + 1)*3 + 1]/255.0,
								don[((int)(vpnts[i][2]*22) + 1)*3 + 2]/255.0};
							glColor4f(bcol[0], bcol[1], bcol[2], 1);
							glutSolidSphere(ballRad/3, 20, 20);
						} glPopMatrix();
					}
				}
				glEnable(GL_LIGHTING);
				
  }//if(gluvv.nrrd)
}

//========================================================== pick
//===============================================================
int  TFWidgetRen::pick(int data1, int data2, int data3,
		       float x, float y, float z)
{
  fill();
  pickedObj = data3;
  glutSetWindowTitle("Transfer Function Widget");
  if(pickedObj == LEVWIDGNAME){
    tris->release();
    pickedTri = data1;
    if(pickedTri == brush->id)
      return brush->pick(data1, data2, data3, x, y, z);
    else
      return tris->pick(data1, data2, data3, x, y, z);
  }
  return 1;
}

int  TFWidgetRen::pick()
{
  fill();
  pickedObj = gluvv.pick.data3;
  glutSetWindowTitle("Transfer Function Widget");
  if(pickedObj == LEVWIDGNAME){
    tris->release();
    pickedTri = gluvv.pick.data1;
    if(pickedTri == brush->id)
      return brush->pick();
    else
      return tris->pick();
  }

  return 1;
}

//=========================================================== key
//===============================================================
int TFWidgetRen::key(unsigned char key, int x, int y)
{
  if(key == 127){ //delete key
    if(pickedObj == LEVWIDGNAME){ //you must mean delete a triangle
      if(pickedTri == tris->id){ //can't delete the root triangle!!! 
	tris->on = 0;
	gluvv.tf.loadme = 1;
	glutPostRedisplay();
	return 1;  //we used the key
      }
      LevWidget * tmplw = tris->remove(pickedTri);
      delete tmplw;
      gluvv.tf.loadme = 1;
      glutPostRedisplay();
      return 1; //key used
    }
  }
  if(key == 'p'){ //paint mode

    return 1;   //key used
  }
  if(key == 'l'){ //levoy mode

    return 1;   //key used
  }

  return 0; //we didn't use the key
}

//========================================================= mouse
//===============================================================
int  TFWidgetRen::mouse(int button, int state,
			int x, int y)
{
  if(pickedObj == LEVWIDGNAME){
    if(pickedTri == brush->id)
      return brush->mouse(button, state, x, y);
    else 
      return tris->mouse(button, state, x, y);
  }

  if(state == GLUT_DOWN){
    lastwinpos[0] = x;
    lastwinpos[1] = y;
    switch(button){
    case GLUT_LEFT_BUTTON:
      switch(pickedObj){
      case tfsheet0:
      case tfsheet1:
      case tfsheet2:
				//mode = color;
				break;
      }
      break;
			case GLUT_MIDDLE_BUTTON:
				switch(pickedObj){
				case tfsheet0:
				case tfsheet1:
				case tfsheet2:
					//mode = colorBlend;
					break;
				}
				break;
				case GLUT_RIGHT_BUTTON:
					LevWidget *lwtmp = new LevWidget(&gluvv);
					mouse2plane(x,y);
					float pnt[2] = {1-(mousepnt[0]+pos[0])/width, 
						(mousepnt[1]-pos[1])/height};
					float b[2] = {pnt[0], 0};
					float l[2] = {pnt[0]-.1, pnt[1]};
					float r[2] = {pnt[0]+.1, pnt[1]};
					lwtmp->setPos(b,l,r, l[1]*.5);
					lwtmp->setDepth(pos[2]);
					tris->insert(lwtmp);
					gluvv.tf.loadme = 1;
					break;
    }
    
  }
  else{ //state is _UP
    gluvv.rinfo.sampleRate = gluvv.rinfo.goodSamp;
    glutPostRedisplay();
    glutSetWindowTitle("Transfer Function Widget");
  }
  return 1;
}

//========================================================== move
//===============================================================
int  TFWidgetRen::move(int x, int y)
{
  if(pickedObj == LEVWIDGNAME){
    if(pickedTri == brush->id)
      return brush->move(x,y);
    else
      return tris->move(x, y);
  }
	
  float sdx = (float)(x - lastwinpos[0])/(float)gluvv.win.width;
  float sdy = (float)(y - lastwinpos[1])/(float)gluvv.win.height;
  //had to change this should be smarter
  float d[3];
  subV3(d, pos, gluvv.env.eye);
  //float dist = normV3(d);
  float dist = pos[2] - gluvv.env.eye[2];
  float dx = sdx *
    (gluvv.env.frustum[1]-gluvv.env.frustum[0]) * dist/gluvv.env.clip[0];
  float dy = sdy *
    (gluvv.env.frustum[3]-gluvv.env.frustum[2]) * dist/gluvv.env.clip[0];
  switch(gluvv.mouse.state){////////////////////////////////////
  case GLUT_DOWN:
    switch(gluvv.mouse.button){///////////////////////
    case GLUT_LEFT_BUTTON:
      switch(pickedObj){///////////////////
      case bbar:
      case tbar:
      case lbar:
      case rbar:
				screenpos[0]  -= sdx;
				screenpos[1]  -= sdy;
				gluvv.rinfo.sampleRate = gluvv.rinfo.interactSamp;
				glutSetWindowTitle("Transfer Function Widget: MOVE");
				glutPostRedisplay();
				break;
      case blball:
				screenpos[0]  -= sdx;
				screenpos[1]  -= sdy;
				screenwidth   -= sdx;
				screenheight  += sdy;
				gluvv.rinfo.sampleRate = gluvv.rinfo.interactSamp;
				glutSetWindowTitle("Transfer Function Widget: RESIZE");
				glutPostRedisplay();
				break;
      case brball:
				screenpos[1]  -= sdy;
				screenwidth   += sdx;
				screenheight  += sdy;
				gluvv.rinfo.sampleRate = gluvv.rinfo.interactSamp;
				glutSetWindowTitle("Transfer Function Widget: RESIZE");
				glutPostRedisplay();
				break;
      case tlball:
				screenpos[0]  -= sdx;
				screenwidth   -= sdx;
				screenheight  -= sdy;
				gluvv.rinfo.sampleRate = gluvv.rinfo.interactSamp;
				glutSetWindowTitle("Transfer Function Widget: RESIZE");
				glutPostRedisplay();
				break;
      case trball:
				screenwidth   += sdx;
				screenheight  -= sdy;
				gluvv.rinfo.sampleRate = gluvv.rinfo.interactSamp;
				glutSetWindowTitle("Transfer Function Widget: RESIZE");
				glutPostRedisplay();
				break;
      case beslider:
				if(pickedTri){
					LevWidget *tt;
					if(tt = tris->get(pickedTri)){
						float slider = tt->getBE();
						slider += dx/width;
						tt->setBE(slider);
						glutSetWindowTitle("Transfer Function Widget: Boundary Emphasis");
						gluvv.rinfo.sampleRate = gluvv.rinfo.interactSamp;
						glutPostRedisplay();
						gluvv.tf.loadme = 1;
					}
				}
				break;
				
      }///////////////////////////////////
      break;
			case GLUT_MIDDLE_BUTTON:
				switch(pickedObj){///////////////////
				case bbar:
				case tbar:
				case lbar:
				case rbar:
					pos[0]  -= dx;
					pos[2]  -= dy;
					gluvv.rinfo.sampleRate = gluvv.rinfo.interactSamp;
					glutSetWindowTitle("Transfer Function Widget: MOVE");
					glutPostRedisplay();
					break;
				}
				break;
				case GLUT_RIGHT_BUTTON:
					
					break;
    }///////////////////////////////////////////////
    break;
		case GLUT_UP:
			break;
  }////////////////////////////////////////////////////////////
	
  lastwinpos[0] = x;
  lastwinpos[1] = y;
	
  return 1;
}

//======================================================= release
//===============================================================
int  TFWidgetRen::release()
{
  //if(pickedObj == LEVWIDGNAME)
  //return tris->release();
  transparent();
  //pickedTri = 0;
  return 1;
}

//=================================================== mouse2plane
//===============================================================
void TFWidgetRen::mouse2plane(int x, int y)
{
  // assume we are always aligned with z axis
  // should be based on up vector and (at-eye)xup vector!!!!!
  float fx = 1 - (float)x/(float)gluvv.win.width;
  float fy = (float)(gluvv.win.height - y)/(float)gluvv.win.height;
  float vdir[3];
  subV3(vdir, gluvv.env.at, gluvv.env.eye);
  normalizeV3(vdir);
  scaleV3(gluvv.env.clip[0], vdir);
  float fcp[3]; //position of front clipping plane
  addV3(fcp, gluvv.env.eye, vdir);
  float cpnt[3];//clip plane point (projecton of screen point on clip plane)
  cpnt[0] = ((fcp[0] + gluvv.env.frustum[0]) + 
	     (gluvv.env.frustum[1] - gluvv.env.frustum[0]) * fx);
  cpnt[1] = ((fcp[1] + gluvv.env.frustum[2]) + 
	     (gluvv.env.frustum[3] - gluvv.env.frustum[2]) * fy);
  cpnt[2] = fcp[2]; //remember, assume z aligned view direction

  float ep[3];  //eye - pos
  subV3(ep, gluvv.env.eye, pos);
  float me[3];  //eye - clipplane point
  subV3(me, gluvv.env.eye, cpnt);
  float dir[3] = {0,0,-1};
  float t = dotV3(dir, ep)/dotV3(dir, me);
  negateV3(me);
  scaleV3(t,me);
  addV3(mousepnt, me, gluvv.env.eye);
}


//===================================================== clear Ptex
//================================================================
void TFWidgetRen::clearPtex()
{
  int sz = (gluvv.tf.numelts*
	    gluvv.tf.ptexsz[0]*
	    gluvv.tf.ptexsz[1]*
	    gluvv.tf.ptexsz[2]);

  for(int i=0; i<sz; ++i){
    gluvv.tf.ptex[i] = gluvv.tf.paintex[i];
  }
}

//===================================================== clear Ptex
//================================================================
void TFWidgetRen::clearPaintex()
{
  int sz = (gluvv.tf.numelts*
	    gluvv.tf.ptexsz[0]*
	    gluvv.tf.ptexsz[1]*
	    gluvv.tf.ptexsz[2]);

  for(int i=0; i<sz; ++i){
    gluvv.tf.paintex[i] = 0;
  }
}

//====================================================== load Ptex
//================================================================
void TFWidgetRen::loadPtex()
{
  if(gluvv.tf.numelts == 4){
    glEnable(GL_TEXTURE_3D_EXT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_3D_EXT, gluvv.tf.ptextex);
    
    glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_R, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage3DEXT(GL_TEXTURE_3D_EXT,  //target
		    0,                  //mip level
		    GL_RGBA,            //internal format
		    gluvv.tf.ptexsz[0], //width
		    gluvv.tf.ptexsz[1], //height
		    gluvv.tf.ptexsz[2], //depth
		    0,                  //border, never works
		    GL_RGBA,            //format
		    GL_UNSIGNED_BYTE,   //type
		    gluvv.tf.ptex);     //data
    TFWidgetRenGlErr("loading ptextex");
    glDisable(GL_TEXTURE_3D_EXT);
  
    glEnable(GL_TEXTURE_3D_EXT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    //NOTE:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // I dont think this is nessesary, but just to be sure....
    //  if it gets removed the parameters have to be set somewhere else
    // actualy, now I think that it IS important on the Octane2 platform
    glEnable(GL_PIXEL_TEXTURE_SGIS);
    glPixelTexGenParameteriSGIS(GL_PIXEL_FRAGMENT_RGB_SOURCE_SGIS,
				GL_PIXEL_GROUP_COLOR_SGIS);
    glPixelTexGenParameteriSGIS(GL_PIXEL_FRAGMENT_ALPHA_SOURCE_SGIS,
				GL_PIXEL_GROUP_COLOR_SGIS);
  // I dont think this has any effect either, try it with out binding
    glBindTexture(GL_TEXTURE_3D_EXT, gluvv.tf.ptexname);
    TFWidgetRenGlErr("GL_PIXEL_TEXTURE_SGIS");
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    
    glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_R, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage3DEXT(GL_TEXTURE_3D_EXT,  //target
		    0,                  //mip level
		    GL_RGBA,            //internal format
		    gluvv.tf.ptexsz[0], //width
		    gluvv.tf.ptexsz[1], //height
		    gluvv.tf.ptexsz[2], //depth
		    0,                  //border, never works
		    GL_RGBA,            //format
		    GL_UNSIGNED_BYTE,   //type
		    gluvv.tf.ptex);     //data

    glDisable(GL_PIXEL_TEXTURE_SGIS);
    glDisable(GL_TEXTURE_3D_EXT);
    TFWidgetRenGlErr("GL_PIXEL_TEXTURE_SGIS-post load");

    if(!gluvv.tf.qnptexname)
      glGenTextures(1, &gluvv.tf.qnptexname); //quantized normals pixel texture
    
  }
}

//===================================== trilinear interpolate VGH
//===============================================================
void TFWidgetRen::triLerpVGH(float val[3], float voxels[8][3], float pos[3])
{
  //fractional values
  float fx = pos[0] - (int)pos[0];
  float fy = pos[1] - (int)pos[1];
  float fz = pos[2] - (int)pos[2];
  
  //now interpolate to find the data value at our point
  //first allong x direction
  for(int i=0; i<3; ++i){
    float x1 = voxels[0][i] + (float)(voxels[1][i] - voxels[0][i]) * fx;
    float x2 = voxels[2][i] + (float)(voxels[3][i] - voxels[2][i]) * fx;
    float x3 = voxels[4][i] + (float)(voxels[5][i] - voxels[4][i]) * fx;
    float x4 = voxels[6][i] + (float)(voxels[7][i] - voxels[6][i]) * fx;
    //then along y direction
    float xy1 = x1 + (x2 - x1) * fy;
    float xy2 = x3 + (x4 - x3) * fy;
    //finaly allong z direction 
    val[i] = (xy1 + (xy2 - xy1) * fz);
  }

}

//====================================================== GL error
//===============================================================
int  TFWidgetRen::TFWidgetRenGlErr(char *place)
{
  GLenum errCode;
  const GLubyte *errString;
  
  if((errCode = glGetError()) != GL_NO_ERROR){
    errString = gluErrorString(errCode);
    cerr << endl << "OpenGL error : TFWidgetRen::" << place << " : " 
	 << (char *) errString << endl << endl;
    return 1;
  }
  return 0;
}

//==========================================================================
//============= *** static color map definitions *** =======================
//==========================================================================

//==========================================================================
// the "don greenburg" color bind color map, outright stolen from GLK
//   from: <~gk/usr/local/src/bane/test/pvg.c>
unsigned char don[] = {0,   0,   0,     /* background: black */
	       /* 1 */ 0,   107, 255,   /* start: blue */
		       51,  104, 255,
		       103, 117, 255,
		       123, 124, 255,
		       141, 130, 255,
		       156, 132, 255,
		       166, 131, 245,
		       174, 131, 231,
		       181, 130, 216,
		       187, 130, 201,
	      /* 11 */ 255, 255, 255,       /* middle: white */
	      /* 12 */ 255, 255, 255,
		       187, 130, 201,
		       192, 129, 186,
		       197, 128, 172,
		       200, 128, 158,
		       204, 127, 142,
		       210, 126, 113,
		       212, 126, 98,
		       213, 126, 84,
		       216, 126, 49,
	      /* 22 */ 220, 133, 0};  /* end: orange */
//===== end "don greenburg" color map ======================================
//==========================================================================



