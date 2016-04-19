//                   ________    ____   ___ 
//                  |        \  /    | /  /
//                  +---+     \/     |/  /
//                  +--+|  |\    /|     < 
//                  |  ||  | \  / |  |\  \ 
//                  |      |  \/  |  | \  \ 
//                   \_____|      |__|  \__\
//                       Copyright  2001 
//                      Joe Michael Kniss
//                    << jmk@cs.utah.edu >>
//               "All Your Base are Belong to Us"
//-------------------------------------------------------------------------
// R8kVolRen3D.cpp: implementation of the R8kVolRen3D class.
//   -Advanced scattering renderer
//////////////////////////////////////////////////////////////////////


#ifdef WIN32
#include <windows.h>
#endif

#include "R8kVolRen3D.h"
#include <iostream.h>
#include <stdlib.h>
#include "VectorMath.h"
#include "glUE.h"
#include "VolRenAux.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

R8kVolRen3D::R8kVolRen3D()
{
  pbuff = 0;
  pbuff2 = 0;
	go = 0;
	fsNames = 0;
	pbfsNames = 0;
	pb2fsNames = 0;
	pb3fsNames = 0;
	currentTStep = gluvv.volren.timestep;
}

R8kVolRen3D::~R8kVolRen3D()
{

}

//================================================================= INIT
//======================================================================

void R8kVolRen3D::init()
{
	srand(328973892);

	cerr << "initing" << endl;
	if(gluvv.mv)
	{
		go = 1;
		createVolumes(1);
		create2DDepTex();
		createShadowTex();
		createFragShades(fsNames);
		createFragClip();
		createCubeTex();
		glFinish();

		//---------------- create 2 pbuffer for forward scattering -------------

    //-------- second pbuffer
		if(!(pbuff2 = new PBuffer(gluvv.light.buffsz[0], gluvv.light.buffsz[1], 
			                       GLUT_SINGLE | GLUT_RGBA)))
		{
			cerr << "ERROR: PBuffer2 intialization failed!" << endl;
		} 
		else 
		{
			pbuff2->Initialize(true, true); //create a shared pbuffer
			pbuff2->MakeCurrent(); {   
				glClearColor( 0.0, 0.0, 0.0, 0.0 );			
				glMatrixMode( GL_PROJECTION );
				glOrtho(0,1,0,1,-1,3);
				glMatrixMode(GL_MODELVIEW);
				gluLookAt(0,0,2,
					        0,0,0,
									0,1,0);
			  createFragShadow(pb2fsNames);
			} MakeCurrent();
			cerr << "PBuffer2 intialization succeded" << endl;
		}
		//----------------end create 2 pbuffer for forward scattering -------------
    
    //----------- first pbuffer
    if(!(pbuff = new PBuffer(gluvv.light.buffsz[0], gluvv.light.buffsz[1], 
			                       GLUT_SINGLE | GLUT_RGBA)))
		{
			cerr << "ERROR: PBuffer intialization failed!" << endl;
		} 
		else 
		{
			pbuff->Initialize(true, true); //create a shared pbuffer
			pbuff->MakeCurrent(); {   
				glClearColor( 0.0, 0.0, 0.0, 0.0 );			
				glMatrixMode( GL_PROJECTION );
				glOrtho(0,1,0,1,-1,3);
				glMatrixMode(GL_MODELVIEW);
				gluLookAt(0,0,2,
					        0,0,0,
									0,1,0);
			  createFragShadow(pbfsNames);
			} MakeCurrent();
			cerr << "PBuffer intialization succeded" << endl;
		}



		//----------------create a pbuffer for backward scattering-----------------
		if(!(sliceBuff = new PBuffer(gluvv.light.buffsz[0], gluvv.light.buffsz[1], 
			                       GLUT_SINGLE | GLUT_RGBA)))
		{
			cerr << "ERROR: slice PBuffer intialization failed!" << endl;
		} 
		else 
		{
			sliceBuff->Initialize(true, true); //create a shared pbuffer
			sliceBuff->MakeCurrent(); {   
				glClearColor( 0.0, 0.0, 0.0, 0.0 );			
				glMatrixMode( GL_PROJECTION );
				glOrtho(0,1,0,1,-1,3);
				glMatrixMode(GL_MODELVIEW);
				gluLookAt(0,0,2,
					        0,0,0,
									0,1,0);
			  createFragShadow(pb3fsNames);
			} MakeCurrent();
			cerr << "slice PBuffer intialization succeded" << endl;
		}
	}//end if gluvv.mv

}

//================================================================= DRAW
//======================================================================

void R8kVolRen3D::draw()
{
	if(!go) return;

	renderVolume();
	//renderSlice();

}

//======================================================== Render Volume
//======================================================================
//========== NOTES ===========================================
// Standard brick vertex ordering:
//  
//     (011)        (111)
//       6 +---------+ 7   Where 1's are the size of the brick
//        /|        /|      allong that axis
//       / |       / |
// (001)/  |(101) /  |
//   4 +---------+ 5 |
//     |   |     |   |(110) z axis
//     | 2 +-----+---+ 3    ^   
//     |  /(010) |  /       |   y axis
//     | /       | /        | /
//     |/        |/         |/
//   0 +---------+ 1        +-------> x axis 
//  (000)      (100)
//

void R8kVolRen3D::renderVolume()
{
	if(gluvv.reblend) return;

	if(gluvv.picking)
	{
		return;
	}

	if(gluvv.volren.timestep != currentTStep)
	{
		cerr << "Loading new volume : " << gluvv.volren.timestep << endl;
		currentTStep = gluvv.volren.timestep;
		createVolumes();
	}

	//-------------- load new shading cube map -----------------------------
	if(gluvv.light.load||gluvv.light.gload)
	{
		loadCubeTex(gluvv.light.gload);
		gluvv.light.load = 0;
		gluvv.light.gload = 0;
	}
	//-------------- end load new shading cube map -------------------------

	//-------------- Re-Scale Alpha values ---------------------------------
	if(gluvv.volren.scaleAlphas)
	{
		if((lastSamp != gluvv.volren.sampleRate)||(gluvv.volren.loadTLUT))
		{             //see if the sample rate changed
			if((lastGoodSamp != gluvv.volren.goodSamp) || gluvv.volren.loadTLUT)
			{               //good sample rate changed
				copyScale(gDeptex, gluvv.volren.goodSamp * 1/gluvv.volren.gamma, deptex);
				lastGoodSamp = gluvv.volren.goodSamp;
			}
			if((lastInteSamp != gluvv.volren.interactSamp) || gluvv.volren.loadTLUT)
			{                //interact samp rate changed
				copyScale(iDeptex, gluvv.volren.interactSamp * 1/gluvv.volren.gamma, deptex);
				lastInteSamp = gluvv.volren.interactSamp;
			}
			
			//seems as though we should just have both loaded
			if(gluvv.volren.sampleRate == gluvv.volren.goodSamp)
			{                //which one do we load (good)
				loadDepTex(gDeptex);
				lastSamp = gluvv.volren.goodSamp;
			}
			else if(gluvv.volren.sampleRate == gluvv.volren.interactSamp)
			{                //(interactive)
				loadDepTex(iDeptex);
				lastSamp = gluvv.volren.interactSamp;
			}

			//we MUST reload if the transfer function changed
			if(gluvv.volren.loadTLUT)
			{                //now load the transfer function
				loadDepTex(gluvv.volren.deptex2, deptex2Name);
			}
			gluvv.volren.loadTLUT = 0;
		}
	} 
	else 
	{  //just do gamma scale, don't update for the sample rate (for testing purposes)
		if(gluvv.volren.loadTLUT)
		{
			copyScale(gDeptex, 1/gluvv.volren.gamma, deptex);
			loadDepTex(gDeptex);
			loadDepTex(gluvv.volren.deptex2, deptex2Name);
			gluvv.volren.loadTLUT = 0;
		}
	}
	//-------------- end Re-Scale Alpha values ------------------------------

	//-------------- Recompute normals --------------------------------------
  if(gluvv.volren.usePostCNorm)
  {
    if(gluvv.volren.loadNorms)
    {
      unsigned char *gradout = new unsigned char[
        gluvv.mv->xiSize*gluvv.mv->yiSize*gluvv.mv->ziSize *3];
      VRA_PostCNorms(gradout);
     
      if(gluvv.mv->numSubVols == 1)
      {
        loadTex3B(gradout, shadeNames[0],
          gluvv.mv->xiSize, gluvv.mv->yiSize, gluvv.mv->ziSize);
      }
      else
      {
        cerr << "Bricked recompute not implemented yet" << endl;
      }
      gluvv.volren.loadNorms = 0;
    }
  }

  //-------------- end Recompute normals ----------------------------------

  
  //-------------- do dot product with clip and view dir
	float vdir[3];
	subV3(vdir, gluvv.env.eye, gluvv.clip.pos);
	normalizeV3(vdir);
	normalizeV3(gluvv.clip.dir);
	float dv = dotV3(vdir,gluvv.clip.dir);
	float globalModV[16]; //save original tranform
	glGetFloatv(GL_MODELVIEW_MATRIX, globalModV); //save the world model view	
	//-------------- end do dot product with clip and view dir
	
	glPushMatrix();
	{                //move to the volume location
		glTranslatef(gluvv.rinfo.trans[0], //translate
					 			 gluvv.rinfo.trans[1], 
								 gluvv.rinfo.trans[2]);
		glMultMatrixf(gluvv.rinfo.xform);  //rotate
		glTranslatef(-gluvv.mv->xfSize/2,  //center
								 -gluvv.mv->yfSize/2,
								 -gluvv.mv->zfSize/2);

		//-- first compute the axis for rendering
		float axis[4]  = {0,0,1,-1}; //axis[0-2] for view direction and [3] for the dot
			 
		// modify slice axis for shadowing -------------------------------------
		if(gluvv.light.shadow)
		{
			float ldir[3];                //figure out the light direction
			copyV3(ldir,gluvv.light.pos); //this is the position
			negateV3(ldir);               //this is the direction
			normalizeV3(ldir);            //normalize both vectors
			normalizeV3(axis);            //    light & axis
			float vdl = dotV3(ldir,axis);  //the dot product tells us to flip the axis
			
			if(vdl<=0)
			{             //need to negate the axis
				//cerr << "back to front" << endl;
				negateV3(axis);
			} 
			else 
			{
				//cerr << "front to back" << endl;
			}
			//now compute  (v - l)/2 + l = halfway view and light
			float spn[3];            //slice plane normal 
			subV3(spn, axis, ldir);
			scaleV3(.5,spn);
			float halfv[3];
			addV3(halfv, spn, ldir);
			copyV3(axis, halfv);
			axis[3] = vdl; //this stores the value of the dot product
			//its a left handed coordiante system duh
			axis[1] = -axis[1];//why? I have no idea, there must be something wrong with my math
		  negateV3(axis);  //the actual direction for slicing
		}
		// end modify slice axis for shadowing --------------------------------

		GLdouble mv[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, mv);  //save modelview matrix

		// --------------------- set up shading transform ---------------------
		float *r = gluvv.rinfo.xform;
		float cons0[4] = {r[0]/2+.5, r[4]/2+.5, r[8]/2+.5,  gluvv.clip.alpha};
		float cons1[4] = {r[1]/2+.5, r[5]/2+.5, r[9]/2+.5,  0};
		float cons2[4] = {r[2]/2+.5, r[6]/2+.5, r[10]/2+.5, 0};
		glSetFragmentShaderConstantATI(GL_CON_0_ATI, cons0);
		glSetFragmentShaderConstantATI(GL_CON_1_ATI, cons1);		
		glSetFragmentShaderConstantATI(GL_CON_2_ATI, cons2);		
		// --------------------- end set up shading transform -----------------

		// --------------------- set up attinuation parameters ----------------
		GLfloat tmpmv[16];
		getModelView(tmpmv);
		GLfloat invmv[16];
		inverseMatrix(invmv, tmpmv);
		translateV3W(volview, invmv, gluvv.env.eye);
		translateV3W(vollt, invmv, gluvv.light.pos);
		ltrange[0] = normV3(gluvv.light.pos) - gluvv.light.lattLimits[0];
		ltrange[1] = normV3(gluvv.light.pos) - gluvv.light.lattLimits[1];
		fogrange[0] = normV3(gluvv.env.eye) - gluvv.light.fogLimits[0];
		fogrange[1] = normV3(gluvv.env.eye) - gluvv.light.fogLimits[1];
		float cons3[4] = {gluvv.light.fogColor[0],
			                gluvv.light.fogColor[1],
											gluvv.light.fogColor[2],
											gluvv.light.amb};
		glSetFragmentShaderConstantATI(GL_CON_3_ATI, cons3);		
		// --------------------- end set up attinuation parameters ------------

		//-------------- draw clip slices --------------------------------------
		if(axis[3] <= 0)
		{                //only do this if front to back
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);
			for(int i=0; i<gluvv.mv->numSubVols; ++i)
			{
				drawClip(i, dv, globalModV);			
			}
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}
		//-------------- end draw clip slices ----------------------------------

		//---------------------- set up shading environment -------------------
		setupTexUs();
		setupFragShade(axis[3]);
		if(gluvv.light.shadow)
			setupPBuff();
		glDepthMask(GL_FALSE); //no depth wright, only depth test
		//---------------------- end set up shading environment ---------------
#if 1
		gluvv.reblend = GB_UNDER; //force gluvv.display to composite the background
#endif
		//----------------------------------------------------------------------
		//-------------- render the volume -------------------------------------
		renderBricks(mv, axis); //RENDER!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//-------------- end render the volume ---------------------------------
		//----------------------------------------------------------------------
		
		//----------------------- clean up shading environment ----------------
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		if(gluvv.light.shadow)
			resetPBuff();
		resetTexUs();
		resetFragShade();
		//----------------------- end clean up shading environment -------------

		//-------------- draw clip slices --------------------------------------
		if(axis[3] <= 0)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);
			for(int i=0; i<gluvv.mv->numSubVols; ++i)
			{
				drawClip(i, -dv, globalModV);			
			}
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		} 
		else 
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);
			for(int i=0; i<gluvv.mv->numSubVols; ++i)
			{
				drawClip(i, -dv, globalModV);			
			}
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}
		//-------------- end draw clip slices ----------------------------------

	} glPopMatrix();

	resetFragShade();
	resetTexUs();
	resetClips();

	//------------- render last thing copied from pbuffer --------------------
	if(gluvv.light.showView)
	{	
		glPushMatrix();
		{
			glTranslatef(gluvv.rinfo.trans[0], gluvv.rinfo.trans[1], gluvv.rinfo.trans[2]);
		
			//first the forward light buffer
			glActiveTexture(GL_TEXTURE1_ARB); 
			{
				glDisable(GL_TEXTURE_3D);
				glEnable(GL_TEXTURE_2D);
				wglBindTexImageARB(pbuff->buffer, WGL_FRONT_LEFT_ARB);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			
			glBegin(GL_QUADS);
			{
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,0);
				glVertex3f(-1, -1, 0);
				
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,1);
				glVertex3f(-1, 0, 0);
				
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1,1);
				glVertex3f(0, 0, 0);
				
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1,0);
				glVertex3f(0, -1, 0);
			} glEnd();
			
			glActiveTexture(GL_TEXTURE1_ARB);
			{
				wglReleaseTexImageARB(pbuff->buffer, WGL_FRONT_LEFT_ARB);
				glDisable(GL_TEXTURE_2D);
			}

			//the other forward buffer
			glActiveTexture(GL_TEXTURE1_ARB); 
			{
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);
				glDisable(GL_TEXTURE_3D);
        glDisable(GL_TEXTURE_2D);
				glEnable(GL_TEXTURE_2D);
				wglBindTexImageARB(pbuff2->buffer, WGL_FRONT_LEFT_ARB);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			
			glBegin(GL_QUADS);
			{
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1,0);
				glVertex3f(1, -1, 0);
				
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1,1);
				glVertex3f(1, 0, 0);
				
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,1);
				glVertex3f(0, 0, 0);
				
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,0);
				glVertex3f(0, -1, 0);
			} glEnd();
			
			glActiveTexture(GL_TEXTURE1_ARB);
			{
				wglReleaseTexImageARB(pbuff2->buffer, WGL_FRONT_LEFT_ARB);
				glDisable(GL_TEXTURE_2D);
			}
#if 0
			//next the backward light buffer
			glActiveTexture(GL_TEXTURE1_ARB); 
			{
				glDisable(GL_TEXTURE_3D);
				glEnable(GL_TEXTURE_2D);
				wglBindTexImageARB(sliceBuff->buffer, WGL_FRONT_LEFT_ARB);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}

			glBegin(GL_QUADS);
			{
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,0);
				glVertex3f(1, 1, 0);
				
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,1);
				glVertex3f(1, 0, 0);
				
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1,1);
				glVertex3f(0, 0, 0);
				
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1,0);
				glVertex3f(0, 1, 0);
			} glEnd();

		glActiveTexture(GL_TEXTURE1_ARB);{
			wglReleaseTexImageARB(sliceBuff->buffer, WGL_FRONT_LEFT_ARB);
			glDisable(GL_TEXTURE_2D);
		}
#endif

		} glPopMatrix();
		

		glActiveTexture(GL_TEXTURE0_ARB);
	}
	//------------- end render last thing copied from pbuffer ----------------

	glEnable(GL_LIGHTING);
	GlErr("r8kvolren::draw", "end");
}

//======================================================= Get Model View
//======================================================================
void R8kVolRen3D::getModelView(float mv[16])
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
  m4[12] = -.5 * gluvv.mv->xfSize;
  m4[13] = -.5 * gluvv.mv->yfSize;
  m4[14] = -.5 * gluvv.mv->zfSize;
  matrixMult(mv, m3, m4);
}


//======================================================== Render Bricks
//======================================================================

void R8kVolRen3D::renderBricks(GLdouble mv[16], GLfloat axis[4])
{
	
	if(gluvv.mv->numSubVols > 1)
	{            //render multiple bricked volumes
		int *order = new int[gluvv.mv->numSubVols];   //the correct draw order
		float *zval = new float[gluvv.mv->numSubVols];//zvalue of each subvolume
		float ref[3];
		copyV3(ref, axis);
		negateV3(ref);
		scaleV3(100, ref);

		//--------------- compute zvalues ---------------------- 
		for(int i=0; i< gluvv.mv->numSubVols; ++i) //for each sub volume
		{ 
			Volume *v = &gluvv.mv->volumes[i];
			order[i] = i;
			float c[3] = {v->xfPos + v->xfSize/2.0, v->yfPos + v->yfSize/2.0, v->zfPos + v->zfSize/2.0};
			float center[3];
			translateV3W(center, mv, c);
			float sub[3];
			subV3(sub, ref, center);
			//zval[i] = normV3(sub);//(float)(center[0]*center[0] + center[1]*center[1] + center[2]*center[2]);
			zval[i] = (float)(center[0]*center[0] + center[1]*center[1] + center[2]*center[2]);
		}
		//--------------- end compute zvalues ------------------- 

		//--------------- sort Z values -------------------------
		for(i=0; i< gluvv.mv->numSubVols-1; ++i)
		{                //now sort
			for(int j=i+1; j<gluvv.mv->numSubVols; ++j)
			{
				if(axis[3] <=0)
				{            //back to front rendering sort
					if(zval[order[i]] < zval[order[j]])
					{     //test for swap
						int tmp = order[i];
						order[i] = order[j];
						order[j] = tmp;
					}
				} 
				else 
				{            //front to back rendering sort
					if(zval[order[i]] > zval[order[j]])
					{         //test for swap
						int tmp = order[i];
						order[i] = order[j];
						order[j] = tmp;
					}
				}
			}
		}
		//--------------- end sort Z values ----------------------

		//--------------- render bricks --------------------------
		for(i=0 ; i< gluvv.mv->numSubVols; ++i)
		{               //finaly render each sub volume
			cerr << "rendering brick " << order[i] << endl;
			Volume *v = &gluvv.mv->volumes[order[i]];

			float sxf = v->xfSize;
			float syf = v->yfSize;
			float szf = v->zfSize;
			float hpx = 0;//1.0/(gluvv.mv->xiSize*2.0);
			float hpy = 0;//1.0/(gluvv.mv->yiSize*2.0);
			float hpz = 0;//1.0/(gluvv.mv->ziSize*2.0);

			float vo[8][3] = {{0,0,0},{sxf,0,0},{0,syf,0},{sxf,syf,0},{0,0,szf},{sxf,0,szf},{0,syf,szf},{sxf,syf,szf}};
			float tx[8][3] = {{0,0,0},{1,0,0},{0,1,0},{1,1,0},
												{0,0,1},{1,0,1},{0,1,1},{1,1,1}};
			glPushMatrix(); 
			{
				setupClips(order[i],vo,tx); 

				glTranslatef(v->xfPos, v->yfPos, v->zfPos);
	  
				render3DVA(gluvv.volren.sampleRate,mv,order[i],vo,tx,axis);
				
			} glPopMatrix();
			//glFinish();
		}
		//--------------- end render bricks ----------------------

	} 
	else 
	{          //just render one volume!
		float sxf = gluvv.mv->xfSize;
		float syf = gluvv.mv->yfSize;
		float szf = gluvv.mv->zfSize;
		float vo[8][3] = {{0,0,0},{sxf,0,0},{0,syf,0},{sxf,syf,0},{0,0,szf},{sxf,0,szf},{0,syf,szf},{sxf,syf,szf}};
		float tx[8][3] = {{0,0,0},{1  ,0,0},{0,1  ,0},{1  ,1  ,0},{0,0,1  },{1  ,0,1  },{0,1  ,1  },{1  ,1  ,1  }};
		
		setupClips(0,vo,tx);

		render3DVA(gluvv.volren.sampleRate,mv,0,vo,tx,axis);
		
	}
}

//========================================================== Setup Clips
//======================================================================
void R8kVolRen3D::setupClips(int vol, float vo[8][3], float tx[8][3])
{
	Volume *v = &gluvv.mv->volumes[vol];

	if(gluvv.clip.on && gluvv.clip.ortho)
	{
				float cp[3];
				cp[0] = gluvv.clip.vpos[0] > v->xfPos ? 
					(gluvv.clip.vpos[0] < v->xfSize + v->xfPos ? gluvv.clip.vpos[0] - v->xfPos : v->xfSize) : 0;
				cp[1] = gluvv.clip.vpos[1] > v->yfPos ? 
					(gluvv.clip.vpos[1] < v->yfSize + v->yfPos ? gluvv.clip.vpos[1] - v->yfPos : v->yfSize) : 0;
				cp[2] = gluvv.clip.vpos[2] > v->zfPos ? 
					(gluvv.clip.vpos[2] < v->zfSize + v->zfPos ? gluvv.clip.vpos[2] - v->zfPos : v->zfSize) : 0;
				switch(gluvv.clip.oaxis)
				{
				case VolRenAxisXPos:
					tx[1][0] = cp[0]/v->xfSize;
					tx[3][0] = cp[0]/v->xfSize;
					tx[5][0] = cp[0]/v->xfSize;
					tx[7][0] = cp[0]/v->xfSize;
					vo[1][0] = cp[0];
					vo[3][0] = cp[0];
					vo[5][0] = cp[0];
					vo[7][0] = cp[0];
					break;
				case VolRenAxisXNeg:
					tx[0][0] = cp[0]/v->xfSize;
					tx[2][0] = cp[0]/v->xfSize;
					tx[4][0] = cp[0]/v->xfSize;
					tx[6][0] = cp[0]/v->xfSize;
					vo[0][0] = cp[0];
					vo[2][0] = cp[0];
					vo[4][0] = cp[0];
					vo[6][0] = cp[0];
					break;
        case VolRenAxisYPos:
					tx[2][1] = cp[1]/v->yfSize;
					tx[3][1] = cp[1]/v->yfSize;
					tx[6][1] = cp[1]/v->yfSize;
					tx[7][1] = cp[1]/v->yfSize;
					vo[2][1] = cp[1];
					vo[3][1] = cp[1];
					vo[6][1] = cp[1];
					vo[7][1] = cp[1];
					break;
        case VolRenAxisYNeg:
					tx[0][1] = cp[1]/v->yfSize;
					tx[1][1] = cp[1]/v->yfSize;
					tx[4][1] = cp[1]/v->yfSize;
					tx[5][1] = cp[1]/v->yfSize;
					vo[0][1] = cp[1];
					vo[1][1] = cp[1];
					vo[4][1] = cp[1];
					vo[5][1] = cp[1];
					break;
        case VolRenAxisZPos:
					tx[4][2] = cp[2]/v->zfSize;
					tx[5][2] = cp[2]/v->zfSize;
					tx[6][2] = cp[2]/v->zfSize;
					tx[7][2] = cp[2]/v->zfSize;
					vo[4][2] = cp[2];
					vo[5][2] = cp[2];
					vo[6][2] = cp[2];
					vo[7][2] = cp[2];
					break;
        case VolRenAxisZNeg:
					tx[0][2] = cp[2]/v->zfSize;
					tx[1][2] = cp[2]/v->zfSize;
					tx[2][2] = cp[2]/v->zfSize;
					tx[3][2] = cp[2]/v->zfSize;
					vo[0][2] = cp[2];
					vo[1][2] = cp[2];
					vo[2][2] = cp[2];
					vo[3][2] = cp[2];
					break;
				}
	} 

}

//============================================================ drawClips
//======================================================================
void R8kVolRen3D::drawClip(int vol, float dv, float wmv[16])
{
	if((!gluvv.clip.ortho))
	{
		switch(gluvv.dmode)
		{
		case GDM_V1:
		case GDM_V1G:
		case GDM_V2:
		case GDM_VGH_VG:
		case GDM_VGH_V:
			cerr << "clipping" << endl;
		  glActiveTexture(GL_TEXTURE2_ARB); 
			{         //3rd & 4th axis, or clipping
				glEnable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_3D);
			}

			glPushMatrix();
			{                   //this sets the clipping plane
				glLoadIdentity();                //clips are set in world space
				glMultMatrixf(wmv);              //origional world space coords
				glTranslatef(gluvv.clip.pos[0],  //location of clipping plane
					           gluvv.clip.pos[1],
					           gluvv.clip.pos[2]);
				glMultMatrixf(gluvv.clip.xform); //rotation of clip plane
				GLdouble zup[4] = {0, 0, -1, 0}; // always in z direction
				glEnable(GL_CLIP_PLANE5);        //enable the gl clip plane
				glClipPlane(GL_CLIP_PLANE5, zup);
      } glPopMatrix();
			break;
		}
	}
	
	
	if((!gluvv.clip.on)||(!gluvv.clip.ortho)) return;
	
	Volume *v = &gluvv.mv->volumes[vol];
	float c[4][3];

	copyV3(c[0],gluvv.clip.corners[0]);
	copyV3(c[1],gluvv.clip.corners[1]);
	copyV3(c[2],gluvv.clip.corners[2]);
	copyV3(c[3],gluvv.clip.corners[3]);
	//this moves the clip plane to sub-volume space
	c[0][0] = CLAMP_ARB(0,c[0][0]-v->xfPos,v->xfSize);
	c[1][0] = CLAMP_ARB(0,c[1][0]-v->xfPos,v->xfSize);
	c[2][0] = CLAMP_ARB(0,c[2][0]-v->xfPos,v->xfSize);
	c[3][0] = CLAMP_ARB(0,c[3][0]-v->xfPos,v->xfSize);
	c[0][1] = CLAMP_ARB(0,c[0][1]-v->yfPos,v->yfSize);
	c[1][1] = CLAMP_ARB(0,c[1][1]-v->yfPos,v->yfSize);
	c[2][1] = CLAMP_ARB(0,c[2][1]-v->yfPos,v->yfSize);
	c[3][1] = CLAMP_ARB(0,c[3][1]-v->yfPos,v->yfSize);
	c[0][2] = CLAMP_ARB(0,c[0][2]-v->zfPos,v->zfSize);
	c[1][2] = CLAMP_ARB(0,c[1][2]-v->zfPos,v->zfSize);
	c[2][2] = CLAMP_ARB(0,c[2][2]-v->zfPos,v->zfSize);
	c[3][2] = CLAMP_ARB(0,c[3][2]-v->zfPos,v->zfSize);


	float offset = .001; //this is used to avoid z-compete when the slice is on top

	glPushMatrix();
	{         //make sure we draw this in sub-volume space
		glTranslatef(v->xfPos, v->yfPos, v->zfPos);
		switch(gluvv.clip.oaxis)
		{
		case VolRenAxisXPos:
			if((dv < 0)&&(c[0][0] > 0)&&(c[0][0]<v->xfSize))
			{
				float ov[3] = {offset, 0, 0}; //offset vector
				for(int i=0; i<4; ++ i)
					addV3(c[i],c[i],ov);
				renderSlice(c,vol);
			}
			break;
		case VolRenAxisXNeg:
			if(dv > 0)
			{
				float ov[3] = {-offset, 0, 0}; //offset vector
				for(int i=0; i<4; ++ i)
					addV3(c[i],c[i],ov);
				renderSlice(c,vol);
			}
			break;
		case VolRenAxisYPos:
			if((dv > 0)&&(c[0][1] > 0)&&(c[0][1]<v->yfSize))
			{
				float ov[3] = {0, offset, 0}; //offset vector
				for(int i=0; i<4; ++ i)
					addV3(c[i],c[i],ov);
				renderSlice(c,vol);
			}
			break;
		case VolRenAxisYNeg:
			if((dv < 0)&&(c[0][1] > 0)&&(c[0][1]<v->yfSize))
			{
				float ov[3] = {0, -offset, 0}; //offset vector
				for(int i=0; i<4; ++ i)
					addV3(c[i],c[i],ov);
				renderSlice(c,vol);
			}
			break;
		case VolRenAxisZPos:
			if((dv < 0)&&(c[0][2] > 0)&&(c[0][2]<v->zfSize))
			{
				float ov[3] = {0, 0, offset}; //offset vector
				for(int i=0; i<4; ++ i)
					addV3(c[i],c[i],ov);
				renderSlice(c,vol);
			}
			break;
		case VolRenAxisZNeg:
			if((dv > 0)&&(c[0][2] > 0)&&(c[0][2]<v->zfSize))
			{
				float ov[3] = {0, 0, -offset}; //offset vector
				for(int i=0; i<4; ++ i)
					addV3(c[i],c[i],ov);
				renderSlice(c,vol);			
			}
			break;
		}
	} glPopMatrix();

	
}

//============================================== Render Slice w/ corners
//======================================================================
void R8kVolRen3D::renderSlice(float c[4][3], int vol)
{    //this just renders a single slice from the volume
	glActiveTexture(GL_TEXTURE0_ARB); 
	{          
		glEnable(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, texNames[vol]);
	}
	
	Volume *v = &gluvv.mv->volumes[vol];

	glEnable(GL_FRAGMENT_SHADER_ATI);
	glBindFragmentShaderATI(cpfsName);

	glBegin(GL_QUADS);
	{
		glMultiTexCoord3fARB(GL_TEXTURE0_ARB,c[0][0]/(v->xfSize),c[0][1]/(v->yfSize),c[0][2]/(v->zfSize));
		glVertex3f(c[0][0], c[0][1], c[0][2]);
		glMultiTexCoord3fARB(GL_TEXTURE0_ARB,c[1][0]/(v->xfSize),c[1][1]/(v->yfSize),c[1][2]/(v->zfSize));
		glVertex3f(c[1][0], c[1][1], c[1][2]);
		glMultiTexCoord3fARB(GL_TEXTURE0_ARB,c[2][0]/(v->xfSize),c[2][1]/(v->yfSize),c[2][2]/(v->zfSize));
		glVertex3f(c[2][0], c[2][1], c[2][2]);
		glMultiTexCoord3fARB(GL_TEXTURE0_ARB,c[3][0]/(v->xfSize),c[3][1]/(v->yfSize),c[3][2]/(v->zfSize));
		glVertex3f(c[3][0], c[3][1], c[3][2]);			
	} glEnd();

	glDisable(GL_FRAGMENT_SHADER_ATI);

	glActiveTexture(GL_TEXTURE0_ARB); 
	{          
		glDisable(GL_TEXTURE_3D);
	}
}

//=========================================================== resetClips
//======================================================================
void R8kVolRen3D::resetClips()
{
	glDisable(GL_CLIP_PLANE0);
}

//======================================================== Setup Shaders
//======================================================================
void R8kVolRen3D::setupTexUs()
{
		GlErr("set up texture units", "before texture0");

		//texture 0 = data
		glActiveTexture(GL_TEXTURE0_ARB); 
		{          //this is for the slice
			glEnable(GL_TEXTURE_3D);
		}
		GlErr("set up texture units", "after texture0");

		//texture 1 = VG tf
		glActiveTexture(GL_TEXTURE1_ARB);
		{          //this is for the transfer function
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, deptexName);
		}
		GlErr("set up texture units", "after texture1");
		
		//texture 2 = shade volume
		glActiveTexture(GL_TEXTURE2_ARB); 
		{          // for shading or clipping (not both)
				glEnable(GL_TEXTURE_3D);
		}
		GlErr("set up texture units", "after texture2");

		//texture 3 = H tf
		glActiveTexture(GL_TEXTURE3_ARB);  
		{            //3rd & 4th axis, or clipping, or shadows
				switch(gluvv.dmode){       //hey! we use this for 3rd & 4th axies
				case GDM_VGH:
				case GDM_V1GH:
				case GDM_V2G:
				case GDM_V2GH:
				case GDM_V3:
				case GDM_V3G:
				case GDM_V4:
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, deptex2Name);
					break;
				case GDM_V1:
				case GDM_V1G:
				case GDM_V2:
				case GDM_VGH_VG:
				case GDM_VGH_V:
					if(!gluvv.clip.ortho) break; //we want to take care of this elsewhere
				default:
					glDisable(GL_TEXTURE_2D);
					glDisable(GL_TEXTURE_3D);
				}
		}
		GlErr("set up texture units", "after texture3");
#if 0
		//texture 4 = cube map dependant texture
		glActiveTexture(GL_TEXTURE4_ARB);
		{ //cube map
			glEnable(GL_TEXTURE_CUBE_MAP_ARB);
			glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, gluvv.light.cubeName);
		}
		GlErr("set up texture units", "after texture4");
#endif

#if 0
		//texture 5 = shadow texture
		glActiveTexture(GL_TEXTURE5_ARB);
		{ //shadow
			glEnable(GL_TEXTURE_2D);
			//glBindTexture(GL_TEXTURE_2D, pbuffName);
		}
#endif
		GlErr("set up texture units", "after texture4");

}

void R8kVolRen3D::resetTexUs()
{
	//NOTE: you must completely un-bind any texture which will be copied to later
		glActiveTexture(GL_TEXTURE5_ARB); 
		{ //not used
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_3D);
				//glBindTexture(GL_TEXTURE_1D, 0);
		}
		glActiveTexture(GL_TEXTURE4_ARB); 
		{ //not used
				//glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, 0);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_3D);
		}
		glActiveTexture(GL_TEXTURE3_ARB); 
		{ //not used
				//glBindTexture(GL_TEXTURE_2D, 0);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_3D);
		}
		glActiveTexture(GL_TEXTURE2_ARB); 
		{ //not used
				//glBindTexture(GL_TEXTURE_2D, 0);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_3D);
		}
		glActiveTexture(GL_TEXTURE1_ARB); 
		{ //not used
				//glBindTexture(GL_TEXTURE_2D, 0);  //this is because of the p-buffer
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_3D);
		}
		glActiveTexture(GL_TEXTURE0_ARB); 
		{ //not used
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_3D);
				//glBindTexture(GL_TEXTURE_1D, 0);
		}
}

//===================================================== Setup Frag Shade
//======================================================================

void R8kVolRen3D::setupFragShade(float ftb)
{
	glEnable(GL_FRAGMENT_SHADER_ATI);
	if(ftb <= 0)
	{ //back to front
		//shaded
		if((gluvv.shade == gluvvShadeDiff) || (gluvv.shade == gluvvShadeDSpec))
		{
			if(gluvv.light.shadow) //shaded & shadowed
				glBindFragmentShaderATI(fsNames[6]);
			else //shaded, no shadow
				glBindFragmentShaderATI(fsNames[4]);
		}
		//shadowed
		else if(!gluvv.light.shadow) //just emissive & absorbtion
			glBindFragmentShaderATI(fsNames[0]);
		else // shadow, no shade
			glBindFragmentShaderATI(fsNames[2]);
	}
	else 
	{ //front to back
		//shaded
		if((gluvv.shade == gluvvShadeDiff) || (gluvv.shade == gluvvShadeDSpec)){			
			if(gluvv.light.shadow) //shaded & shadowed
				glBindFragmentShaderATI(fsNames[7]);
			else //shaded, no shadow
				glBindFragmentShaderATI(fsNames[5]);
		}
		//shadowed
		else if(!gluvv.light.shadow)
			glBindFragmentShaderATI(fsNames[1]);
		else //no shade, no shadow
			glBindFragmentShaderATI(fsNames[3]);
	}

	if(gluvv.light.sill)
			glBindFragmentShaderATI(fsNames[9]);

}


void R8kVolRen3D::resetFragShade()
{
	glBindFragmentShaderATI(0);
	glDisable(GL_FRAGMENT_SHADER_ATI);
}

//====================================================== Set Up P-Buffer
//======================================================================
//======================================================================
void R8kVolRen3D::setupPBuff()       //prepare pbuffer for rendering
{
	if(gluvv.picking) return;
	if(!gluvv.light.shadow) return;

	if(gluvv.volren.sampleRate == gluvv.volren.goodSamp)
	{
		xscale = gluvv.light.gShadowQual;
		yscale = gluvv.light.gShadowQual;
	}
	else 
	{
		xscale = gluvv.light.iShadowQual;	
		yscale = gluvv.light.iShadowQual;	
	}

	currentPbuff = 1;

	pbuff2->MakeCurrent();
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);

		glActiveTexture(GL_TEXTURE1_ARB);
		{          //this is for the transfer function
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, deptexName);
		}

		glActiveTexture(GL_TEXTURE2_ARB);  
		{            //3rd & 4th axis, or clipping, or shadows
				switch(gluvv.dmode){       //hey! we use this for 3rd & 4th axies
				case GDM_VGH:
				case GDM_V1GH:
				case GDM_V2G:
				case GDM_V2GH:
				case GDM_V3:
				case GDM_V3G:
				case GDM_V4:
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, deptex2Name);
					break;
				case GDM_V1:
				case GDM_V1G:
				case GDM_V2:
				case GDM_VGH_VG:
				case GDM_VGH_V:
					if(!gluvv.clip.ortho) break; //we want to take care of this elsewhere
				default:
					glDisable(GL_TEXTURE_2D);
					glDisable(GL_TEXTURE_3D);
				}
		}


		glEnable(GL_FRAGMENT_SHADER_ATI);
		glBindFragmentShaderATI(pb2fsNames[1]);
		glDisable(GL_FRAGMENT_SHADER_ATI);

	} MakeCurrent();

	pbuff->MakeCurrent();
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);

		glActiveTexture(GL_TEXTURE1_ARB);
		{          //this is for the transfer function
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, deptexName);
		}

		glActiveTexture(GL_TEXTURE2_ARB);  
		{            //3rd & 4th axis, or clipping, or shadows
				switch(gluvv.dmode){       //hey! we use this for 3rd & 4th axies
				case GDM_VGH:
				case GDM_V1GH:
				case GDM_V2G:
				case GDM_V2GH:
				case GDM_V3:
				case GDM_V3G:
				case GDM_V4:
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, deptex2Name);
					break;
				case GDM_V1:
				case GDM_V1G:
				case GDM_V2:
				case GDM_VGH_VG:
				case GDM_VGH_V:
					if(!gluvv.clip.ortho) break; //we want to take care of this elsewhere
				default:
					glDisable(GL_TEXTURE_2D);
					glDisable(GL_TEXTURE_3D);
				}
		}


		glEnable(GL_FRAGMENT_SHADER_ATI);
		glBindFragmentShaderATI(pbfsNames[1]);
		glDisable(GL_FRAGMENT_SHADER_ATI);

	} MakeCurrent();



	sliceBuff->MakeCurrent();
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	} MakeCurrent();

	GlErr("volren", "end end of setup pbuff");
}

void R8kVolRen3D::resetPBuff()              //reset pbuffer
{
	cerr << "reseting the pbuffer" << endl;
	pbuff->MakeCurrent();
	{
		resetTexUs();
		glDisable(GL_FRAGMENT_SHADER_ATI);

	} MakeCurrent();
  pbuff2->MakeCurrent();
	{
		resetTexUs();
		glDisable(GL_FRAGMENT_SHADER_ATI);

	} MakeCurrent();

	cerr << "done" << endl;
}


//=================================================== Render 3D Vec Algn
//======================================================================
//======================================================================
//======================================================================
//======================================================================

void R8kVolRen3D::render3DVA(float sampleFrequency,  //how many samples per voxel
		GLdouble mv[16],  //the model view matrix
		unsigned int v,   //volume number (brick)
		float vo[8][3],	  //volume vertex coords model-space coords
		float tx[8][3],	  //texture vertex coords tex-space coords
		float axis[4])	  //axis to slice along world-space coords
{

   float rv[8][3];     //the rotated volume (may include a scale)
	 //float lv[8][4];
   float maxval = -100; //(tmp)
   float minval = 100;
   int minvert = 0;
   GLdouble mvinv[16];

   inverseMatrix(mvinv, mv); //invert model view matrix

	 float start[3];       //pick a reference point for rendering
	 copyV3(start, axis);  // this is some distance out the rendering axis
	 negateV3(start);
	 scaleV3(100, start);

	 //build the light transform ------------------------ ltxf
	 float tm0[16];
	 float tb[16] = {1,0,0,0,         //translate back by a half
		               0,1,0,0,
									 0,0,1,0,
									 gluvv.mv->volumes[v].xfPos-gluvv.mv->xfSize/2, 
									 gluvv.mv->volumes[v].yfPos-gluvv.mv->yfSize/2, 
									 gluvv.mv->volumes[v].zfPos-gluvv.mv->zfSize/2, 1};
	 matrixMult(tm0,gluvv.rinfo.xform,tb);
	 float ltxf[16];
	 matrixMult(ltxf,gluvv.light.xf,tm0);
	 //end build the light transform ------------------------

   int vnear = 0;
	 int vfar  = 0;
	 
	 //translate model to world coords (view space) ---------
	 //also find the near and far slice points
   for(int i=0; i<8; ++i)
	 {   
		 translateV3(rv[i], mv, vo[i]);
		 float tv[3];
		 subV3(tv, start, rv[i]);       //compute vertex distance from reference
		 float tvd = normV3(tv);

		 if(maxval < tvd)
		 {
			 maxval = tvd;
			 vfar = i;   //end point for slicing
		 }
		 if(minval > tvd)
		 {
			 minval = tvd;
			 vnear = i;  //starting point for slicing
		 }
   }
	 //end translate model to world coords (view space) ------

	 //find the slice plane normal (sn) ----------------------
	 float vpn[3];
	 vpn[0] = axis[0]; vpn[1] = axis[1]; vpn[2] = axis[2];
   float sn[3];		    //slice plane normal
   translateV3(sn, mvinv, vpn); //move vpn to sn (model space);
	 normalizeV3(sn);
	 //end find the slice plane normal (sn) ------------------

   //note: sn & sp are defined in Model Space, ie the space where the volume
   // is alligned with the (x,y,z) axies

	 //distance between samples
   float dis = gluvv.mv->xfSize / (gluvv.mv->xiSize * sampleFrequency); 
	
	 cscaleV3(start, 100, sn);  //pick a point far from the volume
	 negateV3(start);

	 //find the nearest slicing point (the closest sample)
	 float np[3];// = {vo[minvert][0], vo[minvert][1], vo[minvert][2]}; 
	 float vms[3];   //v minus start (this is the vector from the point to the near vertex)
	 subV3(vms, vo[vnear], start);  
	 float vmsps[3];  //v minus start project on sn
	 cscaleV3(vmsps, dotV3(vms, sn), sn);
	 int sts = (int)(normV3(vmsps)/dis) + 1; //samples to start
	 cscaleV3(np,sts*dis,sn); //the nearest slicing point
	 addV3(np,np,start);
	 //end find the nearest slicing point (the closest sample)

	 //find the farthest slicing point (same process as near point)
	 float fp[3];
	 subV3(vms, vo[vfar], start);
	 cscaleV3(vmsps, dotV3(vms, sn), sn);
	 sts = (int)(normV3(vmsps)/dis);
	 cscaleV3(fp,sts*dis,sn);
	 addV3(fp,fp,start);
	 //end find the farthest slicing point (same process as near point)

	 float distv[3];            //distance to sample
	 subV3(distv, np, fp);      //needs work (fix so that off angle still has the correct #of slices)
	 float dist = normV3(distv);
	 float del[3];// = {sn[0]*dis, sn[1]*dis, sn[2]*dis};
	 subV3(distv, vo[vfar], vo[vnear]);
	 //normalizeV3(distv);   //now figure out how to move the slice point

   int samples = (int)((dist) / dis);//(total distance to be sampled)/(sample spacing)
	 cscaleV3(del, 1/(float)samples, distv);


	 //set up opengl ---------------------------------------------------

	 //set up pbuffer (per sub volume stuff)
	 if(gluvv.light.shadow)
	 {
		 if(!gluvv.picking)
		 {
			 pbuff->MakeCurrent();
			 {
					//glEnable(GL_BLEND);
					//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); 
#if 0
					glActiveTexture(GL_TEXTURE0_ARB);
					{
						glEnable(GL_TEXTURE_3D);
						glBindTexture(GL_TEXTURE_3D, texNames[v]);
					}
#endif
#if 0
					glActiveTexture(GL_TEXTURE2_ARB);
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_3D, shadeNames[v]);
					}
#endif
			 } MakeCurrent();

			 pbuff2->MakeCurrent();
			 {
					//glEnable(GL_BLEND);
					//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
#if 0
					glActiveTexture(GL_TEXTURE0_ARB);
					{
						glEnable(GL_TEXTURE_3D);
						glBindTexture(GL_TEXTURE_3D, texNames[v]);
					}
#endif
#if 0
					glActiveTexture(GL_TEXTURE2_ARB);
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_3D, shadeNames[v]);
					}
#endif
			 } MakeCurrent();

			 GlErr("volren", "setting data volume for pbuffers");
		 }
	 }

	 //now set up this rendering context
	 glDisable(GL_LIGHTING); //light makes it look bad!
   glDisable(GL_CULL_FACE);
   glPolygonMode(GL_FRONT, GL_FILL);
   glPolygonMode(GL_BACK, GL_FILL);
	 
	 glActiveTexture(GL_TEXTURE0_ARB);
	 {
			glEnable(GL_TEXTURE_3D);
			glBindTexture(GL_TEXTURE_3D, texNames[v]);
	 }

	 glActiveTexture(GL_TEXTURE2_ARB);
	 {
		 glEnable(GL_TEXTURE_2D);
	   glBindTexture(GL_TEXTURE_3D, shadeNames[v]);
	 }

	 glEnable(GL_BLEND);

	 if(axis[3] <= 0)
	 { //back to front
		 //glBlendFunc(GL_ONE, GL_ONE);//Pure Emmisive
		 glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); 
	 } 
	 else 
	 {          //front to back
		 glEnable(GL_BLEND);
		 //glBlendFunc(GL_ONE, GL_ONE);//Pure Emmisive
		 glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
		 glEnable(GL_BLEND);
		 glEnable(GL_DEPTH_TEST);
		 glDepthMask(GL_FALSE);
	 }

   glColor4f(1, 1, 1, 1);
   GlErr("R8kVolRen3D","drawVA");
	 //end set up opengl -----------------------------------------------


   float poly[6][3];   // for edge intersections
   float tcoord[6][3]; // for texture intersections
   float tpoly[6][3];  // for transformed edge intersections
   int edges;	       // total number of edge intersections

	 float sp[3]; //starting point for slicing (aka slice point)
	 copyV3(sp,np);

   //highly un-optimized!!!!!!!!!
	 // SLICE ----------------------------------------------------------
   for(i = 0 ; i < samples; ++i)
	 { //for each slice
      //increment the slice plane point by the slice distance
      sp[0] += del[0];
      sp[1] += del[1];
      sp[2] += del[2];

      edges = 0; 
      //now check each edge of the volume for intersection with.. 
      //the plane defined by sp & sn
      //front bottom edge
      edges += intersect(vo[0], vo[1], tx[0], tx[1], rv[0], rv[1], sp, sn, 
			 poly[edges], tcoord[edges], tpoly[edges]);
      //front left edge
      edges += intersect(vo[0], vo[2], tx[0], tx[2], rv[0], rv[2], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //front right edge
      edges += intersect(vo[1], vo[3], tx[1], tx[3], rv[1], rv[3], sp, sn, 
			 poly[edges], tcoord[edges], tpoly[edges]);
      //left bottom edge
      edges += intersect(vo[4], vo[0], tx[4], tx[0], rv[4], rv[0], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //right bottom edge
      edges += intersect(vo[1], vo[5], tx[1], tx[5], rv[1], rv[5], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //front top edge
      edges += intersect(vo[2], vo[3], tx[2], tx[3], rv[2], rv[3], sp, sn, 
			 poly[edges], tcoord[edges], tpoly[edges]);
      //back bottom edge
      edges += intersect(vo[4], vo[5], tx[4], tx[5], rv[4], rv[5], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //back left edge
      edges += intersect(vo[4], vo[6], tx[4], tx[6], rv[4], rv[6], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //back right edge
      edges += intersect(vo[5], vo[7], tx[5], tx[7], rv[5], rv[7], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //back top edge
      edges += intersect(vo[6], vo[7], tx[6], tx[7], rv[6], rv[7], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //left top edge
      edges += intersect(vo[2], vo[6], tx[2], tx[6], rv[2], rv[6], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //right top edge
      edges += intersect(vo[3], vo[7], tx[3], tx[7], rv[3], rv[7], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);

#if 0
      if(edges == 3)
			{ //no sort required for triangles  //no speed up really
				//cerr << "doing a triangle" <<endl;
				glBegin(GL_TRIANGLES);
				{
					glTexCoord3fv(tcoord[0]);
					glVertex3fv(poly[0]);
					glTexCoord3fv(tcoord[1]);
					glVertex3fv(poly[1]);
					glTexCoord3fv(tcoord[2]);
					glVertex3fv(poly[2]);
				}
				glEnd();
      }
      else 
			{ //compute convex hull and sort, a little piece of magic from:
#endif
				// B.M.E. Moret & H.D. Shapiro "P to NP" pp. 453
				
				float dx, dy, tt ,theta, cen[2];  //tt= TempTheta
				cen[0] = cen[1] = 0.0;
				int next;
				//rather than swap 3 arrays, only one?
				int order[6] ={0,1,2,3,4,5};
				
				// order[6] could be an extreemly inefficient way to do this
				for( int j=0; j<edges; ++j)
				{ //find the center of the points
					cen[0] += tpoly[j][0];
					cen[1] += tpoly[j][1];
				} //by averaging	
				cen[0]/= edges;
				cen[1]/= edges;	 
				
				for(j=0; j<edges; ++j)
				{ //for each vertex
					theta = -10;	       //find one with largest angle from center.. 
					next = j;
					for ( int k= j; k<edges; ++k)
					{ 
						//... and check angle made between other edges
						dx = tpoly[order[k]][0] - cen[0];
						dy = tpoly[order[k]][1] - cen[1];
						if( (dx == 0) && (dy == 0))
						{ //same as center? 
							next = k;
							cout << "what teh " << endl;
							break; //out of this for-loop
						}
						tt = dy/(ABS(dx) + ABS(dy)); //else compute theta [0-4]
						if( dx < 0.0 ) tt = (float)(2.0 - tt); //check quadrants 2&3
						else if( dy < 0.0 ) tt = (float)(4.0 + tt); //quadrant 4
						if( theta <= tt ){  //grab the max theta
							next = k;
							theta = tt;
						}
					} //end for(k) angle checking
					// i am using 'tt' as a temp
					// swap polygon vertex ( is this better than another branch?)
					// I am not sure wich is worse: swapping 3 vectors for every edge
					// or: using an array to index into another array??? hmmm....
					//   should have payed more attention in class
					int tmp = order[j];
					order[j] = order[next];
					order[next] = tmp;
					
				} //end for(j) edge /angle sort
#if 0
				glBegin(GL_POLYGON); //draw slice and texture map it
				{
					for(int j=0; j< edges; ++j)
					{
						glTexCoord3fv(tcoord[order[j]]);
						glVertex3fv(poly[order[j]]);
					}
				}
				glEnd();
#else
				if(!gluvv.light.shadow)
					volSlice(edges, tcoord, poly, order);
				else
					volShadow(edges,tcoord,poly,order,ltxf,texNames[v],mv);
#endif
				//glFlush();
      //}//end else compute convex hull
   }// end for(i) each slice
	 // end SLICE ------------------------------------------------------
	 

	GlErr("r8kvolren::rva", "end");
}

//============================================================ Vol Slice
//======================================================================
//======================================================================

//standard renderer
inline void R8kVolRen3D::volSlice(int edges, 
											             float tc[6][3], 
											             float pc[6][3],
											             int   order[6])
{
	glBegin(GL_POLYGON); //draw slice and texture map it
	{
		for(int j=0; j< edges; ++j)
		{
			glMultiTexCoord3fvARB(GL_TEXTURE0_ARB,tc[order[j]]);
			glMultiTexCoord3fvARB(GL_TEXTURE2_ARB,tc[order[j]]);
			
			//------------- light and view attinuation ---------------
			float latt;
			if(gluvv.light.latt)
			{
				float ld[3];
				subV3(ld, pc[order[j]], vollt);
				latt = affine(ltrange[0],normV3(ld), ltrange[1], 1, 0);
			} else latt = 1;
			float vatt;
			if(gluvv.light.fog)
			{
				float vd[3];
				subV3(vd, pc[order[j]], volview);
				vatt = affine(fogrange[0], normV3(vd), fogrange[1], 
					            1, gluvv.light.fogThresh);
			} else vatt = 1;
			glSecondaryColor3fEXT(latt, vatt, 0);
			//------------- end light and view attinuation ---------------

			glVertex3fv(pc[order[j]]);
		}
	}
	glEnd();
}

//render with shadows ===================================
//======================================================================
//======================================================================
inline void R8kVolRen3D::volShadow(int edges,
																		 float tc[6][3],
																		 float pc[6][3],
																		 int   order[6],
																		 float ltxf[16],
																		 int   vol,
																		 double mv[16])
{

	//-------------- compute coords for light view
	float lv[6][4];  //light space verticies
	float lc[6][2];  //light buffer texture coords
	for(int i=0; i<edges; ++i)
	{
		//move the coords into light space
		//first the vertices
		int idx = order[i];
		translateV4_3W(lv[idx], ltxf, pc[idx]);
		lv[idx][0] /= lv[idx][3];
		lv[idx][1] /= lv[idx][3];
		//now the texture coords in screen space
		lc[idx][0] = ((lv[idx][0]*.85) + .5) * xscale;
		lc[idx][1] = ((lv[idx][1]*.85) + .5) * yscale;
		//get the light buffer coords (screen space) 
		lv[idx][0] = lc[idx][0];
		lv[idx][1] = lc[idx][1];
		//now figure out the rectangular region to copy
	}
	//-------------- end compute coords for light view
	
	//now draw to the frame buffer ---------------------------------

  //bind the light buffer
	glActiveTexture(GL_TEXTURE5_ARB); 
	{
		glDisable(GL_TEXTURE_3D);
		glEnable(GL_TEXTURE_2D);
    if(currentPbuff == 0)  //pick the last one drawn to
    {
  		wglBindTexImageARB(pbuff->buffer, WGL_FRONT_LEFT_ARB);
    }
    else
    {
  		wglBindTexImageARB(pbuff2->buffer, WGL_FRONT_LEFT_ARB);
    }
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	//draw slice and texture map it ----------------------------------
	glColor4f(1,1,1,1);
	glBegin(GL_POLYGON); 
	{
		for(int j=0; j< edges; ++j)
		{
			glMultiTexCoord3fvARB(GL_TEXTURE0_ARB,tc[order[j]]);
			glMultiTexCoord3fvARB(GL_TEXTURE2_ARB,tc[order[j]]);
			glMultiTexCoord2fvARB(GL_TEXTURE5_ARB,lc[order[j]]);

			//------------- light and view attinuation 
			float ld[3];
			subV3(ld, pc[order[j]], vollt);
			float latt;
			
			if(gluvv.light.latt)
			{
				float ld[3];
				subV3(ld, pc[order[j]], vollt);
				latt = affine(ltrange[0],normV3(ld), ltrange[1], 1, 0);
			} 
			else 
				latt = 1;
			
			float vatt;
			if(gluvv.light.fog)
			{
				float vd[3];
				subV3(vd, pc[order[j]], volview);
				vatt = affine(fogrange[0], normV3(vd), fogrange[1], 
					            1, gluvv.light.fogThresh);
			} 
			else 
				vatt = 1;
			
			//glSecondaryColor3fEXT(latt, vatt, 0);
			glSecondaryColor3fEXT(1, 1, 0);
			//------------- end light and view attinuation 

			glVertex3fv(pc[order[j]]);
		}
	}
	glEnd();
	//end draw slice and texture map it ----------------------------------
#if 1
	// unbind the light buffer
	glActiveTexture(GL_TEXTURE5_ARB); 
	{
    if(currentPbuff == 0)
  		wglReleaseTexImageARB(pbuff->buffer, WGL_FRONT_LEFT_ARB);
    else
  		wglReleaseTexImageARB(pbuff2->buffer, WGL_FRONT_LEFT_ARB);
		glDisable(GL_TEXTURE_2D);
	}
	//end now draw to the frame buffer -------------------------------
#endif

  //Render to the light buffer -------------------------------------
  if(currentPbuff == 1)
  {
		pbuff->MakeCurrent();
  }
  else
  {
		pbuff2->MakeCurrent();
  }
  //------- Puffer contex render
	{
		if(currentPbuff == 1)
		{
			glActiveTexture(GL_TEXTURE5_ARB); 
			{
				glEnable(GL_TEXTURE_2D);
				wglBindTexImageARB(pbuff2->buffer, WGL_FRONT_LEFT_ARB);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			glActiveTexture(GL_TEXTURE0_ARB);
			{
				glEnable(GL_TEXTURE_3D);
				glBindTexture(GL_TEXTURE_3D, vol);
			}
		}
		else 
		{
			glActiveTexture(GL_TEXTURE5_ARB); 
			{
				glEnable(GL_TEXTURE_2D);
				wglBindTexImageARB(pbuff->buffer, WGL_FRONT_LEFT_ARB);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
      glActiveTexture(GL_TEXTURE0_ARB);
			{
				glEnable(GL_TEXTURE_3D);
				glBindTexture(GL_TEXTURE_3D, vol);
			}
		}
		
		//render the slice! ---------------------------------
		glEnable(GL_FRAGMENT_SHADER_ATI);
		{
			//glEnable(GL_BLEND);
			//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
			glDisable(GL_BLEND);
			glBegin(GL_POLYGON);
			{
				glColor4f(0,1,1,1);
				for(int j=0; j< edges; ++j)
        {	
					glMultiTexCoord3fvARB(GL_TEXTURE0_ARB,tc[order[j]]);
					glMultiTexCoord2fvARB(GL_TEXTURE4_ARB,lc[order[j]]);
					glMultiTexCoord2fvARB(GL_TEXTURE5_ARB,lc[order[j]]);
					
					glVertex2fv(lc[order[j]]);
				}
			} glEnd();
			
			//glDisable(GL_BLEND);
		}glDisable(GL_FRAGMENT_SHADER_ATI);
		//end render the slice! ---------------------------------
		
		if(currentPbuff == 1)
		{
			glActiveTexture(GL_TEXTURE5_ARB); 
			{
				wglReleaseTexImageARB(pbuff2->buffer, WGL_FRONT_LEFT_ARB);
				glDisable(GL_TEXTURE_2D);
			}
      glActiveTexture(GL_TEXTURE0_ARB);
			{
				//glBindTexture(GL_TEXTURE_3D, 0);
				glDisable(GL_TEXTURE_3D);
			}
		}
		else 
		{
			glActiveTexture(GL_TEXTURE5_ARB); 
			{
				wglReleaseTexImageARB(pbuff->buffer, WGL_FRONT_LEFT_ARB);
				glDisable(GL_TEXTURE_2D);
			}
      glActiveTexture(GL_TEXTURE0_ARB);
			{
				//glBindTexture(GL_TEXTURE_3D, 0);
				glDisable(GL_TEXTURE_3D);
			}
		}
	} MakeCurrent();
#if 1
	if(currentPbuff == 1) 
		currentPbuff = 0;
	else
		currentPbuff = 1;
#endif
	//end Render to the light buffer ---------------------------------
		
	GlErr("volren","end of shadow slice");

}

//============================================================ INTERSECT
//======================================================================
//======================================================================

inline int R8kVolRen3D::intersect(
		     const float p0[3], const float p1[3], //line end points
		     const float t0[3], const float t1[3], //texture points
		     const float v0[3], const float v1[3], //view coord points
		     const float sp[3], const float sn[3], //plane point & norm
		     float pnew[3], float tnew[3], float vnew[3]) //new values 
{
   //t = (sn.(sp - p0))/(sn.(p1 - p0))
   float t = ((sn[0]*(sp[0] - p0[0]) + sn[1]*(sp[1] - p0[1]) 
	       + sn[2]*(sp[2] - p0[2])) / 
	      (sn[0]*(p1[0] - p0[0]) + sn[1]*(p1[1] - p0[1])
	       + sn[2]*(p1[2] - p0[2])));
   //note if the denominator is zero t is a NAN so we should have no problems?
   
   if( (t>=0) && (t<=1) )
	 {
      //compute line intersection
      pnew[0] = p0[0] + t*(p1[0] - p0[0]); 
      pnew[1] = p0[1] + t*(p1[1] - p0[1]); 
      pnew[2] = p0[2] + t*(p1[2] - p0[2]);
      //compute texture interseciton
      tnew[0] = t0[0] + t*(t1[0] - t0[0]); 
      tnew[1] = t0[1] + t*(t1[1] - t0[1]); 
      tnew[2] = t0[2] + t*(t1[2] - t0[2]);
      //compute view coordinate intersections
      vnew[0] = v0[0] + t*(v1[0] - v0[0]); 
      vnew[1] = v0[1] + t*(v1[1] - v0[1]); 
      vnew[2] = v0[2] + t*(v1[2] - v0[2]);
      return 1;
   }
   return 0;
} 


//======================================================= Create Volumes
//======================================================================
//======================================================================

void R8kVolRen3D::createVolumes(int init)
{
	if(!glTexImage3DEXT){
		go = 0;
		return;
	}

	if(gluvv.mv){
			createBricks(init);
	} else {
		go = 0;
	}
}


//======================================================= Create Bricks
//======================================================================
//======================================================================
void R8kVolRen3D::createBricks(int init)
{
	if(init)
	{
		texNames = new unsigned int[gluvv.mv->numSubVols];
		shadeNames = new unsigned int[gluvv.mv->numSubVols];
		glGenTextures(gluvv.mv->numSubVols, texNames);
		glGenTextures(gluvv.mv->numSubVols, shadeNames);
	}

	int ne = gluvv.mv->nelts;

	for(int v=0; v<gluvv.mv->numSubVols; ++v)
	{
		int sx = gluvv.mv->volumes[v].xiSize;
		int sy = gluvv.mv->volumes[v].yiSize;
		int sz = gluvv.mv->volumes[v].ziSize;

		unsigned char *d = gluvv.mv->volumes[v].currentData;
		unsigned char *tex3D;
		int i, j, k;
	

		switch(gluvv.dmode)
		{
		case GDM_V1: //one byte texture
		case GDM_VGH_V:
			//this is where the texture data goes
			cerr << "Creating 1 byte volume ";
			tex3D = new unsigned char[sx*sy*sz];
			for(i = 0; i < sz; ++i)
			{
				for(j = 0; j < sy; ++j)
				{
					for(k = 0; k < sx; ++ k)
					{
						tex3D[i*sx*sy + j*sx + k] = d[i*sx*sy*ne + j*sx*ne + k*ne + 0];
					}
				}
			}
			loadTex1B(tex3D, texNames[v], sx, sy, sz);	
			cerr << " -done" << endl;
			break;
		case GDM_V1G: //two byte texture
		case GDM_V2:
		case GDM_VGH_VG:
			cerr << "Creating 2 byte volume ";
			tex3D = new unsigned char[sx*sy*sz*2];
			for(i = 0; i < sz; ++i)
			{
				for(j = 0; j < sy; ++j)
				{
					for(k = 0; k < sx; ++ k)
					{
						tex3D[i*sx*sy*2 + j*sx*2 + k*2 + 0] = d[i*sx*sy*ne + j*sx*ne + k*ne + 0];
						tex3D[i*sx*sy*2 + j*sx*2 + k*2 + 1] = d[i*sx*sy*ne + j*sx*ne + k*ne + 1];
					}
				}
			}
			loadTex2B(tex3D, texNames[v], sx, sy, sz);
			delete[] tex3D;
			cerr << " -done" << endl;
			break;
		case GDM_V1GH: //3 or 4 byte data == 4 byte texture
		case GDM_V2G:
		case GDM_V2GH:
		case GDM_V3:
		case GDM_V3G:
		case GDM_V4:
		case GDM_VGH:
			cerr << "Creating 4 byte volume ";
			tex3D = new unsigned char[sx*sy*sz*4];
			for(i = 0; i < sz; ++i)
			{
				for(j = 0; j < sy; ++j)
				{
					for(k = 0; k < sx; ++ k)
					{
						tex3D[i*sx*sy*4 + j*sx*4 + k*4 + 0] = d[i*sx*sy*ne + j*sx*ne + k*ne + 0];
						tex3D[i*sx*sy*4 + j*sx*4 + k*4 + 1] = d[i*sx*sy*ne + j*sx*ne + k*ne + 1];
						tex3D[i*sx*sy*4 + j*sx*4 + k*4 + 2] = d[i*sx*sy*ne + j*sx*ne + k*ne + 2];
						if(ne > 3)
							tex3D[i*sx*sy*4 + j*sx*4 + k*4 + 3] = d[i*sx*sy*ne + j*sx*ne + k*ne + 3];
						else
							tex3D[i*sx*sy*4 + j*sx*4 + k*4 + 3] = 50;
					}
				}
			}
			loadTex4B(tex3D, texNames[v], sx, sy, sz);
			delete[] tex3D;
			cerr << " -done" << endl;
			break;
		default:
			cerr << "Error R8kVolRen3D::createBricks(), data type unknown" << endl;
			return;
		}

		if(gluvv.mv->volumes[v].currentGrad)
		{
			cerr << " Down Loading Shade Texture " << v << " ... ";
			glBindTexture(GL_TEXTURE_3D, shadeNames[v]);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			
			glTexImage3DEXT(GL_TEXTURE_3D,
				0,
				GL_RGB8,
				sx,
				sy,
				sz,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				gluvv.mv->volumes[v].currentGrad);
		}

		glFlush();
		GlErr("R8kVolRen3D", "Create Texture - Shade Texture");
		cerr << "Done" << endl;
	


	}//for each sub-volume

	glDisable(GL_TEXTURE_3D);
}

//=================================================== load 1 byte volume
//======================================================================
//======================================================================
void R8kVolRen3D::loadTex1B(unsigned char *tex, unsigned int name, int sx, int sy, int sz)
{
		cerr << " Down Loading Data Texture " << name << " ... ";
		glEnable(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, name);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		glTexImage3DEXT(GL_TEXTURE_3D,
			0,
			GL_ALPHA8,
			sx,
			sy,
			sz,
			0,
			GL_ALPHA,
			GL_UNSIGNED_BYTE,
			tex);
		
		glFlush();
		GlErr("R8kVolRen3D", "Create Texture - Data Texture");	
		cerr << "Done" << endl;	
}

//=================================================== load 2 byte volume
//======================================================================
//======================================================================
void R8kVolRen3D::loadTex2B(unsigned char *tex, unsigned int name, int sx, int sy, int sz)
{
		cerr << " Down Loading Data Texture " << name << " ... ";
		glEnable(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, name);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		glTexImage3DEXT(GL_TEXTURE_3D,
			0,
			GL_LUMINANCE8_ALPHA8,
			sx,
			sy,
			sz,
			0,
			GL_LUMINANCE_ALPHA,
			GL_UNSIGNED_BYTE,
			tex);
		
		glFlush();
		GlErr("R8kVolRen3D", "Create Texture - Data Texture");	
		cerr << "Done" << endl;	
}

//=================================================== load 3 byte volume
//======================================================================
//======================================================================
void R8kVolRen3D::loadTex3B(unsigned char *tex, unsigned int name, int sx, int sy, int sz)
{
		cerr << " Down Loading Data Texture " << name << " ... ";
		glEnable(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, name);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		glTexImage3DEXT(GL_TEXTURE_3D,
			0,
			GL_RGB8,
			sx,
			sy,
			sz,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			tex);
		
		glFlush();
		GlErr("R8kVolRen3D", "Create Texture - Data Texture");	
		cerr << "Done" << endl;	
}

//=================================================== load 4 byte volume
//======================================================================
//======================================================================
void R8kVolRen3D::loadTex4B(unsigned char *tex, unsigned int name, int sx, int sy, int sz)
{
		cerr << " Down Loading Data Texture " << name << " ... ";
		glEnable(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, name);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		glTexImage3DEXT(GL_TEXTURE_3D,
			0,
			GL_RGBA8,
			sx,
			sy,
			sz,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			tex);
		
		glFlush();
		GlErr("R8kVolRen3D", "Create Texture - Data Texture");	
		cerr << "Done" << endl;	
}



//==================================================== Create 2D Dep Tex
//======================================================================
//======================================================================

void R8kVolRen3D::create2DDepTex()
{
	//------------- load gb deptex --------------------
	glGenTextures(1, &deptexName);   //the dep tex that we use for the tf
	glGenTextures(1, &gluvv.volren.deptexName);  //the dep tex that the tf widget sees
	glGenTextures(1, &gluvv.volren.deptex3Name); //the dep tex for the shadow tf

	int dsx = gluvv.tf.ptexsz[0];
	int dsy = gluvv.tf.ptexsz[1];

	deptex = new unsigned char[dsx*dsy*4]; //reference texture
	gluvv.volren.deptex3 = new unsigned char[dsx*dsy*4]; //shadow transfer function

	gluvv.volren.deptex = deptex;

	unsigned char *stf = gluvv.volren.deptex3;

	for(int j=0; j<dsy; ++j)
	{
		for(int k=0; k<dsx; ++k)
		{
			deptex[j*dsx*4 + k*4 + 0] = stf[j*dsx*4 + k*4 + 0] =  k/(float)dsx*255;							
			deptex[j*dsx*4 + k*4 + 1] = stf[j*dsx*4 + k*4 + 1] = 	j/(float)dsy*255;//k==0 ? 0 : 255;							
			deptex[j*dsx*4 + k*4 + 2] = stf[j*dsx*4 + k*4 + 2] =	255 - j/(float)dsy*255; //k==0 ? 255 : 0;							
			deptex[j*dsx*4 + k*4 + 3] = stf[j*dsx*4 + k*4 + 3] =	(unsigned char)(j/(float)dsy*255/(float)2);							
		}
	}
		
	gDeptex = new unsigned char[dsx*dsy*4]; //for good sample rates
	iDeptex = new unsigned char[dsx*dsy*4]; //for interactive sample rate
	copyScale(gDeptex, gluvv.volren.goodSamp, deptex);
	copyScale(iDeptex, gluvv.volren.interactSamp, deptex);

	gDeptex3 = new unsigned char[dsx*dsy*4]; //for good sample rates
	iDeptex3 = new unsigned char[dsx*dsy*4]; //for interactive sample rate
	copyScale(gDeptex3, gluvv.volren.goodSamp, deptex);
	copyScale(iDeptex3, gluvv.volren.interactSamp, deptex);

	lastSamp = gluvv.volren.goodSamp;
	lastGoodSamp = gluvv.volren.goodSamp;
	lastInteSamp = gluvv.volren.interactSamp;

	loadDepTex(gDeptex);

	loadDepTex(gDeptex3, gluvv.volren.deptex3Name);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gluvv.volren.deptexName);
		
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			dsx,
			dsy,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			deptex);
	glFlush();

	//Load the shadows tf
	glBindTexture(GL_TEXTURE_2D, gluvv.volren.deptex3Name);
		
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			dsx,
			dsy,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			deptex);
	glFlush();

	//--------------- load ar deptex ------------------
	glGenTextures(1, &deptex2Name);   //the dep tex that we use for the tf
	glGenTextures(1, &gluvv.volren.deptex2Name);  //the dep tex that the tf widget sees

	deptex2 = new unsigned char[dsx*dsy*4]; //reference texture

	gluvv.volren.deptex2 = deptex2;

	for(j=0; j<dsy; ++j)
	{
		for(int k=0; k<dsx; ++k)
		{
			deptex2[j*dsx*4 + k*4 + 0] =  k/(float)dsx *255;							
			deptex2[j*dsx*4 + k*4 + 1] = 	j/(float)dsy *255;//k==0 ? 0 : 255;							
			deptex2[j*dsx*4 + k*4 + 2] = 	255 - j/(float)dsy*255; //k==0 ? 255 : 0;							
			deptex2[j*dsx*4 + k*4 + 3] = 	255;							
		}
	}
		
	loadDepTex(deptex2, deptex2Name);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gluvv.volren.deptex2Name);
		
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			dsx,
			dsy,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			deptex2);
	glFlush();


}

//========================================================= load Dep tex
//======================================================================
//======================================================================
void R8kVolRen3D::loadDepTex(unsigned char *dtex)
{
	GlErr("R8kVolRen3D", "before load dep tex 1");

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, deptexName);
		
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			gluvv.tf.ptexsz[0],
			gluvv.tf.ptexsz[1],
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			dtex);
	glFlush();

	GlErr("R8kVolRen3D", "load dep tex 1");
}

void R8kVolRen3D::loadDepTex(unsigned char *dtex, unsigned int name)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, name);
		
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			gluvv.tf.ptexsz[0],
			gluvv.tf.ptexsz[1],
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			dtex);
	glFlush();
	
	GlErr("R8kVolRen3D", "load dep tex 2");

}

//==================================================== create Shadow tex
//======================================================================
//======================================================================
void R8kVolRen3D::createShadowTex()  //creates the shadow texture
{
	glGenTextures(1, &shadowName);   //the shadow texture
	unsigned char *stex = new unsigned char[gluvv.light.buffsz[0]*gluvv.light.buffsz[1]*4];

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, shadowName);		
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			gluvv.light.buffsz[0],
			gluvv.light.buffsz[1],
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			stex);
	glFlush();
	glDisable(GL_TEXTURE_2D);

	glGenTextures(1, &pbuffName);   //the pbuff texture name
	glBindTexture(GL_TEXTURE_2D, pbuffName);		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	GlErr("r8kvolren", "create shadow textures");
}

//============================================= create & update Cube Tex
//======================================================================
//======================================================================
void R8kVolRen3D::createCubeTex()    //creates the cube map for shading
{
	cerr << "Creating the cube map for shading - ";
	int csz = gluvv.light.csz;
	float dx = 2.0/(float)(csz-1);
	unsigned char *ctex = new unsigned char[csz*csz*4];

	glGenTextures(1, &gluvv.light.cubeName);
	glEnable(GL_TEXTURE_CUBE_MAP_ARB);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, gluvv.light.cubeName);
		
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	//X Pos ----------------------------------------
	gluvv.light.cubeKey[0] = new float[csz*csz*3];
	for(int i=0; i<csz; ++i)
	{ //z
		for(int j=0; j<csz; ++j)
		{ //y
			gluvv.light.cubeKey[0][i*csz*3 + j*3 + 0] = 1;			
			gluvv.light.cubeKey[0][i*csz*3 + j*3 + 1] = 1-j*dx;			
			gluvv.light.cubeKey[0][i*csz*3 + j*3 + 2] = 1-i*dx;
			normalizeV3(&gluvv.light.cubeKey[0][i*csz*3 + j*3 + 0]);
			//negateV3(&gluvv.light.cubeKey[0][i*csz*3 + j*3 + 0]);
			ctex[i*csz*4 + j*4 + 0] = 255;
			ctex[i*csz*4 + j*4 + 1] = 0;
			ctex[i*csz*4 + j*4 + 2] = 0;
			ctex[i*csz*4 + j*4 + 3] = 255;
		}
	}	
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
			0,
			GL_RGBA8,
			csz,
			csz,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			ctex);
	
	//X Neg
	gluvv.light.cubeKey[1] = new float[csz*csz*3];
	for(i=0; i<csz; ++i)
	{ //z
		for(int j=0; j<csz; ++j)
		{ //y
			gluvv.light.cubeKey[1][i*csz*3 + j*3 + 0] = -1;			
			gluvv.light.cubeKey[1][i*csz*3 + j*3 + 1] = 1-j*dx;			
			gluvv.light.cubeKey[1][i*csz*3 + j*3 + 2] = -1+i*dx;
			normalizeV3(&gluvv.light.cubeKey[1][i*csz*3 + j*3 + 0]);
			//negateV3(&gluvv.light.cubeKey[1][i*csz*3 + j*3 + 0]);
			ctex[i*csz*4 + j*4 + 0] = 255;
			ctex[i*csz*4 + j*4 + 1] = 255;
			ctex[i*csz*4 + j*4 + 2] = 0;
			ctex[i*csz*4 + j*4 + 3] = 255;
		}
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
			0,
			GL_RGBA8,
			csz,
			csz,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			ctex);

	//Y Pos
	gluvv.light.cubeKey[2] = new float[csz*csz*3];
	for(i=0; i<csz; ++i)
	{ //z
		for(int j=0; j<csz; ++j)
		{ //x
			gluvv.light.cubeKey[2][i*csz*3 + j*3 + 0] = -1+i*dx;			
			gluvv.light.cubeKey[2][i*csz*3 + j*3 + 1] = 1;			
			gluvv.light.cubeKey[2][i*csz*3 + j*3 + 2] = -1+j*dx;
			normalizeV3(&gluvv.light.cubeKey[2][i*csz*3 + j*3 + 0]);
			//negateV3(&gluvv.light.cubeKey[2][i*csz*3 + j*3 + 0]);
			ctex[i*csz*4 + j*4 + 0] = 0;
			ctex[i*csz*4 + j*4 + 1] = 255;
			ctex[i*csz*4 + j*4 + 2] = 0;
			ctex[i*csz*4 + j*4 + 3] = 255;
		}
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
			0,
			GL_RGBA8,
			csz,
			csz,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			ctex);

	//Y Neg
	gluvv.light.cubeKey[3] = new float[csz*csz*3];
	for(i=0; i<csz; ++i)
	{ //z
		for(int j=0; j<csz; ++j)
		{ //x
			gluvv.light.cubeKey[3][i*csz*3 + j*3 + 0] = -1+i*dx;			
			gluvv.light.cubeKey[3][i*csz*3 + j*3 + 1] = -1;			
			gluvv.light.cubeKey[3][i*csz*3 + j*3 + 2] = 1-j*dx;
			normalizeV3(&gluvv.light.cubeKey[3][i*csz*3 + j*3 + 0]);
			//negateV3(&gluvv.light.cubeKey[3][i*csz*3 + j*3 + 0]);
			ctex[i*csz*4 + j*4 + 0] = 0;
			ctex[i*csz*4 + j*4 + 1] = 255;
			ctex[i*csz*4 + j*4 + 2] = 255;
			ctex[i*csz*4 + j*4 + 3] = 255;
		}
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
			0,
			GL_RGBA8,
			csz,
			csz,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			ctex);

	//Z Pos
	gluvv.light.cubeKey[4] = new float[csz*csz*3];
	for(i=0; i<csz; ++i)
	{ //y
		for(int j=0; j<csz; ++j)
		{ //x
			gluvv.light.cubeKey[4][i*csz*3 + j*3 + 0] = -1+i*dx;			
			gluvv.light.cubeKey[4][i*csz*3 + j*3 + 1] = 1-j*dx;			
			gluvv.light.cubeKey[4][i*csz*3 + j*3 + 2] = 1;
			normalizeV3(&gluvv.light.cubeKey[4][i*csz*3 + j*3 + 0]);
			//negateV3(&gluvv.light.cubeKey[4][i*csz*3 + j*3 + 0]);
			ctex[i*csz*4 + j*4 + 0] = 0;
			ctex[i*csz*4 + j*4 + 1] = 0;
			ctex[i*csz*4 + j*4 + 2] = 255;
			ctex[i*csz*4 + j*4 + 3] = 255;
		}
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
			0,
			GL_RGBA8,
			csz,
			csz,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			ctex);

	//Z Neg
	gluvv.light.cubeKey[5] = new float[csz*csz*3];
	for(i=0; i<csz; ++i)
	{ //y
		for(int j=0; j<csz; ++j)
		{ //x
			gluvv.light.cubeKey[5][i*csz*3 + j*3 + 0] = 1-i*dx;			
			gluvv.light.cubeKey[5][i*csz*3 + j*3 + 1] = 1-j*dx;			
			gluvv.light.cubeKey[5][i*csz*3 + j*3 + 2] = -1;
			normalizeV3(&gluvv.light.cubeKey[5][i*csz*3 + j*3 + 0]);
			//negateV3(&gluvv.light.cubeKey[5][i*csz*3 + j*3 + 0]);
			ctex[i*csz*4 + j*4 + 0] = 255;
			ctex[i*csz*4 + j*4 + 1] = 0;
			ctex[i*csz*4 + j*4 + 2] = 255;
			ctex[i*csz*4 + j*4 + 3] = 255;
		}
	}
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB,
			0,
			GL_RGBA8,
			csz,
			csz,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			ctex);


	glFlush();
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	cerr << "Done" << endl;
	delete[] ctex;
	GlErr("R8kVolRen::createCubeTex", "after load");
}

//======================================================================
//======================================================================
void R8kVolRen3D::loadCubeFace(unsigned char *tex, unsigned int face, int sz)
{
		glTexImage2D(face,
			0,
			GL_RGBA8,
			sz,
			sz,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			tex);
}


//======================================================================
//======================================================================
void R8kVolRen3D::loadCubeTex(int good)      //updates cube map for shading
{
	int csz = gluvv.light.csz;

	
		float ldir[3];
		copyV3(ldir, gluvv.light.pos);
		negateV3(ldir);
		normalizeV3(ldir);
		float vdir[3];
		subV3(vdir, gluvv.env.eye, gluvv.env.at);
		negateV3(vdir);
		normalizeV3(vdir);
		float ltoe[3];
		subV3(ltoe,vdir,ldir);
		scaleV3(.5, ltoe);
		float half[3];
		addV3(half,ldir, ltoe);
		normalizeV3(half);
		unsigned char *tex = new unsigned char[csz*csz*4];
		
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, gluvv.light.cubeName);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		for(unsigned int f=0; f<6; ++f)
		{ //for each face
			for(int i=0; i<csz; ++i)
			{ //for y
				for(int j=0; j<csz; ++j)
				{ // for x
					float n[3];
					copyV3(n,&gluvv.light.cubeKey[f][i*csz*3 + j*3]);
					float diff = CLAMP(dotV3(ldir, n));
					float spec = CLAMP(dotV3(half, n));
					float sill = ABS(dotV3(vdir, n));
					negateV3(n);
					diff += CLAMP(dotV3(ldir,n));
					spec += CLAMP(dotV3(half,n));
					spec = pow(spec, 30);

					sill = pow(1.0-sill, 10);
					//if(sill > .5) sill = 1.0;
					//else sill = 0.0;
					//diff = ((diff * diff * diff) * 2 + diff)/3;
					tex[j*csz*4 + i*4 + 0] = (CLAMP(MAX(diff,.2))*gluvv.light.intens)*255;
					tex[j*csz*4 + i*4 + 1] = (CLAMP(spec)*gluvv.light.intens)*255;
					tex[j*csz*4 + i*4 + 2] = (1.0-sill)*255; //unused
					tex[j*csz*4 + i*4 + 3] = (1.0-sill)*255;
					
				}
			}
			loadCubeFace(tex, GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + f, csz);
		}//end for each face
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, 0);
		delete[] tex;
}

//=================================================== copy & scale alpha
//======================================================================
//======================================================================

void R8kVolRen3D::scaleAlpha(unsigned char *dout, float nsr, float osr, unsigned char *din)
{
	if(nsr == osr) return;

	int dsx = gluvv.tf.ptexsz[0];
	int dsy = gluvv.tf.ptexsz[1];

	float alphaScale = 1/nsr;
	for(int i=0; i<dsy; ++i)
	{
		for(int j=0; j<dsx; ++j)
		{
			dout[i*dsx*4 + j*4 + 3] = 
				(1- pow((1.0-din[i*dsx*4 + j*4 + 3]/255.0), alphaScale))*255;
		}
	}
}

//======================================================================
//======================================================================
void R8kVolRen3D::copyScale(unsigned char *dout, float sr, unsigned char *din)
{
	int dsx = gluvv.tf.ptexsz[0];
	int dsy = gluvv.tf.ptexsz[1];
	
	float alphaScale = 1.0/sr;
	for(int i=0; i<dsy; ++i)
	{
		for(int j=0; j<dsx; ++j)
		{
			dout[i*dsx*4 + j*4 + 0] = din[i*dsx*4 + j*4 + 0];
			dout[i*dsx*4 + j*4 + 1] = din[i*dsx*4 + j*4 + 1];
			dout[i*dsx*4 + j*4 + 2] = din[i*dsx*4 + j*4 + 2];
			dout[i*dsx*4 + j*4 + 3] = 
				(1.0 - pow((1.0-(din[i*dsx*4 + j*4 + 3]/255.0)), alphaScale))*255;
		}
	}
}

//========================================================= make current
//======================================================================
//======================================================================

void R8kVolRen3D::MakeCurrent()
{
	glutSetWindow(gluvv.mainWindow);
}

//============================================== create Fragment Shaders
//======================================================================
//======================================================================
//======================================================================
//======================================================================

void R8kVolRen3D::createFragShades(unsigned int *&names) // create and update fragment shaders
{
	if(!names){
		names = new unsigned int[20];
		for(int i=0; i<20; ++i) names[i] = -1;
	}

	int thirdAxis = 0;
	switch(gluvv.dmode)
	{ //third axis switch, to use or not to use???
		case GDM_VGH:
		case GDM_V1GH:
		case GDM_V2G:
		case GDM_V2GH:
		case GDM_V3:
		case GDM_V3G:
		case GDM_V4:
			thirdAxis = 1;
			break;
		default:
			thirdAxis = 0;
			break;
	}

	//=================================================================
	//=============================================================== 0

	for(int i=0; i<10; ++i)
	{
		if(names[i] == -1)
			names[i] = glGenFragmentShadersATI(1);
		
		glEnable(GL_FRAGMENT_SHADER_ATI);
		{            //The Shader 
			glBindFragmentShaderATI(names[i]);
			glBeginFragmentShaderATI();
			{
				//---------------- 1st Pass -----------------------------
				//----- routing instructions ------
				//grab the tex coords for tex 0
				glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
				if((i==4)||(i==5)||(i==6)||(i==7)||(i==9)||(1))
				{    //shading
					glSampleMapATI(GL_REG_2_ATI, GL_TEXTURE2_ARB, GL_SWIZZLE_STR_ATI);					
				}
				// shadows
				if((i==2)||(i==3)||(i==6)||(i==7))
				{ 
					//glSampleMapATI(GL_REG_5_ATI, GL_TEXTURE5_ARB, GL_SWIZZLE_STR_ATI);
				}
				//----- end routing instructions --
				
				//----- math ----------------------
				//I doubt the next one is neccissary
				
				if(thirdAxis)
				{
          //just moving data to another register so we can keep 0 free
					glColorFragmentOp2ATI(GL_MUL_ATI, 
						GL_REG_1_ATI, GL_NONE, GL_SATURATE_BIT_ATI,
						GL_REG_0_ATI, GL_NONE, GL_NONE,
						GL_ONE,       GL_NONE, GL_NONE);
					glAlphaFragmentOp2ATI(GL_MUL_ATI, 
						GL_REG_1_ATI, GL_SATURATE_BIT_ATI,
						GL_REG_0_ATI, GL_NONE, GL_NONE,
						GL_ONE,       GL_NONE, GL_NONE);

					//swizzle 0.G to 3.R
					glColorFragmentOp1ATI(GL_MOV_ATI, 
						GL_REG_3_ATI, GL_RED_BIT_ATI, GL_NONE,
						GL_REG_0_ATI, GL_BLUE, GL_NONE);

					//swizzle 0.A to 3.G
					glColorFragmentOp1ATI(GL_MOV_ATI, 
						GL_REG_3_ATI, GL_GREEN_BIT_ATI, GL_NONE,
						GL_REG_0_ATI, GL_ALPHA, GL_NONE);

				} 
				else 
				{
					//swizzle 0.G to 5.R
					glColorFragmentOp1ATI(GL_MOV_ATI, 
						GL_REG_1_ATI, GL_RED_BIT_ATI, GL_NONE,
						GL_REG_0_ATI, GL_RED, GL_NONE);

					//swizzle 0.A to 5.G
					glColorFragmentOp1ATI(GL_MOV_ATI, 
						GL_REG_1_ATI, GL_GREEN_BIT_ATI, GL_NONE,
						GL_REG_0_ATI, GL_ALPHA, GL_NONE);

				}
				
				if((i == 4)||(i == 5)||(i==6)||(i==7)||(i==9))
				{ //shading transform
					glColorFragmentOp2ATI(GL_DOT3_ATI, 
						GL_REG_4_ATI, GL_GREEN_BIT_ATI,  GL_NONE,
						GL_CON_1_ATI, GL_NONE,   GL_BIAS_BIT_ATI | GL_2X_BIT_ATI,
						GL_REG_2_ATI, GL_NONE,   GL_BIAS_BIT_ATI | GL_2X_BIT_ATI);
					
					glColorFragmentOp2ATI(GL_DOT3_ATI, 
						GL_REG_4_ATI, GL_RED_BIT_ATI,  GL_NONE,
						GL_CON_0_ATI, GL_NONE, GL_BIAS_BIT_ATI | GL_2X_BIT_ATI,
						GL_REG_2_ATI, GL_NONE, GL_BIAS_BIT_ATI | GL_2X_BIT_ATI);
					
					glColorFragmentOp2ATI(GL_DOT3_ATI, 
						GL_REG_4_ATI, GL_BLUE_BIT_ATI,  GL_NONE,
						GL_CON_2_ATI, GL_NONE,  GL_BIAS_BIT_ATI | GL_2X_BIT_ATI,
						GL_REG_2_ATI, GL_NONE,  GL_BIAS_BIT_ATI | GL_2X_BIT_ATI);
				}
				
				//----- end math ------------------
				
				//---------------- 2nd Pass -----------------------------
				//----- routing instructions ------
				// get values from 1st pass
				//glPassTexCoordATI(GL_REG_0_ATI, GL_REG_0_ATI, GL_SWIZZLE_STRQ_ATI);
				glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
				// V & G transfer function
				glSampleMapATI(GL_REG_1_ATI, GL_REG_1_ATI, GL_SWIZZLE_STR_ATI);
				// H & 4 transfer function
				if(thirdAxis)
				{
					glSampleMapATI(GL_REG_3_ATI, GL_REG_3_ATI, GL_SWIZZLE_STR_ATI);
				}
				// shading
				if((i == 4)||(i == 5)||(i==6)||(i==7)||(i==9))
				{ 
					glSampleMapATI(GL_REG_4_ATI, GL_REG_4_ATI, GL_SWIZZLE_STR_ATI);
				}
				//shadows
				if((i==2)||(i==3)||(i==6)||(i==7))
				{ 
					glSampleMapATI(GL_REG_5_ATI, GL_TEXTURE5_ARB, GL_SWIZZLE_STR_ATI);
					//glPassTexCoordATI(GL_REG_5_ATI, GL_REG_5_ATI, GL_SWIZZLE_STRQ_ATI);
				}
				//----- end routing instructions --
				
				//----- math ----------------------
#if 0
				glColorFragmentOp3ATI(GL_LERP_ATI, // fog
					GL_REG_0_ATI, GL_NONE, GL_NONE,
					GL_SECONDARY_INTERPOLATOR_ATI, GL_GREEN, GL_NONE,
					GL_REG_1_ATI, GL_NONE, GL_NONE,
					GL_CON_3_ATI, GL_NONE, GL_NONE);
#endif
				if((i==4)||(i==5)||(i==6)||(i==7))
				{  // 1
					glColorFragmentOp3ATI(GL_MAD_ATI,  //color*(diff) + specular
						GL_REG_3_ATI, GL_NONE, GL_NONE,
						GL_REG_4_ATI, GL_RED,  GL_NONE,
						GL_REG_1_ATI, GL_NONE, GL_NONE,
						GL_REG_4_ATI, GL_GREEN,GL_NONE);
				
					glColorFragmentOp2ATI(GL_MUL_ATI,  //color*(diff) + specular
						GL_REG_4_ATI, GL_BLUE_BIT_ATI, GL_SATURATE_BIT_ATI,
						GL_REG_0_ATI, GL_GREEN,  GL_NONE, //GL_2X_BIT_ATI,
						GL_REG_0_ATI, GL_GREEN,  GL_NONE); //GL_2X_BIT_ATI);
					
					
					glColorFragmentOp3ATI(GL_LERP_ATI, // mod shading by gradient
						GL_REG_0_ATI, GL_NONE,  GL_NONE,
						GL_REG_0_ATI, GL_GREEN, GL_NONE, //GL_2X_BIT_ATI,
						GL_REG_3_ATI, GL_NONE,  GL_NONE,
						GL_REG_1_ATI, GL_NONE,  GL_NONE);
				} 
				else 
				{				
					glColorFragmentOp2ATI(GL_MUL_ATI,    //useless op
						GL_REG_0_ATI, GL_NONE, GL_NONE,
						GL_REG_1_ATI, GL_NONE, GL_NONE,
						GL_ONE,       GL_NONE, GL_NONE);
				}
				
				if(thirdAxis)
				{
					glAlphaFragmentOp2ATI(GL_MUL_ATI,    //compose the 3rd axis
						GL_REG_0_ATI, GL_SATURATE_BIT_ATI,
						GL_REG_1_ATI, GL_NONE, GL_NONE,
						GL_REG_3_ATI, GL_NONE, GL_NONE);
				} 
				else 
				{
					glAlphaFragmentOp2ATI(GL_MUL_ATI,    //set up alpha
						GL_REG_0_ATI, GL_SATURATE_BIT_ATI,
						GL_REG_1_ATI, GL_NONE, GL_NONE,
						GL_ONE,       GL_NONE, GL_NONE);
				}
				
				if((i==2)||(i==3)||(i==6)||(i==7))
				{           // 2 shadows
#if 1
					glColorFragmentOp2ATI(GL_MUL_ATI, 
						GL_REG_0_ATI, GL_NONE, GL_NONE,
						GL_REG_0_ATI, GL_NONE, GL_NONE,
						GL_REG_5_ATI, GL_NONE, GL_COMP_BIT_ATI);					
#if 0
					glColorFragmentOp2ATI(GL_MUL_ATI, 
						GL_REG_0_ATI, GL_NONE, GL_NONE,
						GL_REG_0_ATI, GL_NONE, GL_NONE,
						GL_REG_5_ATI, GL_ALPHA, GL_COMP_BIT_ATI);					
#endif
#endif
				}
				
				glAlphaFragmentOp2ATI(GL_ADD_ATI,   //set up edge biased opacity multiplier
					GL_REG_4_ATI, GL_NONE,            // for silhouette edges
					GL_REG_4_ATI, GL_NONE, GL_COMP_BIT_ATI,
					GL_ONE      , GL_NONE, GL_NONE);
#if 0				
				if((i==9))
				{//silhouette edges
					glColorFragmentOp3ATI(GL_LERP_ATI, //4 blend r0= r0*r0.a + (1 - r0.a)*r3
						GL_REG_0_ATI, GL_NONE, GL_NONE,
						GL_REG_4_ATI, GL_BLUE, GL_COMP_BIT_ATI,
						GL_ZERO,      GL_NONE, GL_NONE,
						GL_REG_0_ATI, GL_NONE, GL_NONE);
					glAlphaFragmentOp2ATI(GL_MUL_ATI,   //bump up the opacity at the edges 
						GL_REG_0_ATI, GL_SATURATE_BIT_ATI,
						GL_REG_4_ATI, GL_NONE, GL_NONE,
						GL_REG_0_ATI, GL_NONE, GL_NONE);
				}
#endif
#if 0
				glColorFragmentOp2ATI(GL_MUL_ATI, //light attinuation
					GL_REG_0_ATI, GL_NONE, GL_NONE,
					GL_SECONDARY_INTERPOLATOR_ATI, GL_RED, GL_NONE,
					GL_REG_0_ATI, GL_NONE, GL_NONE);
				
				glColorFragmentOp3ATI(GL_LERP_ATI, // ambient contribution
					GL_REG_0_ATI, GL_NONE, GL_NONE,
					GL_CON_3_ATI, GL_ALPHA, GL_NONE,
					GL_REG_1_ATI, GL_NONE, GL_NONE,
					GL_REG_0_ATI, GL_NONE, GL_NONE);
#endif		
				
				glColorFragmentOp2ATI(GL_MUL_ATI,   //multiply color by its opacity for blending
					GL_REG_0_ATI, GL_NONE, GL_SATURATE_BIT_ATI, //5  -this might move to the transfer funciton!!
					GL_REG_0_ATI, GL_NONE, GL_NONE,
					GL_REG_0_ATI, GL_ALPHA, GL_NONE);
				//----- end math ------------------
			} glEndFragmentShaderATI();
		} glDisable(GL_FRAGMENT_SHADER_ATI);
		GlErr("TestRen::draw()", "build fragment shader 0");
	}




}

//======================================================================
//======================================================================
// create and update fragment shaders for shadows
void R8kVolRen3D::createFragShadow(unsigned int *&names)
{
	if(!names)
	{
		names = new unsigned int[20];
		for(int i=0; i<20; ++i) names[i] = -1;
	}
	
	int thirdAxis = 0;
	switch(gluvv.dmode)
	{ //third axis switch, to use or not to use???
		case GDM_VGH:
		case GDM_V1GH:
		case GDM_V2G:
		case GDM_V2GH:
		case GDM_V3:
		case GDM_V3G:
		case GDM_V4:
			thirdAxis = 1;
			break;
		default:
			thirdAxis = 0;
			break;
	}

	//=================================================================
	//================================================= shadows/ scatter
	
	if(names[0] == -1)
		names[0] = glGenFragmentShadersATI(1);
	if(names[1] == -1)
		names[1] = glGenFragmentShadersATI(1);
	
	for(int i=0; i<2; ++i)
	{
		glEnable(GL_FRAGMENT_SHADER_ATI);
		{                                       //shadow buffer shader
			glBindFragmentShaderATI(names[i]);
			glBeginFragmentShaderATI();
			{
				//----- routing instructions ------
				//grab the tex coords for tex 0
				glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);   //grab data
				//----- end routing instructions --
				
				//----- math ----------------------
				
				if(thirdAxis)
				{
					//swizzle 0.G to 3.R
					glColorFragmentOp1ATI(GL_MOV_ATI,           //swizzle tex coords
						GL_REG_2_ATI, GL_RED_BIT_ATI, GL_NONE,
						GL_REG_0_ATI, GL_GREEN, GL_NONE);
					//swizzle 0.A to 3.G
					glColorFragmentOp1ATI(GL_MOV_ATI, 
						GL_REG_2_ATI, GL_GREEN, GL_NONE,
						GL_REG_0_ATI, GL_ALPHA, GL_NONE);
				} 
				else 
				{
					//swizzle 0.G to 5.R
					glColorFragmentOp1ATI(GL_MOV_ATI, 
						GL_REG_0_ATI, GL_RED_BIT_ATI, GL_NONE,
						GL_REG_0_ATI, GL_RED, GL_NONE);
					//swizzle 0.A to 5.G
					glColorFragmentOp1ATI(GL_MOV_ATI, 
						GL_REG_0_ATI, GL_GREEN_BIT_ATI, GL_NONE,
						GL_REG_0_ATI, GL_ALPHA, GL_NONE);
				}
				//----- end math ------------------
				
				//----- routing instructions ------
				// V & G transfer function
				glSampleMapATI(GL_REG_1_ATI, GL_REG_0_ATI, GL_SWIZZLE_STR_ATI);
				// H & 4 transfer function
				if(thirdAxis)
				{
					glSampleMapATI(GL_REG_2_ATI, GL_REG_2_ATI, GL_SWIZZLE_STR_ATI);
				}
				//sample shadow map (for blending)
				if(i == 0)
				{
					glSampleMapATI(GL_REG_5_ATI, GL_TEXTURE4_ARB, GL_SWIZZLE_STR_ATI); //2nd samp
				}
				if(i == 1)
				{
					glSampleMapATI(GL_REG_5_ATI, GL_TEXTURE5_ARB, GL_SWIZZLE_STR_ATI); //2nd samp
				}
				//----- end routing instructions --
				
				//----- math ----------------------
				glColorFragmentOp2ATI(GL_MUL_ATI, //no op.
					GL_REG_0_ATI, GL_NONE, GL_SATURATE_BIT_ATI,
					GL_REG_1_ATI, GL_NONE, GL_NONE,
					GL_ONE,       GL_NONE, GL_NONE);
				
				if(thirdAxis)
				{
					glAlphaFragmentOp2ATI(GL_MUL_ATI, 
						GL_REG_0_ATI, GL_SATURATE_BIT_ATI,
						GL_REG_1_ATI, GL_NONE, GL_NONE,
						GL_REG_2_ATI, GL_NONE, GL_NONE);
				}
				else
				{
					glAlphaFragmentOp2ATI(GL_MUL_ATI, 
						GL_REG_0_ATI, GL_SATURATE_BIT_ATI,
						GL_REG_1_ATI, GL_NONE, GL_NONE,
						GL_ONE,       GL_NONE, GL_NONE);
				}


#if 0
#if 1
				glColorFragmentOp2ATI(GL_MUL_ATI, //mulitply color by alpha
					GL_REG_0_ATI, GL_NONE,  GL_SATURATE_BIT_ATI,
					GL_REG_1_ATI, GL_NONE,  GL_COMP_BIT_ATI,
					GL_REG_0_ATI, GL_ALPHA, GL_NONE);
#if 1
				glColorFragmentOp2ATI(GL_MUL_ATI, //mulitply color by alpha
					GL_REG_3_ATI, GL_NONE,  GL_NONE,
					GL_REG_1_ATI, GL_NONE,  GL_NONE,
					GL_REG_0_ATI, GL_ALPHA, GL_NONE);
				
				glColorFragmentOp2ATI(GL_ADD_ATI, //mulitply color by alpha
					GL_REG_0_ATI, GL_NONE,  GL_SATURATE_BIT_ATI,
					GL_REG_3_ATI, GL_NONE,  GL_NONE,
					GL_REG_0_ATI, GL_NONE, GL_NONE);
#endif
#endif

#if 0				
				glColorFragmentOp2ATI(GL_MUL_ATI, //mulitply color by alpha
					GL_REG_0_ATI, GL_NONE,  GL_SATURATE_BIT_ATI,
					GL_REG_1_ATI, GL_NONE,  GL_NONE,
					GL_REG_0_ATI, GL_ALPHA, GL_NONE);
#endif

#else
				if(i == 0)
				{
					glColorFragmentOp3ATI(GL_LERP_ATI, // blend
						GL_REG_0_ATI, GL_NONE,  GL_NONE,
						GL_REG_0_ATI, GL_ALPHA, GL_NONE,
						GL_REG_0_ATI, GL_NONE,  GL_NONE,
						GL_REG_5_ATI, GL_NONE,  GL_NONE);

					glAlphaFragmentOp3ATI(GL_MAD_ATI, 
						GL_REG_0_ATI, GL_SATURATE_BIT_ATI,
						GL_REG_0_ATI, GL_NONE, GL_COMP_BIT_ATI,
						GL_REG_5_ATI, GL_NONE, GL_NONE,
						GL_REG_0_ATI, GL_NONE, GL_NONE);
#if 0
					glColorFragmentOp1ATI(GL_MOV_ATI,           //swizzle tex coords
						GL_REG_0_ATI, GL_NONE, GL_NONE,
						GL_REG_5_ATI, GL_NONE, GL_NONE);
					glAlphaFragmentOp1ATI(GL_MOV_ATI,           //swizzle tex coords
						GL_REG_0_ATI, GL_NONE,
						GL_REG_5_ATI, GL_NONE, GL_NONE);
#endif			
				}
				if(i == 1)
				{
					glColorFragmentOp3ATI(GL_LERP_ATI, // blend
						GL_REG_0_ATI, GL_NONE,  GL_SATURATE_BIT_ATI,
						GL_REG_0_ATI, GL_ALPHA, GL_NONE,
						GL_REG_0_ATI, GL_NONE,  GL_NONE,
						GL_REG_5_ATI, GL_NONE,  GL_NONE);

					glAlphaFragmentOp3ATI(GL_MAD_ATI, 
						GL_REG_0_ATI, GL_SATURATE_BIT_ATI,
						GL_REG_0_ATI, GL_NONE, GL_COMP_BIT_ATI,
						GL_REG_5_ATI, GL_NONE, GL_NONE,
						GL_REG_0_ATI, GL_NONE, GL_NONE);
#if 0
					glColorFragmentOp1ATI(GL_MOV_ATI,           //swizzle tex coords
						GL_REG_0_ATI, GL_NONE, GL_SATURATE_BIT_ATI,
						GL_REG_1_ATI, GL_NONE, GL_NONE);
					glAlphaFragmentOp1ATI(GL_MOV_ATI,           //swizzle tex coords
						GL_REG_0_ATI, GL_SATURATE_BIT_ATI,
						GL_REG_1_ATI, GL_NONE, GL_NONE);
#endif
				}
#endif
				//----- end math ------------------
			} glEndFragmentShaderATI();
		} glDisable(GL_FRAGMENT_SHADER_ATI);
		GlErr("TestRen::draw()", "build fragment shader 9");
	} //end for

	//=================================================================
	//============================================================ clip
	
}

//======================================================================
//======================================================================
// create and update fragment shaders for clips
void R8kVolRen3D::createFragClip()
{
	int thirdAxis = 0;
	switch(gluvv.dmode)
	{ //third axis switch, to use or not to use???
		case GDM_VGH:
		case GDM_V1GH:
		case GDM_V2G:
		case GDM_V2GH:
		case GDM_V3:
		case GDM_V3G:
		case GDM_V4:
			thirdAxis = 1;
			break;
		default:
			thirdAxis = 0;
			break;
	}

	cpfsName = glGenFragmentShadersATI(1);
	
	glEnable(GL_FRAGMENT_SHADER_ATI);
	{                                        //clipping plane shader
		glBindFragmentShaderATI(cpfsName);
		glBeginFragmentShaderATI();
		{
			//----- routing instructions ------
			//grab the tex coords for tex 0
			glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);   //grab data
			//----- end routing instructions --
			
			//----- math ----------------------

			if(thirdAxis)
			{
				glColorFragmentOp2ATI(GL_MUL_ATI, //no op.
					GL_REG_0_ATI, GL_NONE, GL_SATURATE_BIT_ATI,
					GL_REG_0_ATI, GL_NONE, GL_NONE,
					GL_ONE,       GL_NONE, GL_NONE);
			} 
			else 
			{
				glColorFragmentOp1ATI(GL_MOV_ATI, //no op.
				  GL_REG_0_ATI, GL_GREEN_BIT_ATI, GL_SATURATE_BIT_ATI,
					GL_REG_0_ATI, GL_ALPHA, GL_NONE);
			}

			glAlphaFragmentOp2ATI(GL_MUL_ATI, 
					GL_REG_0_ATI, GL_SATURATE_BIT_ATI,
					GL_CON_0_ATI, GL_NONE, GL_NONE,
					GL_ONE,       GL_NONE, GL_NONE);

			glColorFragmentOp2ATI(GL_MUL_ATI, //no op.
					GL_REG_0_ATI, GL_NONE,  GL_SATURATE_BIT_ATI,
					GL_REG_0_ATI, GL_NONE,  GL_NONE,
					GL_REG_0_ATI, GL_ALPHA, GL_NONE);
			//----- end math ------------------			
		} glEndFragmentShaderATI();
	} glDisable(GL_FRAGMENT_SHADER_ATI);
	GlErr("TestRen::draw()", "build fragment shader clip");	
}
