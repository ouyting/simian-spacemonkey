//////////////////////////////////////////////////////////////////////
//                   ________    ____   ___ 
//                  |        \  /    | /  /
//                  +---+     \/     |/  /
//                  +--+|  |\    /|     < 
//                  |  ||  | \  / |  |\  \ 
//                  |      |  \/  |  | \  \ 
//                   \_____|      |__|  \__\
//                       Copyright  2001 
//                      Joe Michael Kniss
//                    <<<jmk@cs.utah.edu>>>
//               "All Your Base are Belong to Us"
//
//
// NV20VolRen.cpp: implementation of the NV20VolRen class.
//
//////////////////////////////////////////////////////////////////////

#include "NV20VolRen.h"
#include <iostream.h>
#include <GL/glExt.h>
#include "VectorMath.h"

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


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NV20VolRen::NV20VolRen()
{
	texNames2D = 0;
	sx = sy = sz = 0;

	if(gluvv.mv){
		vcoords[0][0] = vcoords[0][1] = vcoords[0][2] = 0;
		vcoords[1][0] = gluvv.mv->xfSize; vcoords[1][1] = vcoords[1][2] = 0;
		vcoords[2][0] = 0; vcoords[2][1] = gluvv.mv->yfSize; vcoords[2][2] = 0; 
		vcoords[3][0] = gluvv.mv->xfSize; vcoords[3][1] = gluvv.mv->yfSize; vcoords[3][2] = 0;
		vcoords[4][0] = vcoords[4][1] = 0; vcoords[4][2] = gluvv.mv->zfSize;
		vcoords[5][0] = gluvv.mv->xfSize; vcoords[5][1] = 0; vcoords[5][2] = gluvv.mv->zfSize;
		vcoords[6][0] = 0; vcoords[6][1] = gluvv.mv->yfSize; vcoords[6][2] = gluvv.mv->zfSize;
		vcoords[7][0] = gluvv.mv->xfSize; 
	    vcoords[7][1] = gluvv.mv->yfSize; vcoords[7][2] = gluvv.mv->zfSize;

		texts[0][0] = texts[1][0] = texts[2][0] = 0;
		texts[0][1] = texts[1][1] = texts[2][1] = 1;
		vexts[0][0] = vexts[1][0] = vexts[2][0] = 0;
		vexts[0][1] = gluvv.mv->xfSize;
		vexts[1][1] = gluvv.mv->yfSize;
		vexts[2][1] = gluvv.mv->zfSize;
	}
}

NV20VolRen::~NV20VolRen()
{

}

//================================================================= INIT
//======================================================================

void NV20VolRen::init()
{
	if(gluvv.mv){
		create2DTextures(gluvv.mv);
		create2DDepTex();
		createTempTex();
		
		//we should select pbuffer size based on the size of the largest slice extents
		if(!(pbuff = new PBuffer(512, 512, GLUT_SINGLE | GLUT_RGBA))){
			cerr << "ERROR: PBuffer intialization failed!" << endl;
		} else {
			pbuff->Initialize(true);
			pbuff->MakeCurrent();
			glClearColor( 0.0, 0.0, 0.0, 0.0 );			
			glMatrixMode( GL_PROJECTION );
			glOrtho(0,512,0,512,1,3);
			glMatrixMode(GL_MODELVIEW);
			gluLookAt(0,0,2,
				        0,0,0,
								0,1,0);
			MakeCurrent();
			cerr << "PBuffer intialization succeded" << endl;
		}
	}
}

//=============================================================== DRAW !
//======================================================================

void NV20VolRen::draw()
{
	if(!texNames2D) return;
	//interpSlices();
	//drawSlices();

	
	
	drawVolume();

}

//========================================================== Draw Volume
//======================================================================

void NV20VolRen::drawVolume()
{
	//double mv0[16];

	glPushMatrix();{
		//--------------- set up transformation -------------------------------
		glGetDoublev(GL_MODELVIEW_MATRIX, mv0);

		glTranslatef(gluvv.rinfo.trans[0], //translate
					 			 gluvv.rinfo.trans[1], 
								 gluvv.rinfo.trans[2]);
		glMultMatrixf(gluvv.rinfo.xform);  //rotate
		glTranslatef(-gluvv.mv->xfSize/2,  //center
								 -gluvv.mv->yfSize/2,
								 -gluvv.mv->zfSize/2);
	  
		GLdouble mv[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, mv);  //save modelview matrix
		//--------------- end set up transformation ----------------------------

		//----------------------------- get clip space values -----------------
		GLfloat trans[16] = { 1,0,0,0,
			                     0,1,0,0,
													 0,0,1,0,
													 gluvv.rinfo.trans[0], gluvv.rinfo.trans[1], gluvv.rinfo.trans[2],1};
		GLfloat shift[16] = { 1,0,0,0,
			                     0,1,0,0,
													 0,0,1,0,
													 -gluvv.mv->xfSize/2, -gluvv.mv->yfSize/2, -gluvv.mv->zfSize/2,1};
		GLfloat tmp1[16];
		matrixMult(tmp1, gluvv.rinfo.xform, shift);
		GLfloat volmv[16];  //volume - model view
		matrixMult(volmv, trans, tmp1);

		GLfloat clipt[16] = { 1,0,0,0,
			                     0,1,0,0,
													 0,0,1,0,
													 gluvv.clip.pos[0], gluvv.clip.pos[1], gluvv.clip.pos[2],1};
		GLfloat tmp2[16];
		matrixMult(tmp2, clipt, gluvv.clip.xform);
		GLfloat invcp[16];  // inverse clip
		inverseMatrix(invcp, tmp2);

		GLfloat vol2cp[16]; //volume space to clip space
		matrixMult(vol2cp, invcp, volmv);
	
		for(int i =0; i<8; ++i){
			translateV3W(ccoords[i], vol2cp, vcoords[i]);
		}
		//-------------------------- end get clip space values -----------------
	
		//-------------- Re-Scale Alpha values ---------------------------------
    if(gluvv.volren.scaleAlphas){
			if((lastSamp != gluvv.volren.sampleRate)||(gluvv.volren.loadTLUT)){  //see if the sample rate changed
				if((lastGoodSamp != gluvv.volren.goodSamp) || gluvv.volren.loadTLUT){
					copyScale(gluvv.volren.goodSamp * 1/gluvv.volren.gamma, gDeptex);
					lastGoodSamp = gluvv.volren.goodSamp;
				}
				if((lastInteSamp != gluvv.volren.interactSamp) || gluvv.volren.loadTLUT){
					copyScale(gluvv.volren.interactSamp * 1/gluvv.volren.gamma, iDeptex);
					lastInteSamp = gluvv.volren.interactSamp;
				}
				
				if(gluvv.volren.sampleRate == gluvv.volren.goodSamp){
					loadDepTex(gDeptex);
					lastSamp = gluvv.volren.goodSamp;
				}
				else if(gluvv.volren.sampleRate == gluvv.volren.interactSamp){
					loadDepTex(iDeptex);
					lastSamp = gluvv.volren.interactSamp;
				}
				gluvv.volren.loadTLUT = 0;
			}
		} else {  //just do gamma scale
			if(gluvv.volren.loadTLUT){
				copyScale(1/gluvv.volren.gamma, gDeptex);
				loadDepTex(gDeptex);
				gluvv.volren.loadTLUT = 0;
			}
		}
		//-------------- end Re-Scale Alpha values ---------------------------------

			//-------------- clipping plane setup -----------
			if(gluvv.clip.on && (!gluvv.clip.ortho)){
				glPushMatrix();{
					glLoadIdentity();
					glMultMatrixd(mv0);
					glTranslatef(gluvv.clip.pos[0],  //location of clipping plane
						gluvv.clip.pos[1],
						gluvv.clip.pos[2]);
					glMultMatrixf(gluvv.clip.xform); //rotation of clip plane
					GLdouble zup[4] = {0, 0, -1, 0}; // always in z direction
					glEnable(GL_CLIP_PLANE5);        //enable the gl clip plane
					glClipPlane(GL_CLIP_PLANE5, zup);
				} glPopMatrix();
			}
			
			texts[0][0] = texts[1][0] = texts[2][0] = 0.02;
			texts[0][1] = texts[1][1] = texts[2][1] = .98;
			vexts[0][0] = vexts[1][0] = vexts[2][0] = 0;
			vexts[0][1] = gluvv.mv->xfSize;
			vexts[1][1] = gluvv.mv->yfSize;
			vexts[2][1] = gluvv.mv->zfSize;
			
			if(gluvv.clip.on && gluvv.clip.ortho){
				float cp[3];
				cp[0] = gluvv.clip.vpos[0] > 0 ? 
					(gluvv.clip.vpos[0] < gluvv.mv->xfSize ? gluvv.clip.vpos[0] : gluvv.mv->xfSize) : 0;
				cp[1] = gluvv.clip.vpos[1] > 0 ? 
					(gluvv.clip.vpos[1] < gluvv.mv->yfSize ? gluvv.clip.vpos[1] : gluvv.mv->yfSize) : 0;
				cp[2] = gluvv.clip.vpos[2] > 0 ? 
					(gluvv.clip.vpos[2] < gluvv.mv->zfSize ? gluvv.clip.vpos[2] : gluvv.mv->zfSize) : 0;
				switch(gluvv.clip.oaxis){
				case VolRenAxisXPos:
					texts[0][1] = cp[0]/gluvv.mv->xfSize;
					vexts[0][1] = cp[0];
					break;
				case VolRenAxisXNeg:
					texts[0][0] = cp[0]/gluvv.mv->xfSize;
					vexts[0][0] = cp[0];
					break;
        case VolRenAxisYPos:
					texts[1][1] = cp[1]/gluvv.mv->yfSize;
					vexts[1][1] = cp[1];
					break;
        case VolRenAxisYNeg:
					texts[1][0] = cp[1]/gluvv.mv->yfSize;
					vexts[1][0] = cp[1];
					break;
        case VolRenAxisZPos:
					texts[2][1] = cp[2]/gluvv.mv->zfSize;
					vexts[2][1] = cp[2];
					break;
        case VolRenAxisZNeg:
					texts[2][0] = cp[2]/gluvv.mv->zfSize;
					vexts[2][0] = cp[2];
					break;
				}
			} 

			//-------------- end clipping plane setup -------

			
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


			//-------------- draw Clip plane slice ----------
			float vdir[3];
			subV3(vdir, gluvv.env.eye, gluvv.clip.pos);
			normalizeV3(vdir);
			normalizeV3(gluvv.clip.dir);
			float dv = dotV3(vdir,gluvv.clip.dir);
			drawClip(dv);			
			//--------------  end draw Clip plane slice -----


			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			
		//-------------- Texture Shader setup --------------------------------------
		glEnable(GL_TEXTURE_SHADER_NV); {
			GlErr("nv20volren", "enable shader");
			
			glActiveTexture(GL_TEXTURE0_ARB); { //this is for the slice
				glEnable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NONE);
				glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
			}
			glActiveTexture(GL_TEXTURE1_ARB); { //this is for the transfer function
				glEnable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DEPENDENT_GB_TEXTURE_2D_NV);
				glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
				glBindTexture(GL_TEXTURE_2D, deptexName);
			}
			glActiveTexture(GL_TEXTURE2_ARB); { // for shading or clipping (not both)
				glDisable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NONE);
				if((gluvv.shade == gluvvShadeDiff) || (gluvv.shade == gluvvShadeDSpec)){
					glEnable(GL_TEXTURE_2D);
					glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
				  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				}
				else if(gluvv.clip.on && (!gluvv.clip.ortho))
					glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_CULL_FRAGMENT_NV);
			}
			glActiveTexture(GL_TEXTURE3_ARB); { //not used
				glDisable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NONE);
				if((gluvv.shade == gluvvShadeDiff) || (gluvv.shade == gluvvShadeDSpec)){
					glEnable(GL_TEXTURE_2D);
					glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
				  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				}
			}
			//-------------- Texture Shader setup --------------------------------------
			
			//-------------- Register Combiner Setup -----------------------------------
			  setupShading();
			//-------------- end Register Combiner Setup -------------------------------
			
			//-------------- Render the Volume -----------------------------------------
			VolRenMajorAxis ma = getMajorAxis(mv);

			glColor4f(1,1,1,gluvv.tf.slider1hi);
			switch(ma){
			case VolRenAxisXPos:
				renXPos();
				break;
			case VolRenAxisYPos:
				renYPos();
				break;
			case VolRenAxisZPos:
				renZPos();
				break;
			case VolRenAxisXNeg:
				renXNeg();
				break;
			case VolRenAxisYNeg:
				renYNeg();
				break;
			case VolRenAxisZNeg:
				renZNeg();
				break;
			} //end switch major axis
			//-------------- end Render the Volume --------------------------------------
			
		} glDisable(GL_TEXTURE_SHADER_NV);
	
		//if((gluvv.shade == gluvvShadeDiff) || (gluvv.shade == gluvvShadeDSpec)){
		glDisable(GL_REGISTER_COMBINERS_NV);
		//}

		//---------------- Reset the texture units ------------------------------------
		glActiveTexture(GL_TEXTURE3_ARB); { //not used
				glDisable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NONE);
		}
		glActiveTexture(GL_TEXTURE2_ARB); { //not used
				glDisable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NONE);
		}
		glActiveTexture(GL_TEXTURE1_ARB); { //not used
				glDisable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NONE);
		}
		glActiveTexture(GL_TEXTURE0_ARB); { //not used
				glDisable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NONE);
		}
		//---------------- end Reset the texture units --------------------------------

		//-------------- draw Clip plane slice ----------
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		drawClip(-dv);
		glDisable(GL_BLEND);
		//-------------- end draw Clip plane slice ------

	} glPopMatrix();

	glDisable(GL_CLIP_PLANE5);

}

//=================================================== Render along axies
//======================================================================

void NV20VolRen::renXPos()
{

	float dist = vexts[0][1] - vexts[0][0];

	int tsamps = gluvv.volren.sampleRate * 100 * gluvv.mv->xfSize;
	int samps = tsamps * (texts[0][1] - texts[0][0]);
	
	//float inc = gluvv.mv->zfSize/(float)samps;
	float inc = dist/(float)samps;
	
	float idxi = (1.0/(float)samps) * sx * (dist/gluvv.mv->xfSize);
	int   istart = texts[0][0] * sx;
	
	float w[4];

	for(int i=samps-2; i>=1; --i){
		float ds = 1.0 - ((istart +i*idxi) - (int)(istart + i*idxi));
		w[0] = ds; w[1] = ds; w[2] = ds; w[3] = ds;
		interpSlice(temptexNames[2],sy,sz,
					      texNames2D[sz+sy+(int)(istart + i*idxi)],
								texNames2D[sz+sy+MIN((int)(istart + i*idxi)+1, sx-1)], w);
		glActiveTexture(GL_TEXTURE0_ARB); glBindTexture(GL_TEXTURE_2D, temptexNames[2]);
		glActiveTexture(GL_TEXTURE2_ARB); glBindTexture(GL_TEXTURE_2D, shadeNames[sz+sy+(int)(istart + i*idxi)]);
		glActiveTexture(GL_TEXTURE3_ARB); glBindTexture(GL_TEXTURE_2D, shadeNames[sz+sy+MIN((int)(istart + i*idxi)+1, sx-1)]);		

		w[3] = .3;
		glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, w); //set w
		
		glBegin(GL_QUADS);{
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[1][0],texts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[1][0],texts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[1][0],texts[2][0]);
			glVertex3f(vexts[0][0] + i*inc, vexts[1][0], vexts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[1][1],texts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[1][1],texts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[1][1],texts[2][0]);
			glVertex3f(vexts[0][0] + i*inc, vexts[1][1], vexts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[1][1],texts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[1][1],texts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[1][1],texts[2][1]);
			glVertex3f(vexts[0][0] + i*inc, vexts[1][1], vexts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[1][0],texts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[1][0],texts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[1][0],texts[2][1]);
			glVertex3f(vexts[0][0] + i*inc, vexts[1][0], vexts[2][1]);
		} glEnd();
	}
}

void NV20VolRen::renXNeg()
{	
	
	float dist = vexts[0][1] - vexts[0][0];

	int tsamps = gluvv.volren.sampleRate * 100 * gluvv.mv->xfSize;
	int samps = tsamps * (texts[0][1] - texts[0][0]);
	
	//float inc = gluvv.mv->zfSize/(float)samps;
	float inc = dist/(float)samps;
	
	float idxi = (1.0/(float)samps) * sx * (dist/gluvv.mv->xfSize);
	int   istart = texts[0][0] * sx;
	
	float w[4];


	for(int i=1; i<samps-1; ++i){
		float ds = 1.0 - ((istart +i*idxi) - (int)(istart + i*idxi));
		w[0] = ds; w[1] = ds; w[2] = ds; w[3] = ds;
		interpSlice(temptexNames[2],sy,sz,
					      texNames2D[sz+sy+(int)(istart + i*idxi)],
								texNames2D[sz+sy+MIN((int)(istart + i*idxi)+1, sx-1)], w);
		glActiveTexture(GL_TEXTURE0_ARB); glBindTexture(GL_TEXTURE_2D, temptexNames[2]);
		glActiveTexture(GL_TEXTURE2_ARB); glBindTexture(GL_TEXTURE_2D, shadeNames[sz+sy+(int)(istart + i*idxi)]);
		glActiveTexture(GL_TEXTURE3_ARB); glBindTexture(GL_TEXTURE_2D, shadeNames[sz+sy+MIN((int)(istart + i*idxi)+1, sx-1)]);		

		
		w[3] = .3;
		glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, w); //set w
		
		glBegin(GL_QUADS);{
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[1][0],texts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[1][0],texts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[1][0],texts[2][0]);
			glVertex3f(vexts[0][0] + i*inc, vexts[1][0], vexts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[1][1],texts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[1][1],texts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[1][1],texts[2][0]);
			glVertex3f(vexts[0][0] + i*inc, vexts[1][1], vexts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[1][1],texts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[1][1],texts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[1][1],texts[2][1]);
			glVertex3f(vexts[0][0] + i*inc, vexts[1][1], vexts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[1][0],texts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[1][0],texts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[1][0],texts[2][1]);
			glVertex3f(vexts[0][0] + i*inc, vexts[1][0], vexts[2][1]);
		} glEnd();

	}

}

void NV20VolRen::renYPos()
{
	float dist = vexts[1][1] - vexts[1][0];

	int tsamps = gluvv.volren.sampleRate * 100 * gluvv.mv->yfSize;
	int samps = tsamps * (texts[1][1] - texts[1][0]);
	
	//float inc = gluvv.mv->zfSize/(float)samps;
	float inc = dist/(float)samps;
	
	float idxi = (1.0/(float)samps) * sy * (dist/gluvv.mv->yfSize);
	int   istart = texts[1][0] * sy;
	float w[4];


	for(int i=samps-2; i>=1; --i){
		float ds = 1.0 - ((istart + i*idxi) - (int)(istart + i*idxi));
		w[0] = ds; w[1] = ds; w[2] = ds; w[3] = ds;
		interpSlice(temptexNames[1],sx,sz,
			          texNames2D[sz+(int)(istart + i*idxi)],
								texNames2D[sz+MIN((int)(istart + i*idxi)+1, sy-1)], w);
		w[3] = .3;
		glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, w); //set w

		glActiveTexture(GL_TEXTURE0_ARB); glBindTexture(GL_TEXTURE_2D, temptexNames[1]);
		glActiveTexture(GL_TEXTURE2_ARB); glBindTexture(GL_TEXTURE_2D, shadeNames[sz+(int)(istart + i*idxi)]);
		glActiveTexture(GL_TEXTURE3_ARB); glBindTexture(GL_TEXTURE_2D, shadeNames[sz+MIN((int)(istart + i*idxi)+1, sy-1)]);		
		
		glBegin(GL_QUADS);{
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[0][0],texts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[0][0],texts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[0][0],texts[2][0]);
			glVertex3f(vexts[0][0],vexts[1][0]+i*inc, vexts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[0][1],texts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[0][1],texts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[0][1],texts[2][0]);
			glVertex3f(vexts[0][1],vexts[1][0]+i*inc, vexts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[0][1],texts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[0][1],texts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[0][1],texts[2][1]);
			glVertex3f(vexts[0][1],vexts[1][0]+i*inc, vexts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[0][0],texts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[0][0],texts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[0][0],texts[2][1]);
			glVertex3f(vexts[0][0],vexts[1][0]+i*inc, vexts[2][1]);
		} glEnd();

	}
}

void NV20VolRen::renYNeg()
{

	float dist = vexts[1][1] - vexts[1][0];

	int tsamps = gluvv.volren.sampleRate * 100 * gluvv.mv->yfSize;
	int samps = tsamps * (texts[1][1] - texts[1][0]);
	
	//float inc = gluvv.mv->zfSize/(float)samps;
	float inc = dist/(float)samps;
	
	float idxi = (1.0/(float)samps) * sy * (dist/gluvv.mv->yfSize);
	int   istart = texts[1][0] * sy;
	float w[4];


	for(int i=1; i<samps-2; ++i){
		float ds = 1.0 - ((istart + i*idxi) - (int)(istart + i*idxi));
		w[0] = ds; w[1] = ds; w[2] = ds; w[3] = ds;
		interpSlice(temptexNames[1],sx,sz,
			          texNames2D[sz+(int)(istart + i*idxi)],
								texNames2D[sz+MIN((int)(istart + i*idxi)+1, sy-1)], w);
		
		w[3] = .3;
		glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, w); //set w

		glActiveTexture(GL_TEXTURE0_ARB); glBindTexture(GL_TEXTURE_2D, temptexNames[1]);
		glActiveTexture(GL_TEXTURE2_ARB); glBindTexture(GL_TEXTURE_2D, shadeNames[sz+(int)(istart + i*idxi)]);
		glActiveTexture(GL_TEXTURE3_ARB); glBindTexture(GL_TEXTURE_2D, shadeNames[sz+MIN((int)(istart + i*idxi)+1, sy-1)]);		
		
		glBegin(GL_QUADS);{
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[0][0],texts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[0][0],texts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[0][0],texts[2][0]);
			glVertex3f(vexts[0][0],vexts[1][0]+i*inc, vexts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[0][1],texts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[0][1],texts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[0][1],texts[2][0]);
			glVertex3f(vexts[0][1],vexts[1][0]+i*inc, vexts[2][0]);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[0][1],texts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[0][1],texts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[0][1],texts[2][1]);
			glVertex3f(vexts[0][1],vexts[1][0]+i*inc, vexts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[0][0],texts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[0][0],texts[2][1]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[0][0],texts[2][1]);
			glVertex3f(vexts[0][0],vexts[1][0]+i*inc, vexts[2][1]);
		} glEnd();

	}
}

void NV20VolRen::renZNeg()
{

	float dist = vexts[2][1] - vexts[2][0];

	int tsamps = gluvv.volren.sampleRate * 100 * gluvv.mv->zfSize;
	int samps = tsamps * (texts[2][1] - texts[2][0]);
	
	//float inc = gluvv.mv->zfSize/(float)samps;
	float inc = dist/(float)samps;
	
	float idxi = (1.0/(float)samps) * sz * (dist/gluvv.mv->zfSize);
	int   istart = texts[2][0] * sz;
	float w[4];


	for(int i=samps-1; i>=0; --i){
		float ds = 1.0 - ((istart + i*idxi) - (floor((istart + i*idxi))));
		w[0] = ds; w[1] = ds; w[2] = ds; w[3] = ds;
		interpSlice(temptexNames[0],sx,sy,
			          texNames2D[(int)(istart + i*idxi)],
								texNames2D[MIN((int)(istart + i*idxi)+1, sz-1)], w);
		
		w[3] = .3;
		glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, w); //set w

		glActiveTexture(GL_TEXTURE0_ARB); glBindTexture(GL_TEXTURE_2D, temptexNames[0]);
		glActiveTexture(GL_TEXTURE2_ARB); glBindTexture(GL_TEXTURE_2D, shadeNames[(int)(istart + i*idxi)]);
		glActiveTexture(GL_TEXTURE3_ARB); glBindTexture(GL_TEXTURE_2D, shadeNames[MIN((int)(istart + i*idxi)+1, sz-1)]);		
		
		glBegin(GL_QUADS);{

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[0][0],texts[1][0]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[0][0],texts[1][0]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[0][0],texts[1][0]);
			glVertex3f(vexts[0][0],vexts[1][0],vexts[2][0] + i*inc);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[0][1],texts[1][0]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[0][1],texts[1][0]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[0][1],texts[1][0]);
			glVertex3f(vexts[0][1],vexts[1][0],vexts[2][0] + i*inc);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[0][1],texts[1][1]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[0][1],texts[1][1]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[0][1],texts[1][1]);
			glVertex3f(vexts[0][1],vexts[1][1],vexts[2][0] + i*inc);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[0][0],texts[1][1]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[0][0],texts[1][1]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[0][0],texts[1][1]);
			glVertex3f(vexts[0][0],vexts[1][1],vexts[2][0] + i*inc);
		} glEnd();

	}

}

void NV20VolRen::renZPos()
{	
	float dist = vexts[2][1] - vexts[2][0];

	int tsamps = gluvv.volren.sampleRate * 100 * gluvv.mv->zfSize;
	int samps = tsamps * (texts[2][1] - texts[2][0]);
	
	//float inc = gluvv.mv->zfSize/(float)samps;
	float inc = dist/(float)samps;
	
	float idxi = (1.0/(float)samps) * sz * (dist/gluvv.mv->zfSize);
	int   istart = texts[2][0] * sz;
	float w[4];
	

	for(int i=1; i<samps-1; ++i){
		float ds = 1.0 - ((istart + i*idxi) - (floor((istart + i*idxi))));
		w[0] = ds; w[1] = ds; w[2] = ds; w[3] = ds;
		interpSlice(temptexNames[0],sx,sy,
			          texNames2D[(int)(istart + i*idxi)],
								texNames2D[MIN((int)(istart + i*idxi)+1, sz-1)], w);
		w[3] = .3;
		glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, w); //set w
		
		glActiveTexture(GL_TEXTURE0_ARB); glBindTexture(GL_TEXTURE_2D, temptexNames[0]);
		glActiveTexture(GL_TEXTURE2_ARB); glBindTexture(GL_TEXTURE_2D, shadeNames[(int)(istart + i*idxi)]);
		glActiveTexture(GL_TEXTURE3_ARB); glBindTexture(GL_TEXTURE_2D, shadeNames[MIN((int)(istart + i*idxi)+1, sz-1)]);

		
		glBegin(GL_QUADS);{
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[0][0],texts[1][0]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[0][0],texts[1][0]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[0][0],texts[1][0]);
			glVertex3f(vexts[0][0],vexts[1][0],vexts[2][0] + i*inc);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[0][1],texts[1][0]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[0][1],texts[1][0]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[0][1],texts[1][0]);
			glVertex3f(vexts[0][1],vexts[1][0],vexts[2][0] + i*inc);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[0][1],texts[1][1]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[0][1],texts[1][1]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[0][1],texts[1][1]);
			glVertex3f(vexts[0][1],vexts[1][1],vexts[2][0] + i*inc);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,texts[0][0],texts[1][1]);
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,texts[0][0],texts[1][1]);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,texts[0][0],texts[1][1]);
			glVertex3f(vexts[0][0],vexts[1][1],vexts[2][0] + i*inc);
		} glEnd();

	}

}

//======================================================== setup shading
//======================================================================

void NV20VolRen::setupShading()
{
	if((gluvv.shade == gluvvShadeDiff) || (gluvv.shade == gluvvShadeDSpec)){

		float vdir[3];
		subV3(vdir, gluvv.env.eye, gluvv.env.at);
		normalizeV3(vdir);
		float ltdir[3];
		subV3(ltdir, gluvv.light.pos, gluvv.env.at);
		normalizeV3(ltdir);
		float ltoe[3];
		subV3(ltoe,vdir,ltdir);
		scaleV3(.5, ltoe);
		float half[4];
		addV3(half,ltdir, ltoe);
		half[3] = 1;
		float vhalf[4];
		float invx[16];
		inverseMatrix(invx, gluvv.rinfo.xform);
		translateV3(vhalf,invx,half);
		negateV3(vhalf);
		normalizeV3(vhalf);
		vhalf[0] = vhalf[0]/2 + .5;
		vhalf[1] = vhalf[1]/2 + .5;
		vhalf[2] = vhalf[2]/2 + .5;
		glFogfv(GL_FOG_COLOR, vhalf);
		
		float lpos[4];
		translateV3W(lpos, invx, ltdir); 
		negateV3(lpos);
		normalizeV3(ltdir);
		lpos[0] = lpos[0]/2 + .5;
		lpos[1] = lpos[1]/2 + .5;
		lpos[2] = lpos[2]/2 + .5;
		lpos[3] = gluvv.tf.slider1lo;
		glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, lpos); //set light direction
		
		glEnable(GL_REGISTER_COMBINERS_NV);
		
		glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 8);

		//---------------- set up alpha part ------------------
		if(gluvv.mv1){
#if 1
			glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
			glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, 
				GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
			GlErr("nv20volren","gcom0.4a");
			
			glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
			glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, 
				GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
			GlErr("nv20volren","gcom1.4a");
			
			glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, 
				GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
			GlErr("nv20volren","gcom2.4a");
			
			glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, 
				GL_SPARE1_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
			GlErr("nv20volren","gcom3.4a");
			
			
			glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerOutputNV(GL_COMBINER4_NV, GL_ALPHA, 
				GL_TEXTURE1_ARB, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
			GlErr("nv20volren","gcom4.4a");
#endif
		}

		//------------------------- set up RGB part -------------------------------------------------
		//compute tex1 * w + tex2 * (1-w), ie interp shade textures
		glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_INVERT_NV, GL_RGB);
		glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, 
			GL_DISCARD_NV, GL_DISCARD_NV, GL_PRIMARY_COLOR_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
		GlErr("nv20volren","gcom0.4");
		
		//do dot products for 2-sided lighting
		glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_PRIMARY_COLOR_NV, GL_EXPAND_NEGATE_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
		glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, 
			GL_SPARE1_NV, GL_SPARE0_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_TRUE, GL_FALSE);
		GlErr("nv20volren","gcom1.4");
		
		//compute diffuse illumination times color
		glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, 
			GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
		GlErr("nv20volren","gcom2.4");
		
		//compute specular illumination & ambiant contribution
		glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_FOG, GL_EXPAND_NORMAL_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
		glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, 
			GL_TEXTURE2_ARB, GL_TEXTURE3_ARB, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
		GlErr("nv20volren","gcom3.4");
		
		//diffuse + ambiant
		glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
		glCombinerOutputNV(GL_COMBINER4_NV, GL_RGB, 
			GL_DISCARD_NV, GL_DISCARD_NV, GL_TEXTURE3_ARB, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
		GlErr("nv20volren","gcom4.4");
		
		glCombinerInputNV(GL_COMBINER5_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER5_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER5_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER5_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerOutputNV(GL_COMBINER5_NV, GL_RGB, 
			GL_TEXTURE2_ARB, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
		GlErr("nv20volren","gcom5.4");
		
		glCombinerInputNV(GL_COMBINER6_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER6_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER6_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER6_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerOutputNV(GL_COMBINER6_NV, GL_RGB, 
			GL_TEXTURE2_ARB, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
		GlErr("nv20volren","gcom6.4");
		
		glCombinerInputNV(GL_COMBINER7_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER7_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER7_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerInputNV(GL_COMBINER7_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glCombinerOutputNV(GL_COMBINER7_NV, GL_RGB, 
			GL_TEXTURE2_ARB, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
		GlErr("nv20volren","gcom7.4");
		
		
		glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_E_TIMES_F_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_E_TIMES_F_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glFinalCombinerInputNV(GL_VARIABLE_E_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
		glFinalCombinerInputNV(GL_VARIABLE_F_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);	
		glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
		GlErr("nv20volren","fcom.0");
	}
	else { //no shading!
#if 1
		if(gluvv.mv1){
			glEnable(GL_REGISTER_COMBINERS_NV);
			
			float emph[4];
			emph[3] = gluvv.tf.slider1lo;
			glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, emph); 
			
			glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 5);
			
			glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
			glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, 
				GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
			GlErr("nv20volren","gcom0.4a");
			
			glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
			glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, 
				GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
			GlErr("nv20volren","gcom1.4a");
			
			glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, 
				GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
			GlErr("nv20volren","gcom2.4a");

			glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, 
				GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
			GlErr("nv20volren","gcom3.4a");
					
			glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			glCombinerOutputNV(GL_COMBINER4_NV, GL_ALPHA, 
				GL_TEXTURE1_ARB, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
			GlErr("nv20volren","gcom4.4a");
			
			glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
			glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
			glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
			glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
			glFinalCombinerInputNV(GL_VARIABLE_E_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
			glFinalCombinerInputNV(GL_VARIABLE_F_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);	
			glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
			GlErr("nv20volren","fcom.0");
		}
#endif

	}
}


//============================================================ draw Clip
//======================================================================
void NV20VolRen::drawClip(float dv)
{
	if((!gluvv.clip.on)||(!gluvv.clip.ortho)) return;
	float *c[4];
	c[0] = gluvv.clip.corners[0];
	c[1] = gluvv.clip.corners[1];
	c[2] = gluvv.clip.corners[2];
	c[3] = gluvv.clip.corners[3];
	c[0][0] = CLAMP_ARB(0,c[0][0],gluvv.mv->xfSize);
	c[1][0] = CLAMP_ARB(0,c[1][0],gluvv.mv->xfSize);
	c[2][0] = CLAMP_ARB(0,c[2][0],gluvv.mv->xfSize);
	c[3][0] = CLAMP_ARB(0,c[3][0],gluvv.mv->xfSize);
	c[0][1] = CLAMP_ARB(0,c[0][1],gluvv.mv->yfSize);
	c[1][1] = CLAMP_ARB(0,c[1][1],gluvv.mv->yfSize);
	c[2][1] = CLAMP_ARB(0,c[2][1],gluvv.mv->yfSize);
	c[3][1] = CLAMP_ARB(0,c[3][1],gluvv.mv->yfSize);
	c[0][2] = CLAMP_ARB(0,c[0][2],gluvv.mv->zfSize);
	c[1][2] = CLAMP_ARB(0,c[1][2],gluvv.mv->zfSize);
	c[2][2] = CLAMP_ARB(0,c[2][2],gluvv.mv->zfSize);
	c[3][2] = CLAMP_ARB(0,c[3][2],gluvv.mv->zfSize);

	glEnable(GL_REGISTER_COMBINERS_NV);
	glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);

	// I am just using these to replace the texture's alpha with a different one
	float alpha[4] = {1,1,1,gluvv.clip.alpha};
	glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, alpha); //set W

	glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	GlErr("nv20volren","fcom1");
	glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	GlErr("nv20volren","fcom2");
	glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	GlErr("nv20volren","fcom3");
	glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	GlErr("nv20volren","fcom0");
	
	glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);


	switch(gluvv.clip.oaxis){
	case VolRenAxisXPos:
		if((dv < 0)&&(c[0][0] > 0)&&(c[0][0]<gluvv.mv->xfSize)){
			float xw[4];
			xw[0] = xw[1] = xw[2] = xw[3] = 
				1-(gluvv.clip.vpos[0]/gluvv.mv->xfSize*sx - 
				(int)(gluvv.clip.vpos[0]/gluvv.mv->xfSize*sx));
			interpSlice(temptexNames[2],sy,sz,
				texNames2D[sz+sy+(int)(gluvv.clip.vpos[0]/gluvv.mv->xfSize*sx)],
				texNames2D[sz+sy+MIN((int)(gluvv.clip.vpos[0]/gluvv.mv->xfSize*sx)+1, sx-1)], 
				xw);
			glActiveTexture(GL_TEXTURE0_ARB); { //this is for the slice
				glEnable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, temptexNames[2]);
			}
			
			glColor4f(1,1,1,gluvv.clip.alpha);
			glBegin(GL_QUADS);{
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[0][1]/gluvv.mv->yfSize,c[0][2]/gluvv.mv->zfSize);
				glVertex3f(c[0][0], c[0][1], c[0][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[1][1]/gluvv.mv->yfSize,c[1][2]/gluvv.mv->zfSize);
				glVertex3f(c[1][0], c[1][1], c[1][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[2][1]/gluvv.mv->yfSize,c[2][2]/gluvv.mv->zfSize);
				glVertex3f(c[2][0], c[2][1], c[2][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[3][1]/gluvv.mv->yfSize,c[3][2]/gluvv.mv->zfSize);
				glVertex3f(c[3][0], c[3][1], c[3][2]);			
			} glEnd();
		}
		break;
	case VolRenAxisXNeg:
		if(dv > 0){
			float xw[4];
			xw[0] = xw[1] = xw[2] = xw[3] = 
				1-(gluvv.clip.vpos[0]/gluvv.mv->xfSize*sx - 
				(int)(gluvv.clip.vpos[0]/gluvv.mv->xfSize*sx));
			interpSlice(temptexNames[2],sy,sz,
				texNames2D[sz+sy+(int)(gluvv.clip.vpos[0]/gluvv.mv->xfSize*sx)],
				texNames2D[sz+sy+MIN((int)(gluvv.clip.vpos[0]/gluvv.mv->xfSize*sx)+1, sx-1)], 
				xw);
			glActiveTexture(GL_TEXTURE0_ARB); { //this is for the slice
				glEnable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, temptexNames[2]);
			}

			glColor4f(1,1,1,gluvv.clip.alpha);
			glBegin(GL_QUADS);{
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[0][1]/gluvv.mv->yfSize,c[0][2]/gluvv.mv->zfSize);
				glVertex3f(c[0][0], c[0][1], c[0][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[1][1]/gluvv.mv->yfSize,c[1][2]/gluvv.mv->zfSize);
				glVertex3f(c[1][0], c[1][1], c[1][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[2][1]/gluvv.mv->yfSize,c[2][2]/gluvv.mv->zfSize);
				glVertex3f(c[2][0], c[2][1], c[2][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[3][1]/gluvv.mv->yfSize,c[3][2]/gluvv.mv->zfSize);
				glVertex3f(c[3][0], c[3][1], c[3][2]);			
			} glEnd();
		}
		break;
	case VolRenAxisYPos:
		if((dv > 0)&&(c[0][1] > 0)&&(c[0][1]<gluvv.mv->yfSize)){
			float yw[4];
			yw[0] = yw[1] = yw[2] = yw[3] = 
				1-(gluvv.clip.vpos[1]/gluvv.mv->yfSize*sy - 
				(int)(gluvv.clip.vpos[1]/gluvv.mv->yfSize*sy));
			interpSlice(temptexNames[1],sx,sz,
				texNames2D[sz+(int)(gluvv.clip.vpos[1]/gluvv.mv->yfSize*sy)],
				texNames2D[sz+MIN((int)(gluvv.clip.vpos[1]/gluvv.mv->yfSize*sy)+1, sy-1)], 
				yw);
			glActiveTexture(GL_TEXTURE0_ARB); { //this is for the slice
				glEnable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, temptexNames[1]);
			}
			
			glColor4f(1,1,1,gluvv.clip.alpha);
			glBegin(GL_QUADS);{
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[0][0]/gluvv.mv->xfSize,c[0][2]/gluvv.mv->zfSize);
				glVertex3f(c[0][0], c[0][1], c[0][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[1][0]/gluvv.mv->xfSize,c[1][2]/gluvv.mv->zfSize);
				glVertex3f(c[1][0], c[1][1], c[1][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[2][0]/gluvv.mv->xfSize,c[2][2]/gluvv.mv->zfSize);
				glVertex3f(c[2][0], c[2][1], c[2][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[3][0]/gluvv.mv->xfSize,c[3][2]/gluvv.mv->zfSize);
				glVertex3f(c[3][0], c[3][1], c[3][2]);			
			} glEnd();
		}
		break;
	case VolRenAxisYNeg:
		if((dv < 0)&&(c[0][1] > 0)&&(c[0][1]<gluvv.mv->yfSize)){
			float yw[4];
			yw[0] = yw[1] = yw[2] = yw[3] = 
				1-(gluvv.clip.vpos[1]/gluvv.mv->yfSize*sy - 
				(int)(gluvv.clip.vpos[1]/gluvv.mv->yfSize*sy));
			interpSlice(temptexNames[1],sx,sz,
				texNames2D[sz+(int)(gluvv.clip.vpos[1]/gluvv.mv->yfSize*sy)],
				texNames2D[sz+MIN((int)(gluvv.clip.vpos[1]/gluvv.mv->yfSize*sy)+1, sy-1)], 
				yw);
			glActiveTexture(GL_TEXTURE0_ARB); { //this is for the slice
				glEnable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, temptexNames[1]);
			}
			
			glColor4f(1,1,1,gluvv.clip.alpha);
			glBegin(GL_QUADS);{
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[0][0]/gluvv.mv->xfSize,c[0][2]/gluvv.mv->zfSize);
				glVertex3f(c[0][0], c[0][1], c[0][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[1][0]/gluvv.mv->xfSize,c[1][2]/gluvv.mv->zfSize);
				glVertex3f(c[1][0], c[1][1], c[1][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[2][0]/gluvv.mv->xfSize,c[2][2]/gluvv.mv->zfSize);
				glVertex3f(c[2][0], c[2][1], c[2][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[3][0]/gluvv.mv->xfSize,c[3][2]/gluvv.mv->zfSize);
				glVertex3f(c[3][0], c[3][1], c[3][2]);			
			} glEnd();
		}
				break;
	case VolRenAxisZPos:
		if((dv < 0)&&(c[0][2] > 0)&&(c[0][2]<gluvv.mv->zfSize)){
			float zw[4];
			zw[0] = zw[1] = zw[2] = zw[3] = 
				1-(gluvv.clip.vpos[2]/gluvv.mv->zfSize*sz - 
				(int)(gluvv.clip.vpos[2]/gluvv.mv->zfSize*sz));
			interpSlice(temptexNames[0],sx,sz,
				texNames2D[(int)(gluvv.clip.vpos[2]/gluvv.mv->zfSize*sz)],
				texNames2D[MIN((int)(gluvv.clip.vpos[2]/gluvv.mv->zfSize*sz)+1, sz-1)], 
				zw);
			glActiveTexture(GL_TEXTURE0_ARB); { //this is for the slice
				glEnable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, temptexNames[0]);
			}
			
			glColor4f(1,1,1,gluvv.clip.alpha);
			glBegin(GL_QUADS);{
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[0][0]/gluvv.mv->xfSize,c[0][1]/gluvv.mv->yfSize);
				glVertex3f(c[0][0], c[0][1], c[0][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[1][0]/gluvv.mv->xfSize,c[1][1]/gluvv.mv->yfSize);
				glVertex3f(c[1][0], c[1][1], c[1][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[2][0]/gluvv.mv->xfSize,c[2][1]/gluvv.mv->yfSize);
				glVertex3f(c[2][0], c[2][1], c[2][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[3][0]/gluvv.mv->xfSize,c[3][1]/gluvv.mv->yfSize);
				glVertex3f(c[3][0], c[3][1], c[3][2]);			
			} glEnd();
		}
		break;
	case VolRenAxisZNeg:
		if((dv > 0)&&(c[0][2] > 0)&&(c[0][2]<gluvv.mv->zfSize)){
			float zw[4];
			zw[0] = zw[1] = zw[2] = zw[3] = 
				1-(gluvv.clip.vpos[2]/gluvv.mv->zfSize*sz - 
				(int)(gluvv.clip.vpos[2]/gluvv.mv->zfSize*sz));
			interpSlice(temptexNames[0],sx,sz,
				texNames2D[(int)(gluvv.clip.vpos[2]/gluvv.mv->zfSize*sz)],
				texNames2D[MIN((int)(gluvv.clip.vpos[2]/gluvv.mv->zfSize*sz)+1, sz-1)], 
				zw);
			glActiveTexture(GL_TEXTURE0_ARB); { //this is for the slice
				glEnable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, temptexNames[0]);
			}
			
			glColor4f(1,1,1,gluvv.clip.alpha);
			glBegin(GL_QUADS);{
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[0][0]/gluvv.mv->xfSize,c[0][1]/gluvv.mv->yfSize);
				glVertex3f(c[0][0], c[0][1], c[0][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[1][0]/gluvv.mv->xfSize,c[1][1]/gluvv.mv->yfSize);
				glVertex3f(c[1][0], c[1][1], c[1][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[2][0]/gluvv.mv->xfSize,c[2][1]/gluvv.mv->yfSize);
				glVertex3f(c[2][0], c[2][1], c[2][2]);
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,c[3][0]/gluvv.mv->xfSize,c[3][1]/gluvv.mv->yfSize);
				glVertex3f(c[3][0], c[3][1], c[3][2]);			
			} glEnd();
		}
		break;
	}

	glDisable(GL_REGISTER_COMBINERS_NV);  //clean up opengl state
	glActiveTexture(GL_TEXTURE0_ARB); { 
				glDisable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NONE);
				glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
	}

}

//======================================================= Get Major Axis
//======================================================================

VolRenMajorAxis  NV20VolRen::getMajorAxis(GLdouble mv[16])
{
   GLdouble zdir[4] = {0, 0, -1, 1};
   GLdouble xdir[4] = {1, 0, 0, 1};
   GLdouble ydir[4] = {0, 1, 0, 1};
   GLdouble newzdir[4], newxdir[4], newydir[4], dotz, dotx, doty;

   mv[12]=0; mv[13]=0; mv[14]=0; 
   translateV3(newzdir, mv, zdir); 
   translateV3(newxdir, mv, xdir); 
   translateV3(newydir, mv, ydir);

   dotz = dotV3(newzdir, zdir);
   dotx = dotV3(newxdir, zdir);
   doty = dotV3(newydir, zdir);
   
   if((dotz > dotx) && (dotz > doty) && (dotz > -doty) && (dotz > -dotx)){
      return VolRenAxisZPos;// positve z axis
   }
   else if((dotx > doty) && (dotx > -doty) && (dotx > -dotz)){
      return VolRenAxisXPos; // positive x axis
   }
   else if((doty > 0) && (doty > -dotx) && (doty > -dotz)){
      return VolRenAxisYPos; // positive y axis
   }
   else if((-dotz > 0) && (-dotz > -dotx) && (-dotz > -doty)){
      return VolRenAxisZNeg; // negative z axis
   }
   else if((-dotx > 0) && (-dotx > -doty)){
      return VolRenAxisXNeg; // negativ x axis
   }
   else if(-doty > 0){
      return VolRenAxisYNeg;// negative y axis
   }
   else{
      cerr << "VolumeRenderer Error: getMajorAxis(): viewing direction unknown\n";
      return VolRenAxisUnknown;
   }
}

//======================================================== Interp Slices
//======================================================================

void NV20VolRen::interpSlices()
{

	glPushMatrix();{
		glTranslatef(gluvv.rinfo.trans[0], //translate
					 			 gluvv.rinfo.trans[1], 
								 gluvv.rinfo.trans[2]);
		glMultMatrixf(gluvv.rinfo.xform);  //rotate
		glTranslatef(-gluvv.mv->xfSize/2,  //center
								 -gluvv.mv->yfSize/2,
								 -gluvv.mv->zfSize/2);
	
	GLfloat lweight[4] = {.5, .5,.5,.5};

	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0_ARB); {
		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
		glBindTexture(GL_TEXTURE_2D, texNames2D[sz/3]);
	}

	glActiveTexture(GL_TEXTURE1_ARB); {
		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
		glBindTexture(GL_TEXTURE_2D, texNames2D[sz/3 * 2]);
	}

	glActiveTexture(GL_TEXTURE2_ARB); {
		glDisable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
	}
	glActiveTexture(GL_TEXTURE3_ARB); {
		glDisable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
	}
	
	GLfloat rweight[4] = {1.0-.5, 0,0,0};

	glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, lweight);
	glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, rweight);
	
	glEnable(GL_REGISTER_COMBINERS_NV);

	//glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 0);
	GlErr("nv20volren","fcom4");
	 //glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	//glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	//glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
	//glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	//glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
	glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	GlErr("nv20volren","fcom1");
	glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	GlErr("nv20volren","fcom2");
	glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	GlErr("nv20volren","fcom3");
	glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	GlErr("nv20volren","fcom0");

	glBegin(GL_QUADS);{
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,0);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,0);
		//glTexCoord2f(0.0,0.0);
		glVertex3f(0,0,.5);
		
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,0);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1,0);
		//glTexCoord2f(1.0,0.0);
		glVertex3f(1,0,.5);
		
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,1);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1,1);
		//glTexCoord2f(1.0,1.0);
		glVertex3f(1,1,.5);
		
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,1);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,1);
		//glTexCoord2f(0.0,1.0);
		glVertex3f(0,1,.5);
	} glEnd();

	glDisable(GL_REGISTER_COMBINERS_NV);

	glActiveTexture(GL_TEXTURE1_ARB); {
		glDisable(GL_TEXTURE_2D);
	}
	glActiveTexture(GL_TEXTURE0_ARB); {
		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, texNames2D[sz/3]);
	}
	glBegin(GL_QUADS);{
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,0);
		//glTexCoord2f(0.0,0.0);
		glVertex3f(-1,0,.5);
		
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,0);
		//glTexCoord2f(1.0,0.0);
		glVertex3f(0,0,.5);
		
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,1);
		//glTexCoord2f(1.0,1.0);
		glVertex3f(0,1,.5);
		
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,1);
		//glTexCoord2f(0.0,1.0);
		glVertex3f(-1,1,.5);
	} glEnd();


	lweight[0]=lweight[1]=lweight[2]=lweight[3]=gluvv.volren.interactSamp;

	interpSlice(temptexNames[0],512,512,texNames2D[sz/3],texNames2D[sz/3*2], lweight);

#if 1

	glActiveTexture(GL_TEXTURE0_ARB); {
		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, temptexNames[0]);
	}
	glActiveTexture(GL_TEXTURE1_ARB); {
		glDisable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NONE);
	}

	glBegin(GL_QUADS);{
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,0);
		glVertex3f(0,-1,.5);
		
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,0);
		glVertex3f(1,-1,.5);
		
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,1);
		glVertex3f(1,0,.5);
		
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,1);
		glVertex3f(0,0,.5);
	} glEnd();
#endif


	}glPopMatrix();
}

//========================================================== Draw Slices
//======================================================================

void NV20VolRen::drawSlices()
{

	glDisable(GL_LIGHTING);
	
	glPushMatrix();{
		glTranslatef(gluvv.rinfo.trans[0], //translate
					 			 gluvv.rinfo.trans[1], 
								 gluvv.rinfo.trans[2]);
		glMultMatrixf(gluvv.rinfo.xform);  //rotate
		glTranslatef(-gluvv.mv->xfSize/2,  //center
								 -gluvv.mv->yfSize/2,
								 -gluvv.mv->zfSize/2);
		
		glEnable(GL_TEXTURE_SHADER_NV);
		GlErr("nv20volren", "enable shader");

		glActiveTexture(GL_TEXTURE0_ARB); {
			glEnable(GL_TEXTURE_2D);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NONE);
			glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texNames2D[sz/2]);
		}
		glActiveTexture(GL_TEXTURE1_ARB); {
			glEnable(GL_TEXTURE_2D);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DEPENDENT_AR_TEXTURE_2D_NV);
			glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, deptexName);
		}
		glActiveTexture(GL_TEXTURE2_ARB); {
			glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NONE);
		}
		glActiveTexture(GL_TEXTURE3_ARB); {
			glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NONE);
		}

		glBegin(GL_QUADS);{
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,0);
			//glTexCoord2f(0.0,0.0);
			glVertex3f(0,0,.5);
			
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,0);
			//glTexCoord2f(1.0,0.0);
			glVertex3f(1,0,.5);
			
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,1);
			//glTexCoord2f(1.0,1.0);
			glVertex3f(1,1,.5);
			
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,1);
			//glTexCoord2f(0.0,1.0);
			glVertex3f(0,1,.5);
		} glEnd();
		
		glDisable(GL_TEXTURE_SHADER_NV);
		glActiveTexture(GL_TEXTURE1_ARB);{
			glDisable(GL_TEXTURE_2D);
		}
		glActiveTexture(GL_TEXTURE0_ARB);{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		}

		//glBindTexture(GL_TEXTURE_2D, texNames2D[sz + (sy/2)]);
		glBindTexture(GL_TEXTURE_2D, deptexName);
		glBegin(GL_QUADS);{
			//glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,0);
			glTexCoord2f(0.0,0.0);
			glVertex3f(0,.5,0);
			
			//glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,0);
			glTexCoord2f(1.0,0.0);
			glVertex3f(1,.5,0);
			
			//glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,1);
			glTexCoord2f(1.0,1.0);
			glVertex3f(1,.5,1);
			
			//glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,1);
			glTexCoord2f(0.0,1.0);
			glVertex3f(0,.5, 1);
		} glEnd();
		
		glBindTexture(GL_TEXTURE_2D, texNames2D[sz + sy + (sx/2)]);

		glBegin(GL_QUADS);{
			//glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,0);
			glTexCoord2f(0.0,0.0);
			glVertex3f(.5,0,0);
			
			//glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,0);
			glTexCoord2f(1.0,0.0);
			glVertex3f(.5,1,0);
			
			//glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,1);
			glTexCoord2f(1.0,1.0);
			glVertex3f(.5,1,1);
			
			//glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,1);
			glTexCoord2f(0.0,1.0);
			glVertex3f(.5,0, 1);
		} glEnd();
		



	} glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

}

//=================================================== Create 2D Textures
//======================================================================

void NV20VolRen::create2DTextures(MetaVolume *mv)
{
#if 0
	
	unsigned char *data = mv->volumes[0].currentData;

	sx = mv->xiSize;  //sizes
	sy = mv->yiSize;
	sz = mv->ziSize;

  unsigned char *gmag;
	unsigned char *gmag2 = new unsigned char[sx*sy*sz];
	unsigned char *grad = new unsigned char[sx*sy*sz*3];
	float *fgrad = new float[sx*sy*sz*3];

	if(!gluvv.mv1){ //no second axis specified, use gmag
		cerr << " Computing derivatives ...";
		gmag = new unsigned char[sx*sy*sz];
		derivative3D(gmag, fgrad, sx, sy, sz, data); 
		gluvv.mv->volumes[0].currentData1 = gmag;
		cerr << endl;
	}
	else{ //second axis IS specified, compute a correlation derivative & gmag2
		gmag = gluvv.mv1->volumes[0].currentData;
		gluvv.mv->volumes[0].currentData1 = gmag;
		addDer(gmag2,fgrad,sx,sy,sz,data,gmag);	
	}

	cerr << "  Blurring normals" ;
	blurV3D(fgrad, 4, 1, 1, 1, sx, sy, sz);
	cerr << "...";
	//blurV3DN(fgrad, 10, 1, 1, 1, sx, sy, sz);
	cerr << endl << "  Scale & Bias" << endl;
	
	scalebiasN(grad, fgrad, sx,sy,sz);

	cerr << "creating textures " << sz << " x " << sy << " x " << sx << endl;

	int stx = 1;          //strides
	int sty = sx;
	int stz = sx * sy;
	
	if(texNames2D) delete[] texNames2D;
	texNames2D = new unsigned int[sz+sy+sx];
	glGenTextures(sz+sy+sx, texNames2D);
  GlErr("NV20VolRen", "genTextures - data textues");
	//if(shadeNames) delete[] shadeNames;
	shadeNames = new unsigned int[sz+sy+sx];
	glGenTextures(sz+sy+sx, shadeNames);
  GlErr("NV20VolRen", "genTextures - shade textures");


	glEnable(GL_TEXTURE_2D);

	// create z axis textures
	unsigned char *tex = new unsigned char[sx*sy*4]; //temperary z axis texture
	unsigned char *shade = new unsigned char[sx*sy*3]; //temp shade texture

	cerr << "Creating Axies: Z " << sz;

	for(int i=0; i<sz; ++i){
		for(int j=0; j<sy; ++j){
			for(int k=0; k<sx; ++k){
				tex[j*sx*4 + k*4 + 0] = 	0;//data[i*stz + j*sty + k];							
				tex[j*sx*4 + k*4 + 1] = 	data[i*stz + j*sty + k];							
				tex[j*sx*4 + k*4 + 2] = 	gmag[i*stz + j*sty + k];//0;//data[i*stz + j*sty + k];							
				tex[j*sx*4 + k*4 + 3] = 	gmag2[i*stz + j*sty + k];//data[i*stz + j*sty + k];
				shade[j*sx*3 + k*3 + 0] = grad[i*stz*3 + j*sty*3 + k*3 + 0];
				shade[j*sx*3 + k*3 + 1] = grad[i*stz*3 + j*sty*3 + k*3 + 1];
				shade[j*sx*3 + k*3 + 2] = grad[i*stz*3 + j*sty*3 + k*3 + 2];
			}
		}
		//glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texNames2D[i]);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D,
								 0,
								 GL_RGBA8,
								 sx,
								 sy,
								 0,
								 GL_RGBA,
								 GL_UNSIGNED_BYTE,
								 tex);

		glFlush();

		glBindTexture(GL_TEXTURE_2D, shadeNames[i]);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D,
								 0,
								 GL_RGB8,
								 sx,
								 sy,
								 0,
								 GL_RGB,
								 GL_UNSIGNED_BYTE,
								 shade);
		glFlush();
	}
#if 1

	// create y axis textures
	delete[] tex;
	delete[] shade;
	
	unsigned char *texY = new unsigned char[sx*sz*4]; //temperary z axis texture
	unsigned char *shadeY = new unsigned char[sx*sz*3]; //temperary z axis texture

	cerr << " Y " << sy;

	for(int j=0; j<sy; ++j){
		for(int i=0; i<sz; ++i){
			for(int k=0; k<sx; ++k){
				texY[i*sx*4 + k*4 + 0] = 0;//data[i*stz + j*sty + k];							
				texY[i*sx*4 + k*4 + 1] = data[i*stz + j*sty + k];//0;
				texY[i*sx*4 + k*4 + 2] = gmag[i*stz + j*sty + k];//0;
				texY[i*sx*4 + k*4 + 3] = gmag2[i*stz + j*sty + k];
				shadeY[i*sx*3 + k*3 + 0] = grad[i*stz*3 + j*sty*3 + k*3 + 0];
				shadeY[i*sx*3 + k*3 + 1] = grad[i*stz*3 + j*sty*3 + k*3 + 1];
				shadeY[i*sx*3 + k*3 + 2] = grad[i*stz*3 + j*sty*3 + k*3 + 2];
			}
		}
		glBindTexture(GL_TEXTURE_2D, texNames2D[sz + j]);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,
								 0,
								 GL_RGBA8,
								 sx,
								 sz,
								 0,
								 GL_RGBA,
								 GL_UNSIGNED_BYTE,
								 texY);
		glFlush();

		glBindTexture(GL_TEXTURE_2D, shadeNames[sz + j]);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,
								 0,
								 GL_RGB8,
								 sx,
								 sz,
								 0,
								 GL_RGB,
								 GL_UNSIGNED_BYTE,
								 shadeY);

		glFlush();
	}

	// create x axis textures
	delete[] texY;
	delete[] shadeY;

	unsigned char *texX = new unsigned char[sy*sz*4]; //temperary z axis texture
	unsigned char *shadeX = new unsigned char[sy*sz*3]; //temperary z axis texture

	cerr << " X " << sx;

	for(int k=0; k<sx; ++k){
		for(int i=0; i<sz; ++i){
			for(int j=0; j<sy; ++j){
				texX[i*sy*4 + j*4 + 0] = 	0;//data[i*stz + j*sty + k];							
				texX[i*sy*4 + j*4 + 1] = 	data[i*stz + j*sty + k];//0;							
				texX[i*sy*4 + j*4 + 2] = 	gmag[i*stz + j*sty + k];//0;							
				texX[i*sy*4 + j*4 + 3] = 	gmag2[i*stz + j*sty + k];							
				shadeX[i*sy*3 + j*3 + 0] = 	grad[i*stz*3 + j*sty*3 + k*3 + 0];							
				shadeX[i*sy*3 + j*3 + 1] = 	grad[i*stz*3 + j*sty*3 + k*3 + 1];							
				shadeX[i*sy*3 + j*3 + 2] = 	grad[i*stz*3 + j*sty*3 + k*3 + 2];							
			}
		}
		glBindTexture(GL_TEXTURE_2D, texNames2D[sz + sy + k]);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		glTexImage2D(GL_TEXTURE_2D,
								 0,
								 GL_RGBA8,
								 sy,
								 sz,
								 0,
								 GL_RGBA,
								 GL_UNSIGNED_BYTE,
								 texX);
		glFlush();

		glBindTexture(GL_TEXTURE_2D, shadeNames[sz + sy + k]);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


		glTexImage2D(GL_TEXTURE_2D,
								 0,
								 GL_RGB8,
								 sy,
								 sz,
								 0,
								 GL_RGB,
								 GL_UNSIGNED_BYTE,
								 shadeX);
		glFlush();

	}
#endif
	
	cerr << endl;

	glDisable(GL_TEXTURE_2D);

	GlErr("NV20VolRen", "glTexImage2D");
#endif
}

//==================================================== Create 2D Dep Tex
//======================================================================

void NV20VolRen::create2DDepTex()
{
	glGenTextures(1, &deptexName);
	glGenTextures(1, &gluvv.volren.deptexName);

	deptex = new unsigned char[256*256*4]; //reference texture

	gluvv.volren.deptex = deptex;

	for(int j=0; j<256; ++j){
		for(int k=0; k<256; ++k){
			deptex[j*256*4 + k*4 + 0] =   k;							
			deptex[j*256*4 + k*4 + 1] = 	j;//k==0 ? 0 : 255;							
			deptex[j*256*4 + k*4 + 2] = 	255 - j; //k==0 ? 255 : 0;							
			deptex[j*256*4 + k*4 + 3] = 	(unsigned char)(j/(float)2);							
		}
	}
		
	gDeptex = new unsigned char[256*256*4]; //for good sample rates
	iDeptex = new unsigned char[256*256*4]; //for interactive sample rate
	copyScale(gluvv.volren.goodSamp, gDeptex);
	copyScale(gluvv.volren.interactSamp, iDeptex);

	lastSamp = gluvv.volren.goodSamp;
	lastGoodSamp = gluvv.volren.goodSamp;
	lastInteSamp = gluvv.volren.interactSamp;

	loadDepTex(gDeptex);

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
			deptex);
	glFlush();

}



void NV20VolRen::loadDepTex(unsigned char *dtex)
{
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
			dtex);
	glFlush();
}

//=================================================== copy & scale alpha
//======================================================================

void NV20VolRen::scaleAlpha(float nsr, float osr, unsigned char *dtex)
{
	if(nsr == osr) return;
	float alphaScale = 1/nsr;
	for(int i=0; i<256; ++i){
		for(int j=0; j<256; ++j){
			dtex[i*256*4 + j*4 + 3] = 
				(1- pow((1.0-deptex[i*256*4 + j*4 + 3]/255.0), alphaScale))*255;
		}
	}
}

void NV20VolRen::copyScale(float sr, unsigned char *dtex)
{
	float alphaScale = 1.0/sr;
	for(int i=0; i<256; ++i){
		for(int j=0; j<256; ++j){
			dtex[i*256*4 + j*4 + 0] = deptex[i*256*4 + j*4 + 0];
			dtex[i*256*4 + j*4 + 1] = deptex[i*256*4 + j*4 + 1];
			dtex[i*256*4 + j*4 + 2] = deptex[i*256*4 + j*4 + 2];
			dtex[i*256*4 + j*4 + 3] = 
				(1.0 - pow((1.0-(deptex[i*256*4 + j*4 + 3]/255.0)), alphaScale))*255;
		}
	}
}


//====================================================== Create Temp Tex
//======================================================================

void NV20VolRen::createTempTex()
{
	unsigned char *texZ = new unsigned char[sx*sy*4];
	unsigned char *texY = new unsigned char[sx*sz*4];
	unsigned char *texX = new unsigned char[sy*sz*4];
	
	glGenTextures(1, temptexNames);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, temptexNames[0]);
		
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
	glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			sx,
			sy,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			texZ);
	glFlush();
	
	glBindTexture(GL_TEXTURE_2D, temptexNames[1]);
		
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
	glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			sx,
			sz,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			texY);
	glFlush();

	glBindTexture(GL_TEXTURE_2D, temptexNames[2]);
		
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
	glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			sy,
			sz,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			texX);
	glFlush();
}

//========================================================= Make Current
//======================================================================

void NV20VolRen::MakeCurrent()
{
	glutSetWindow(gluvv.mainWindow);
}

//========================================================== interpSlice
//======================================================================

inline 
void NV20VolRen::interpSlice(unsigned int tname,     //temperary texture to write to 
  		                       int width, int height,  //size of textures
								        	   unsigned int atname,    //A texture
									           unsigned int btname,    //B texture
									           float w0[4])       //W, compute: AW + (1-W)B ->tname
{
	//cerr << "w = " << width << " h = " << height << "atname" <<  atname << "  btname" << btname << endl;
	if(gluvv.picking) return;
	//pbuff->HandleModeSwitch();
	pbuff->MakeCurrent();

  GlErr("nv20volren","is-fcom4.7");
	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glActiveTexture(GL_TEXTURE0_ARB); { //A texture
		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, atname);
	}

	glActiveTexture(GL_TEXTURE1_ARB); { //B texture
		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, btname);
	}

	glActiveTexture(GL_TEXTURE2_ARB); { //nothing
		glDisable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
	}
	glActiveTexture(GL_TEXTURE3_ARB); { //nothing
		glDisable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
	}
	
	float w1[4] = {w0[0], w0[1], w0[2], w0[3]};
	float w2[4] = {1-w0[0], 1-w0[1], 1-w0[2], 1-w0[3]};
	
	GlErr("nv20volren","is-fcom4.6");

	glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, w0); //set W
	if(GlErr("nv20volren","is-fcom4.5")){
		cerr << " w" << w0[0] << " " <<w0[1] << " " << w0[2] << " " << w0[3] << endl;

	}
	
	glEnable(GL_REGISTER_COMBINERS_NV);

#if 0

	//computes: A(W) + (1-W)B + 0
	glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	GlErr("nv20volren","fcom1");
	glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	GlErr("nv20volren","fcom2");
	glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	GlErr("nv20volren","fcom3");
	glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	GlErr("nv20volren","fcom0");

#else
	
	glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
	GlErr("nv20volren","is-fcom4.4");
	glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_INVERT_NV, GL_RGB);
	glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, 
		GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
	GlErr("nv20volren","is-fcom4.3");

	glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
	glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
	glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
	glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
	glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, 
		GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
	GlErr("nv20volren","is-fcom4.2");

	glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	GlErr("nv20volren","fcom1");
	glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	GlErr("nv20volren","fcom2");
	glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	GlErr("nv20volren","fcom3");
	glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
	GlErr("nv20volren","is-fcom0");
	
	glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
	GlErr("nv20volren","is-fcom4.1");

#endif

	glColor4f(1,1,1,1);
	glBegin(GL_QUADS);{
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,0);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,0);
		glVertex3f(0,0,.5);
		
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,0);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1,0);
		glVertex3f(width,0,.5);
		
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,1);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1,1);
		glVertex3f(width,height,.5);
		
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,1);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,1);
		glVertex3f(0,height,.5);
	} glEnd();


	glDisable(GL_REGISTER_COMBINERS_NV);

	glBindTexture(GL_TEXTURE_2D, tname);
	//see if glCopyTexImage2D is faster!
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);
	glFlush();

	MakeCurrent();
	GlErr("nv20volren","is-makecurrent");

}