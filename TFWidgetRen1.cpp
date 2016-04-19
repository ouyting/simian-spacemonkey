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
// TFWidgetRen1.cpp: implementation of the TFWidgetRen class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif

#include "TFWidgetRen1.h"
#include "glUE.h"
#include "gluvv.h"
#include "VectorMath.h"
#include <iostream.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TFWidgetRen::TFWidgetRen()
{
	ballRad = .1;
	barRad  = .05;
	position[0] = -1.3;
	position[1] = -1.3;
	position[2] = -1;

	screenwidth = .9;
	screenheight = .2;
  screenpos[0] = .45;
  screenpos[1] = -.46;

	screenbar = .01;
  screenball = screenbar * 1.5;

	//gluvv.tf.ptexsz[0] = 256;
	//gluvv.tf.ptexsz[1] = 256;
	//gluvv.tf.ptexsz[2] = 1;
	//gluvv.tf.numelts   = 4;

	gluvv.tf.loadme    = 0;

	lastppos[0] = lastppos[1] = lastppos[2] = 0;
	lastSlide1Pos[0] = 1;
	lastSlide1Pos[1] = 0;

	histo = 0;
}

TFWidgetRen::~TFWidgetRen()
{

}

//================================================================= draw
//======================================================================
void TFWidgetRen::draw()
{

	if(gluvv.reblend) return;
  if(!gluvv.mv) return;
	
	glEnable(GL_LIGHTING);

	float dist = position[2] - gluvv.env.eye[2];

  width  = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	          dist/gluvv.env.clip[0]) * screenwidth;
  height = ((gluvv.env.frustum[3]-gluvv.env.frustum[2]) * 
	          dist/gluvv.env.clip[0]) * screenheight;
	
	position[0] = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	               dist/gluvv.env.clip[0]) * screenpos[0];
  position[1] = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	               dist/gluvv.env.clip[0]) * screenpos[1];

	barRad = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
  	        dist/gluvv.env.clip[0]) * screenbar;

  ballRad = ((gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
	           dist/gluvv.env.clip[0]) * screenball;

	tris->setWH(width-ballRad*2, height-ballRad*2);
	brush->setWH(width-ballRad*2, height-ballRad*2);
	tris->setBallBar(ballRad/2.0, barRad/2.0);
  brush->setBallBar(ballRad/2.0, barRad/2.0);

	fill();
	gluvvPushName();
  glPushName(22);
  glPushName(222);
  glPushName(2222);

	glPushMatrix();{
		glTranslatef(position[0], position[1], position[2]);
		glRotatef(180, 0, 1, 0);
		drawFrame();
	} glPopMatrix();

	gluvvPopNames();

	GlErr("TFWidgetRen","draw()");
}

//=========================================================== draw frame
//======================================================================
void  TFWidgetRen::drawFrame()
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

	glColor3f(.8, .5, .1);                       //sliders
	//interactive sample rate
	float ispos[3] = {ballRad + (gluvv.volren.interactSamp/6.0)*(width-2*ballRad), 0,0};
	drawSlider(90,WdgYAxis,ispos,islide);
	glColor3f(.8, .2, .1);
	//good sample rate
	float gspos[3] = {ballRad + (gluvv.volren.goodSamp/6.0)*(width-2*ballRad), 0,0};
	drawSlider(90,WdgYAxis,gspos,gslide);
	glColor3f(.6, .2, .8);
	//alpha gamma slider
	float aspos[3] = {0, ballRad + (gluvv.volren.gamma/10.0)*(height-2*ballRad), 0};
	drawSlider(90,WdgXAxis,aspos,aslide);
	glColor3f(.5, .6, .8);
#if 1
	if(gluvv.dmode == GDM_V2G){ //multiple channel
		float a3spos[3] = {ballRad + (gluvv.tf.slider1hi)*(width-2*ballRad), height, 0};
		drawSlider(90,WdgYAxis,a3spos,a3slidehi);
		glColor3f(.2, .6, .8);
		a3spos[0] = ballRad + (gluvv.tf.slider1lo)*(width-2*ballRad);
		drawSlider(90,WdgYAxis,a3spos,a3slidelo);
		if((lastSlide1Pos[0] != gluvv.tf.slider1hi) || (lastSlide1Pos[1] != gluvv.tf.slider1lo)){
      rasterizevgH(gluvv.volren.deptex2);
			gluvv.volren.loadTLUT = 1;
		}
		lastSlide1Pos[0] = gluvv.tf.slider1hi;
		lastSlide1Pos[1] = gluvv.tf.slider1lo;
	}
#endif
	if((gluvv.dmode == GDM_VGH)||(gluvv.dmode == GDM_V1GH)){ //vgh
		if(lastSlide1Pos[0] != gluvv.tf.slider1hi){
			rasterizevgH(gluvv.volren.deptex2);
			gluvv.volren.loadTLUT = 1;
		}
		lastSlide1Pos[0] = gluvv.tf.slider1hi;
		float a3spos[3] = {ballRad + (gluvv.tf.slider1hi)*(width-2*ballRad), height, 0};
		drawSlider(90,WdgYAxis,a3spos,a3slidehi);
	}


	glDisable(GL_LIGHTING);
	glColor4f(0,0,0,1);                          //transfer function map depth
  glLoadName(tfsheet1);
  glBindTexture(GL_TEXTURE_2D, gluvv.volren.deptexName);
  glBegin(GL_QUADS);{  //draw the transfer function
    glVertex3f(barRad*1.5,    barRad*1.5,     0);
    glVertex3f(width-barRad*1.5, barRad*1.5,     0);
    glVertex3f(width-barRad*1.5, height-barRad*1.5, 0);
    glVertex3f(barRad*1.5,    height-barRad*1.5,0);
  } glEnd();
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);                       //---------------------------

	drawProbe();

	//-------------- load new pixel texture ----------------------------------
	if(gluvv.tf.clearpaint){   //clear the paint texture
		clearPtex(paintex);
		gluvv.tf.clearpaint = 0;
		gluvv.tf.loadme = 1;
	}

	//gluvvShade oldshade;	
	if(gluvv.tf.paintme){      //add to the paint texture
		switch(gluvv.probe.brush){
		case EllipseBrush:
		case AutoEllipseBrush:
		case OneDBrush:
		case AutoOneDBrush:
			//oldshade = gluvv.shade; //faux shading is bad for painting
			//gluvv.shade = gluvvShadeAmb;
			brush->rasterize(paintex);
			//gluvv.shade = oldshade;
			gluvv.tf.paintme = 0;
			gluvv.tf.loadme = 1;
		  break;
		case TriangleBrush:
			LevWidget *lwtmp = new LevWidget(*brush);
			tris->insert(lwtmp);
			gluvv.tf.paintme = 0;
			gluvv.tf.loadme = 1;
		}
	}
	
	if(gluvv.tf.dropme){
		LevWidget *lwtmp = new LevWidget(*brush);
		tris->insert(lwtmp);
		gluvv.tf.dropme = 0;
		gluvv.tf.loadme = 1;
		glutPostRedisplay();
	}
  
	if(gluvv.tf.loadme){      //generate a new transfer function
    //clearPtex();
    clearPtex(gluvv.volren.deptex3);
    copyPtex(gluvv.volren.deptex, paintex);
    tris->rasterize(gluvv.volren.deptex, gluvv.volren.deptex3);
		if(gluvv.tf.brushon)
	    brush->rasterize(gluvv.volren.deptex, gluvv.volren.deptex3);
    loadPtex();
    gluvv.tf.loadme = 0;
		gluvv.volren.loadTLUT = 1;
  } 

	//----------------------------------------------------------------------

	glColor4f(1,1,1,1);                          //transfer function color 
  glLoadName(tfsheet1);
	glActiveTexture(GL_TEXTURE0_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glDisable(GL_LIGHTING);
	glDepthFunc(GL_EQUAL);

	if(!histo){
		loadHist2D();
	}

	if(gluvv.tf.histOn){ //histogram
		glBindTexture(GL_TEXTURE_2D, histName);
		glDisable(GL_BLEND);
		glBegin(GL_QUADS);{  //draw the transfer function
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,0);
			glVertex3f(barRad*1.5,    barRad*1.5,     0);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,0);
			glVertex3f(width-barRad*1.5, barRad*1.5,     0);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,1);
			glVertex3f(width-barRad*1.5, height-barRad*1.5, 0);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,1);
			glVertex3f(barRad*1.5,    height-barRad*1.5,0);
		} glEnd();	
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} else {
		glEnable(GL_BLEND);
	  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}


	                 //transfer function
	glBindTexture(GL_TEXTURE_2D, gluvv.volren.deptexName);
  glBegin(GL_QUADS);{  //draw the transfer function
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,0);
    glVertex3f(barRad*1.5,    barRad*1.5,     0);
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,0);
    glVertex3f(width-barRad*1.5, barRad*1.5,     0);
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,1);
    glVertex3f(width-barRad*1.5, height-barRad*1.5, 0);
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,1);
    glVertex3f(barRad*1.5,    height-barRad*1.5,0);
  } glEnd();

  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);                       //---------------------------
	glDepthFunc(GL_LESS);

	glPushMatrix();{                             //draw levoy triangles
    glTranslatef(ballRad, ballRad, ballRad);
    tris->draw();
		brush->draw();
  } glPopMatrix();

}

//=========================================================== draw probe
//======================================================================
void TFWidgetRen::drawProbe()
{	
	int xi = gluvv.mv->xiSize;   //size of volume (axies)
	int yi = gluvv.mv->yiSize;
	int zi = gluvv.mv->ziSize;
	int px, py, pz;
	float fpos[3];

	if(!gluvv.mprobe){ //probe
		px = gluvv.probe.vpos[0] * xi;  //integer position in volume
		py = gluvv.probe.vpos[1] * yi;  //(least)
		pz = gluvv.probe.vpos[2] * zi;
		fpos[0] = gluvv.probe.vpos[0] * xi;  //floating point position in volume
		fpos[1] = gluvv.probe.vpos[1] * yi;  //
		fpos[2] = gluvv.probe.vpos[2] * zi;
		if((lastppos[0] != gluvv.probe.vpos[0])||
			(lastppos[1] != gluvv.probe.vpos[1])||
			(lastppos[2] != gluvv.probe.vpos[2]))
			if(gluvv.tf.brushon)
				gluvv.tf.loadme = 1;
		lastppos[0] = gluvv.probe.vpos[0];
		lastppos[1] = gluvv.probe.vpos[1];
		lastppos[2] = gluvv.probe.vpos[2];
	} else {           //clip
		px = gluvv.clip.mousepnt[0] * xi;  //integer position in volume
		py = gluvv.clip.mousepnt[1] * yi;  //(least)
		pz = gluvv.clip.mousepnt[2] * zi;
		fpos[0] = gluvv.clip.mousepnt[0] * xi;//floating point position in volume
		fpos[1] = gluvv.clip.mousepnt[1] * yi;  
		fpos[2] = gluvv.clip.mousepnt[2] * zi;
		if((lastppos[0] != gluvv.clip.mousepnt[0])||
			(lastppos[1] != gluvv.clip.mousepnt[1])||
			(lastppos[2] != gluvv.clip.mousepnt[2]))
			if(gluvv.tf.brushon)
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
		
    unsigned char *dp = (unsigned char*)gluvv.mv->volumes[0].currentData;
		//I need to handle bricked volumes !!!!
		if((gluvv.mv->numSubVols != 1) && (gluvv.mv->wholeVol)){
			dp = (unsigned char*)gluvv.mv->wholeVol->currentData;
		}
		//unsigned char *gp = (unsigned char*)gluvv.mv->volumes[0].currentData1;
    float vpnts[8][3];  //voxel corners, tf space
    float vvals[8][3];  //voxel values, [0..1]

		int sx = gluvv.mv->xiSize;
		int sy = gluvv.mv->yiSize;
		int sz = gluvv.mv->ziSize;

		//initialize values
		for(int i=0; i<2; ++i){      //z = i
      for(int j=0; j<2; ++j){    //y = j
				for(int k=0; k<2; ++k){  //x = k
					vpnts[i*4 + j*2 + k][0] = 0;
					vpnts[i*4 + j*2 + k][1] = 0;
					vvals[i*4 + j*2 + k][0] = 0;
					vvals[i*4 + j*2 + k][1] = 0;
				}
			}
		}
		int ne = gluvv.mv->nelts;
		float hacktmp;
		for(i=0; i<2; ++i){      //z = i
      for(int j=0; j<2; ++j){    //y = j
				for(int k=0; k<2; ++k){  //x = k
					switch(gluvv.dmode){
					case GDM_V1:
						vpnts[i*4 + j*2 + k][0] = ballRad +                //get 1st value
							dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne)]/
								255.0 * (width - 2*ballRad);
						vvals[i*4 + j*2 + k][0] =                          //draw in center
							dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne)]/255.0;
						vpnts[i*4 + j*2 + k][1] = ballRad + .5 * (height - 2*ballRad);;
						break;
					case GDM_V1G:
					case GDM_V2:
					case GDM_V2G:
						vpnts[i*4 + j*2 + k][0] = ballRad +                //get 1st value
							dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne)]/
								255.0 * (width - 2*ballRad);
						vvals[i*4 + j*2 + k][0] =
							dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne)]/255.0;
						vpnts[i*4 + j*2 + k][1] = ballRad +                //get 2nd value
							dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne) + 1]/
								255.0 * (height - 2*ballRad);
						vvals[i*4 + j*2 + k][1] =                          
							dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne) + 1]/255.0;
						break;
					case GDM_V2GH:
					case GDM_V3:
					case GDM_V3G: //sigh what am I going to do about 4 value volumes
					case GDM_V4: 
						vpnts[i*4 + j*2 + k][0] = ballRad +                //get 1st value
							dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne)]/
								255.0 * (width - 2*ballRad);
						vvals[i*4 + j*2 + k][0] =
							dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne)]/255.0;
						vpnts[i*4 + j*2 + k][1] = ballRad +                //get 2nd value
							dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne) + 1]/
								255.0 * (height - 2*ballRad);
						vvals[i*4 + j*2 + k][1] =
							dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne) + 1]/255.0;
						vpnts[i*4 + j*2 + k][2] = ballRad +                //get 3rd value
							dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne) + 2]/
								255.0 * (width - 2*ballRad);
						vvals[i*4 + j*2 + k][2] =
							dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne) + 2]/255.0;
					case GDM_VGH:
					case GDM_V1GH:
					case GDM_VGH_VG:
					case GDM_VGH_V:
						vpnts[i*4 + j*2 + k][0] = ballRad +                //get 1st value
							dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne)]/
								255.0 * (width - 2*ballRad);
						vvals[i*4 + j*2 + k][0] =
							dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne)]/255.0;
						vpnts[i*4 + j*2 + k][1] = ballRad +                //get 2nd value
							dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne) + 1]/
								255.0 * (height - 2*ballRad);
						vvals[i*4 + j*2 + k][1] =
							dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne) + 1]/255.0;
						                                                   //get hessian
						hacktmp = dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne) + 2]/85.0-1;
						hacktmp = hacktmp > 0 ? sqrt(hacktmp) : -sqrt(-hacktmp);
						vpnts[i*4 + j*2 + k][2] = (hacktmp+1)/2.0;
						vvals[i*4 + j*2 + k][2] = 
							dp[((pz+i)*sx*sy*ne) + ((py+j)*sx*ne) + ((px+k)*ne) + 2]/169.0;
						break;

					}
				}
			}
		}
		//done getting voxel corner values

		if(!gluvv.picking){
      glDisable(GL_LIGHTING);
      //glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      //glDisable(GL_DEPTH_TEST);
      //glDepthFunc(GL_EQUAL);
			glColor4f(1,1,1,.3);
      
      glBegin(GL_LINE_STRIP);{
				glVertex3f(vpnts[0][0], vpnts[0][1], .01);
				glVertex3f(vpnts[1][0], vpnts[1][1], .01);
				glVertex3f(vpnts[3][0], vpnts[3][1], .01);
				glVertex3f(vpnts[2][0], vpnts[2][1], .01);
				glVertex3f(vpnts[0][0], vpnts[0][1], .01);
				glVertex3f(vpnts[4][0], vpnts[4][1], .01);
				glVertex3f(vpnts[5][0], vpnts[5][1], .01);
				glVertex3f(vpnts[7][0], vpnts[7][1], .01);
				glVertex3f(vpnts[6][0], vpnts[6][1], .01);
				glVertex3f(vpnts[4][0], vpnts[4][1], .01);
      } glEnd();
      
      glBegin(GL_LINES);{
				glVertex3f(vpnts[1][0], vpnts[1][1], .01);
				glVertex3f(vpnts[5][0], vpnts[5][1], .01);
				
				glVertex3f(vpnts[3][0], vpnts[3][1], .01);
				glVertex3f(vpnts[7][0], vpnts[7][1], .01);
				
				glVertex3f(vpnts[2][0], vpnts[2][1], .01);
				glVertex3f(vpnts[6][0], vpnts[6][1], .01);
      } glEnd();
      
      glEnable(GL_DEPTH_TEST);
      glDisable(GL_BLEND);
    } //if not picking

		float val[3];
    triLerpV3(val, vvals, fpos);             //trilinear interp for values

    //------------ Paint Brush ---------------------------------------
		if(gluvv.tf.brushon){ //wow clean this up!!!
			if(gluvv.probe.brush == EllipseBrush){
				float bsz = (1.0 - gluvv.probe.slider)/4.0;
				float plpos[2] = {val[0]-bsz, val[1]+bsz};//paint positions
				float prpos[2] = {val[0]+bsz, val[1]+bsz};
				float pbpos[2] = {val[0]+bsz, val[1]-bsz};
				brush->setPos(pbpos, plpos, prpos, val[0], val[1]);
				brush->drawOn = 0;
				brush->squareType();
				glPushMatrix();{                             //draw brush
				glTranslatef(ballRad, ballRad, ballRad);
				brush->draw();
			} glPopMatrix();
			} else if((gluvv.probe.brush == TriangleBrush)||(gluvv.probe.brush == AutoEllipseBrush)){
				float maxx = -1000;
				float maxy = -1000;
				float minx = 1000;
				float miny = 1000;
				for(int i=0; i<8; ++i){
					maxx = MAX(maxx, vvals[i][0]);
					maxy = MAX(maxy, vvals[i][1]);
					minx = MIN(minx, vvals[i][0]);
					miny = MIN(miny, vvals[i][1]);
				}
				float bsz = (1.0 - gluvv.probe.slider)*2;
				float w = MAX((maxx - minx)/2.0, .01);
				float h = MAX((maxy - miny)/2.0, .01);
				float plpos[2] = {val[0]-w*bsz, val[1]+h*bsz};
				float prpos[2] = {val[0]+w*bsz, val[1]+h*bsz};
				float pbpos[2] = {val[0], 0};
				if(gluvv.probe.brush == TriangleBrush){
					brush->setPos(pbpos, plpos, prpos, 0, val[1] - h*bsz);
					brush->drawOn = 1;
					brush->triangleType();
				}
				else {
					pbpos[0] = val[0]+w*bsz;
					pbpos[1] = val[1]-h*bsz;
					brush->setPos(pbpos, plpos, prpos, val[0], val[1]);
					brush->drawOn = 0;
					brush->squareType();
					glPushMatrix();{                             //draw brush
						glTranslatef(ballRad, ballRad, ballRad);
						brush->draw();
					} glPopMatrix();
				}
			}
			if((gluvv.dmode == GDM_V1) || (gluvv.dmode == GDM_VGH_V) || 
				(gluvv.probe.brush == OneDBrush)||(gluvv.probe.brush == AutoOneDBrush)){
				float maxx = -1000;
				float minx = 1000;
				for(int i=0; i<8; ++i){
					maxx = MAX(maxx, vvals[i][0]);
					minx = MIN(minx, vvals[i][0]);
				}
				float bsz = (1.0 - gluvv.probe.slider)*2;
				float w = MAX((maxx - minx)/2.0, .01);
				float plpos[2] = {val[0]-w*bsz, 1};
				float prpos[2] = {val[0]+w*bsz, 1};
				float pbpos[2] = {val[0], 0};
				if(gluvv.probe.brush == OneDBrush){
					plpos[0] = val[0] + (1.0 - gluvv.probe.slider)/4.0;
					prpos[0] = val[0] - (1.0 - gluvv.probe.slider)/4.0;
				}
				brush->setPos(pbpos, plpos, prpos, val[0], val[1]);
				brush->oneDType();
			}

		}
		//------------ end Paint Brush ------------------------------------

		if(!gluvv.picking){
      //position of point in tf space:
      glPushMatrix();{
				glTranslatef(ballRad + val[0]*(width - 2*ballRad),
					ballRad + val[1]*(height - 2*ballRad),
					.01);
				glColor4f(1, 0, 0, 1);
				glLoadName(dpoint);
				glutSolidSphere(ballRad/2.5, 20, 20);
      } glPopMatrix();
      //draw voxel points in data domain
      for(i=0; i<8; ++i){
				glPushMatrix();{
					glTranslatef(vpnts[i][0], vpnts[i][1], .01);
					float bcol[3] = {1,1,1};
					if(gluvv.dmode == GDM_VGH){
						bcol[0]= don[((int)(vpnts[i][2]*22) + 1)*3 + 0]/255.0;
						bcol[1]= don[((int)(vpnts[i][2]*22) + 1)*3 + 1]/255.0;
						bcol[2]= don[((int)(vpnts[i][2]*22) + 1)*3 + 2]/255.0;
					}
					glColor4f(bcol[0], bcol[1], bcol[2], 1);
					glutSolidSphere(ballRad/3.5, 20, 20);
				} glPopMatrix();
      }
    }
    glEnable(GL_LIGHTING);
		

}

//=========================================================== trilerp v3
//======================================================================
void TFWidgetRen::triLerpV3(float val[3], float voxels[8][3], float pos[3])
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

//================================================================= init
//======================================================================
void TFWidgetRen::init()
{
	widgetInit();

	//-------- Levoy triangles -----------
  tris = new LevWidget(&gluvv);
  tris->setWH(width-ballRad*2, height-ballRad*2);
  float b[2] = {.5,0}; float l[2]={.3,.7}; float r[2] = {.7,.7};
  tris->setPos(b,l,r);
  tris->setBallBar(ballRad/2.0, barRad/2.0);
  tris->setDepth(position[2]);
  tris->squareType();
  tris->on = 0;  //turn root triangle off

	shadTris = new LevWidget(&gluvv);
  shadTris->setWH(width-ballRad*2, height-ballRad*2);
  shadTris->setPos(b,l,r);
  shadTris->setBallBar(ballRad/2.0, barRad/2.0);
  shadTris->setDepth(position[2]);
  shadTris->squareType();
  shadTris->on = 0;  //turn root triangle off

  brush= new LevWidget(&gluvv);
  brush->setWH(width-ballRad*2, height-ballRad*2);
  brush->setPos(b,l,r);
  brush->setBallBar(ballRad/2.0, barRad/2.0);
  brush->setDepth(position[2]);
  brush->setAlpha(.7);
  brush->squareType();
  brush->on = 1;     //turn paint brush on
  brush->drawOn = 0; //but dont actualy draw the widget

	paintex = new unsigned char[gluvv.tf.ptexsz[0]*gluvv.tf.ptexsz[1]*gluvv.tf.ptexsz[2]*gluvv.tf.numelts];
	clearPtex(paintex);

}

//============================================================ load Hist
//======================================================================
void  TFWidgetRen::loadHist2D()
{
	histo = new unsigned char[256*256];
	cerr << "Generating histogram";
	if(!gluvv.mv->hist2D(histo)){
		delete[] histo;
		histo = 0;
		return;
	}
	cerr << endl;

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &histName);   //the dep tex that we use for the tf
	glBindTexture(GL_TEXTURE_2D, histName);
		
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_LUMINANCE,
			256,
			256,
			0,
			GL_LUMINANCE,
			GL_UNSIGNED_BYTE,
			histo);
	glFlush();

	glDisable(GL_TEXTURE_2D);


}


//================================================================= pick
//======================================================================
int  TFWidgetRen::pick(int data1, int data2, int data3,
					             float x, float y, float z)
{
	//cerr << "hi you picked the tf widget" << endl;
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

//================================================================= key
//======================================================================
int  TFWidgetRen::key(unsigned char key, int x, int y)
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
	 switch(key){
	 case 'B':
		 if(brush->on){
			 if(brush->drawOn) brush->drawOn = 0;
			 else brush->drawOn = 1;
			 return 1;
		 }
		 break;
	 }
	return 0;
}

//================================================================ mouse
//======================================================================
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
					float pnt[2] = {1-(mousepnt[0]+position[0])/width, 
						(mousepnt[1]-position[1])/height};
					float b[2] = {pnt[0], 0};
					float l[2] = {pnt[0]-.1, pnt[1]};
					float r[2] = {pnt[0]+.1, pnt[1]};
					lwtmp->setPos(b,l,r, l[1]*.5);
					lwtmp->setDepth(position[2]);
					tris->insert(lwtmp);
					gluvv.tf.loadme = 1;
					break;
    }
    
  }
  else{ //state is _UP
    gluvv.volren.sampleRate = gluvv.volren.goodSamp;
    glutPostRedisplay();
    glutSetWindowTitle("Transfer Function Widget");
  }

  return 1;

	lastwinpos[0] = x;
	lastwinpos[1] = y;
	return 0;
}

//================================================================= move
//======================================================================
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
  subV3(d, position, gluvv.env.eye);
  //float dist = normV3(d);
  float dist = position[2] - gluvv.env.eye[2];
  float dx = sdx *
    (gluvv.env.frustum[1]-gluvv.env.frustum[0]) * dist/gluvv.env.clip[0];
  float dy = sdy *
    (gluvv.env.frustum[3]-gluvv.env.frustum[2]) * dist/gluvv.env.clip[0];
	
	switch(gluvv.mouse.state){////////////////////////////////////
  case GLUT_DOWN:
    switch(gluvv.mouse.button){///////////////////////
    case GLUT_LEFT_BUTTON:
      switch(pickedObj){///////////////////
			case gslide:
				gluvv.volren.goodSamp += sdx * 6;
				char str[255];
				sprintf(str, "High Quality Sample Rate = %f", gluvv.volren.goodSamp);
				glutSetWindowTitle(str);
				glutPostRedisplay();
				break;
			case islide:
				gluvv.volren.interactSamp += sdx * 6;
				str[255];
				sprintf(str, "Interactive Sample Rate = %f", gluvv.volren.interactSamp);
				glutSetWindowTitle(str);
				glutPostRedisplay();
				break;
			case aslide:
				gluvv.volren.gamma -= sdy/height * 10;
				gluvv.volren.gamma = gluvv.volren.gamma < 0 ? 0 : gluvv.volren.gamma;
				str[255];
				sprintf(str, "Opacity Gamma = %f", gluvv.volren.gamma);
				glutSetWindowTitle(str);
				gluvv.volren.loadTLUT = 1;
				glutPostRedisplay();
				break;
			case a3slidehi:
				gluvv.tf.slider1hi += sdx;
				gluvv.tf.slider1hi = gluvv.tf.slider1hi < 0 ? 0 : (gluvv.tf.slider1hi > 1 ? 1 : gluvv.tf.slider1hi);
				if(gluvv.tf.slider1hi < gluvv.tf.slider1lo) gluvv.tf.slider1lo = gluvv.tf.slider1hi;
				str[255];
				sprintf(str, "3rd Axis Threshold HI = %f", gluvv.tf.slider1hi);
				glutSetWindowTitle(str);
				//gluvv.volren.loadTLUT = 1;
				glutPostRedisplay();
				break;
			case a3slidelo:
				gluvv.tf.slider1lo += sdx;
				gluvv.tf.slider1lo = gluvv.tf.slider1lo < 0 ? 0 : (gluvv.tf.slider1lo > 1 ? 1 : gluvv.tf.slider1lo);
				if(gluvv.tf.slider1hi < gluvv.tf.slider1lo) gluvv.tf.slider1hi = gluvv.tf.slider1lo;
				str[255];
				sprintf(str, "3rd Axis Threshold LO = %f", gluvv.tf.slider1lo);
				glutSetWindowTitle(str);
				//gluvv.volren.loadTLUT = 1;
				glutPostRedisplay();
				break;
			case bbar:
      case tbar:
      case lbar:
      case rbar:
				screenpos[0]  -= sdx;
				screenpos[1]  -= sdy;
				gluvv.volren.sampleRate = gluvv.volren.interactSamp;
				glutSetWindowTitle("Transfer Function Widget: MOVE");
				glutPostRedisplay();
				break;
      case blball:
				screenpos[0]  -= sdx;
				screenpos[1]  -= sdy;
				screenwidth   -= sdx;
				screenheight  += sdy;
				gluvv.volren.sampleRate = gluvv.volren.interactSamp;
				glutSetWindowTitle("Transfer Function Widget: RESIZE");
				glutPostRedisplay();
				break;
      case brball:
				screenpos[1]  -= sdy;
				screenwidth   += sdx;
				screenheight  += sdy;
				gluvv.volren.sampleRate = gluvv.volren.interactSamp;
				glutSetWindowTitle("Transfer Function Widget: RESIZE");
				glutPostRedisplay();
				break;
      case tlball:
				screenpos[0]  -= sdx;
				screenwidth   -= sdx;
				screenheight  -= sdy;
				gluvv.volren.sampleRate = gluvv.volren.interactSamp;
				glutSetWindowTitle("Transfer Function Widget: RESIZE");
				glutPostRedisplay();
				break;
      case trball:
				screenwidth   += sdx;
				screenheight  -= sdy;
				gluvv.volren.sampleRate = gluvv.volren.interactSamp;
				glutSetWindowTitle("Transfer Function Widget: RESIZE");
				glutPostRedisplay();
				break;
#if 0
      case beslider:
				if(pickedTri){
					LevWidget *tt;
					if(tt = tris->get(pickedTri)){
						float slider = tt->getBE();
						slider += dx/width;
						tt->setBE(slider);
						glutSetWindowTitle("Transfer Function Widget: Boundary Emphasis");
						gluvv.volren.sampleRate = gluvv.volren.interactSamp;
						glutPostRedisplay();
						gluvv.tf.loadme = 1;
					}
				}
				break;
#endif		
      }///////////////////////////////////
      break;
			case GLUT_RIGHT_BUTTON:
				switch(pickedObj){///////////////////
				case bbar:
				case tbar:
				case lbar:
				case rbar:
					position[0]  -= dx;
					position[2]  -= dy;
					gluvv.volren.sampleRate = gluvv.volren.interactSamp;
					glutSetWindowTitle("Transfer Function Widget: MOVE");
					glutPostRedisplay();
					break;
				}
				break;
				case GLUT_MIDDLE_BUTTON:
					
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

//============================================================== release
//======================================================================
int  TFWidgetRen::release()
{

	return 0;
}

//============================================================ clearPtex
//======================================================================
void TFWidgetRen::clearPtex()
{
	int sz = (gluvv.tf.numelts*
	    gluvv.tf.ptexsz[0]*
	    gluvv.tf.ptexsz[1]*
	    gluvv.tf.ptexsz[2]);

  for(int i=0; i<sz; ++i){
    gluvv.volren.deptex[i] = 0;
  }
}


void TFWidgetRen::clearPtex(unsigned char *ptex)
{
	if(!ptex) return;
	int sz = (gluvv.tf.numelts*
						gluvv.tf.ptexsz[0]*
				    gluvv.tf.ptexsz[1]*
						gluvv.tf.ptexsz[2]);
	for(int i=0; i<sz; ++i){
    ptex[i] = 0;
  }
}

//============================================================ copyPtex
//======================================================================
void TFWidgetRen::copyPtex(unsigned char *ptexout, unsigned char *ptexin)
{
	if((!ptexout) || (!ptexin)) return;
	int sz = (gluvv.tf.numelts*
						gluvv.tf.ptexsz[0]*
				    gluvv.tf.ptexsz[1]*
						gluvv.tf.ptexsz[2]);
	for(int i=0; i<sz; ++i){
    ptexout[i] = ptexin[i];
  }
}

//======================================================== rasterize vgH
//======================================================================
void TFWidgetRen::rasterizevgH(unsigned char *ptex)
{
	if(!ptex) return;
	//vgh style
	if((gluvv.dmode == GDM_VGH) || (gluvv.dmode == GDM_V1GH)){
		int cent = (int)(gluvv.tf.ptexsz[0]/3.0);
		int sti = gluvv.tf.ptexsz[0]*4;
		int stj = 4;
		
		float b = 255-20*cent*(1-gluvv.tf.slider1hi);
		float m = ABS(255-b)/(float)cent;
		
		for(int i=0; i<gluvv.tf.ptexsz[1]; ++i){
			for(int j=0; j<cent; ++j){
				ptex[i*sti + j*stj + 3] = CLAMP_ARB(0,j*m + b,255); 
			}
		}
		
		b = 255;
		m = -m;
		for(i=0; i<gluvv.tf.ptexsz[1]; ++i){
			//I added 2 extra vertical lines here to handle interpolation artifacts
			for(int j=1; j<cent+1; ++j){ 
				ptex[i*sti + (j+cent)*stj + 3] = CLAMP_ARB(0,j*m + b,255); 
			}
		}
	}
	if((gluvv.dmode == GDM_V2G) || (gluvv.dmode == GDM_V2GH)){
		int sti = gluvv.tf.ptexsz[0]*4;
		int stj = 4;
		//up to the bottom slider
		for(int i=0; i<gluvv.tf.ptexsz[1]; ++i){  //all the way to the top
			for(int j=0; j<(int)(gluvv.tf.slider1lo*255); ++j){ 
				ptex[i*sti + j*stj + 3] = 0;
			}
			//bottom slider to top slider
			for(j=(int)(gluvv.tf.slider1lo*255); j<(int)(gluvv.tf.slider1hi*255); ++j){ 
				ptex[i*sti + j*stj + 3] = (j-(int)(gluvv.tf.slider1lo*255))/((gluvv.tf.slider1hi-gluvv.tf.slider1lo)*255)*255;
			}
			//top slider to end
			for(j=(int)(gluvv.tf.slider1hi*255); j<gluvv.tf.ptexsz[0]; ++j){ 
				ptex[i*sti + j*stj + 3] = 255;
			}
		}
	}
}

//============================================================= loadPtex
//======================================================================
// this is just the visible texture, the volume renderer will load the actual tf
void TFWidgetRen::loadPtex()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gluvv.volren.deptexName);
		
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

	glDisable(GL_TEXTURE_2D);
}

//======================================================== mouse 2 plane
//======================================================================
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
  subV3(ep, gluvv.env.eye, position);
  float me[3];  //eye - clipplane point
  subV3(me, gluvv.env.eye, cpnt);
  float dir[3] = {0,0,-1};
  float t = dotV3(dir, ep)/dotV3(dir, me);
  negateV3(me);
  scaleV3(t,me);
  addV3(mousepnt, me, gluvv.env.eye);
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