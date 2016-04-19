// testPert.cpp: implementation of the testPert class.
//
//////////////////////////////////////////////////////////////////////

#include "testPert.h"
#include "gluvv.h"
#include <iostream.h>
#include "PNoise.h"
#include "glUE.h"
#include "VectorMath.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

testPert::testPert()
{

}

testPert::~testPert()
{

}


void  testPert::init()
{
		if(!(pbuff = new PBuffer(gluvv.light.buffsz[0], gluvv.light.buffsz[1], 
			                       GLUT_SINGLE | GLUT_RGBA)))
		{
			cerr << "ERROR: PBuffer intialization failed!" << endl;
		} 
		else 
		{
			cerr << "you are in the P-Buffer tester initialize" << endl;
			pbuff->Initialize(true, true); //create a shared pbuffer, with render to texture
			pbuff->MakeCurrent(); 
			{   
				glClearColor( 1.0, 0.0, 1.0, 0.0 );			//this is not the color it gets cleared with
				glMatrixMode( GL_PROJECTION );
				glOrtho(0,1,0,1,-1,3);
				glMatrixMode(GL_MODELVIEW);
				gluLookAt(0,0,2,
					        0,0,0,
									0,1,0);
        glClear(GL_COLOR_BUFFER_BIT);
			} MakeCurrent();
			cerr << "PBuffer intialization succeded" << endl;
		}

  //--------------------------------------------------------

  glGenTextures(1, &ptTexName);   //the copy texture
	unsigned char *stex = new unsigned char[gluvv.light.buffsz[0]*gluvv.light.buffsz[1]*4];

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ptTexName);		
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
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
  delete[] stex;

  //--------------------------------------------------------

  int sx = 32;
  int sy = 32;

  unsigned char *data = new unsigned char[sx*sy*4];

  for(int i=0; i< sy; ++i){
    for(int j=0; j<sx; ++j){
      data[i*sx*4 + j*4 + 0] = ((j/(sx/16)) % 2)   * 255;  
      data[i*sx*4 + j*4 + 1] = ((i/(sy/16) + 1) % 2)   * 255;  
      data[i*sx*4 + j*4 + 2] = 0;//(((j*i)/16.0) % 2) * 255;  
      data[i*sx*4 + j*4 + 3] = 255;  
    }
  }

  glGenTextures(1, &texName);   //the data texture

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName);		
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
			data);
	glFlush();
	glDisable(GL_TEXTURE_2D);

  
  //--------------------------------------------------------

  PNoise px;
  PNoise py;
  PNoise pz;
  PNoise pw;
  px.setParams(1,1,1);
  py.setParams(1,1,1);
  pz.setParams(1,1,1);
  pw.setParams(1,1,1);

  unsigned char *ptdata = new unsigned char[64*64*64*4];

#if 0
  for(i = 0; i<64; ++i){
    for(int j=0; j<64; ++j){
      for(int k=0; k<64; ++k){
        ptdata[i*64*64*4 + j*64*4 + k*4 + 0] = (px.PNoise3D(i/63.0*2, j/63.0*2, k/63.0*2)*.5 + .5) * 255;
        ptdata[i*64*64*4 + j*64*4 + k*4 + 1] = (py.PNoise3D(i/63.0*2, j/63.0*2, k/63.0*2)*.5 + .5) * 255;
        ptdata[i*64*64*4 + j*64*4 + k*4 + 2] = (pz.PNoise3D(i/63.0*2, j/63.0*2, k/63.0*2)*.5 + .5) * 255;
        ptdata[i*64*64*4 + j*64*4 + k*4 + 3] = (pw.PNoise3D(i/63.0*2, j/63.0*2, k/63.0*2)*.5 + .5) * 255;
      }
    }
  }
#else

  //srand(time(NULL));
  srand(1);

  for(i = 0; i<64; ++i){
    for(int j=0; j<64; ++j){
      for(int k=0; k<64; ++k){
        ptdata[i*64*64*4 + j*64*4 + k*4 + 0] = ((rand()/(float)RAND_MAX *.5) + .5) * 255;
        ptdata[i*64*64*4 + j*64*4 + k*4 + 1] = ((rand()/(float)RAND_MAX *.5) + .5) * 255;;
        ptdata[i*64*64*4 + j*64*4 + k*4 + 2] = ((rand()/(float)RAND_MAX *.5) + .5) * 255;;
        ptdata[i*64*64*4 + j*64*4 + k*4 + 3] = ((rand()/(float)RAND_MAX *.5) + .5) * 255;;
      }
    }
  }

  for(int t=0; t<1; ++ t){
    for(i = 0; i<64; ++i){
      for(int j=0; j<64; ++j){
        for(int k=0; k<64; ++k){
          int upx = (k + 1) %64;
          int dnx = (k - 1) < 0 ? 63 : (k - 1);
          int upy = (j + 1) %64;
          int dny = (j - 1) < 0 ? 63 : (j - 1);
          int upz = (i + 1) %64;
          int dnz = (i - 1) < 0 ? 63 : (i - 1);
          ptdata[i*64*64*4 + j*64*4 + k*4 + 0] = 
            (ptdata[upz*64*64*4 + j*64*4 + k*4 + 0] + ptdata[dnz*64*64*4 + j*64*4 + k*4 + 0] + 
            ptdata[i*64*64*4 + upy*64*4 + k*4 + 0] + ptdata[i*64*64*4 + dny*64*4 + k*4 + 0] +
            ptdata[i*64*64*4 + j*64*4 + upx*4 + 0] + ptdata[i*64*64*4 + j*64*4 + dnx*4 + 0] +
            ptdata[i*64*64*4 + j*64*4 + k*4 + 0]*10)/16.0;
          ptdata[i*64*64*4 + j*64*4 + k*4 + 1] = 
            (ptdata[upz*64*64*4 + j*64*4 + k*4 + 1] + ptdata[dnz*64*64*4 + j*64*4 + k*4 + 1] + 
            ptdata[i*64*64*4 + upy*64*4 + k*4 + 1] + ptdata[i*64*64*4 + dny*64*4 + k*4 + 1] +
            ptdata[i*64*64*4 + j*64*4 + upx*4 + 1] + ptdata[i*64*64*4 + j*64*4 + dnx*4 + 1] +
            ptdata[i*64*64*4 + j*64*4 + k*4 + 1]*10)/16.0;
          ptdata[i*64*64*4 + j*64*4 + k*4 + 2] = 
            (ptdata[upz*64*64*4 + j*64*4 + k*4 + 2] + ptdata[dnz*64*64*4 + j*64*4 + k*4 + 2] + 
            ptdata[i*64*64*4 + upy*64*4 + k*4 + 2] + ptdata[i*64*64*4 + dny*64*4 + k*4 + 2] +
            ptdata[i*64*64*4 + j*64*4 + upx*4 + 2] + ptdata[i*64*64*4 + j*64*4 + dnx*4 + 2] +
            ptdata[i*64*64*4 + j*64*4 + k*4 + 2]*10)/16.0;
          ptdata[i*64*64*4 + j*64*4 + k*4 + 3] = 
            (ptdata[upz*64*64*4 + j*64*4 + k*4 + 3] + ptdata[dnz*64*64*4 + j*64*4 + k*4 + 3] + 
            ptdata[i*64*64*4 + upy*64*4 + k*4 + 3] + ptdata[i*64*64*4 + dny*64*4 + k*4 + 3] +
            ptdata[i*64*64*4 + j*64*4 + upx*4 + 3] + ptdata[i*64*64*4 + j*64*4 + dnx*4 + 3] +
            ptdata[i*64*64*4 + j*64*4 + k*4 + 3]*10)/16.0;
        }
      }
    }
  }
#endif

  glGenTextures(1, &ptName);   //the perturbation texture

	glEnable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, ptName);		
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage3DEXT(GL_TEXTURE_3D,
			0,
			GL_RGBA8,
			64,
			64,
      64,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			ptdata);
	glFlush();
	glDisable(GL_TEXTURE_3D);


  //--------------------------------------------------------

  fsname = glGenFragmentShadersATI(1);
  
  glEnable(GL_FRAGMENT_SHADER_ATI);
  {                                        //clipping plane shader
    glBindFragmentShaderATI(fsname);
    glBeginFragmentShaderATI();
    {
		  glPassTexCoordATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STRQ_ATI);  //grab texcoords
      glSampleMapATI(GL_REG_1_ATI, GL_TEXTURE1_ARB, GL_SWIZZLE_STRQ_ATI);   //grab perterb
      glSampleMapATI(GL_REG_2_ATI, GL_TEXTURE2_ARB, GL_SWIZZLE_STRQ_ATI);   //grab perterb
      
      glColorFragmentOp2ATI(GL_MUL_ATI,
        GL_REG_1_ATI, GL_NONE, GL_NONE,
        GL_CON_0_ATI, GL_RED,  GL_NONE,
        GL_REG_1_ATI, GL_NONE, GL_BIAS_BIT_ATI | GL_2X_BIT_ATI);

      glColorFragmentOp2ATI(GL_ADD_ATI,
        GL_REG_1_ATI, GL_RED_BIT_ATI|GL_GREEN_BIT_ATI, GL_NONE,
        GL_REG_0_ATI, GL_NONE, GL_NONE,
        GL_REG_1_ATI, GL_NONE, GL_NONE);

      glColorFragmentOp2ATI(GL_MUL_ATI,
        GL_REG_2_ATI, GL_NONE, GL_NONE,
        GL_CON_0_ATI, GL_GREEN,  GL_NONE,
        GL_REG_2_ATI, GL_NONE, GL_BIAS_BIT_ATI | GL_2X_BIT_ATI);

      glColorFragmentOp2ATI(GL_ADD_ATI,
        GL_REG_1_ATI, GL_RED_BIT_ATI|GL_GREEN_BIT_ATI, GL_NONE,
        GL_REG_2_ATI, GL_NONE, GL_NONE,
        GL_REG_1_ATI, GL_NONE, GL_NONE);

      glSampleMapATI(GL_REG_0_ATI, GL_REG_1_ATI, GL_SWIZZLE_STR_ATI);   //grab data
      
    } glEndFragmentShaderATI();
  } glDisable(GL_FRAGMENT_SHADER_ATI);

  
  fsname2 = glGenFragmentShadersATI(1);
  
  glEnable(GL_FRAGMENT_SHADER_ATI);
  {                                        //procedural data shader
    glBindFragmentShaderATI(fsname2);
    glBeginFragmentShaderATI();
    {
		  //glPassTexCoordATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STRQ_ATI);  //grab texcoords
      glSampleMapATI(GL_REG_1_ATI, GL_TEXTURE1_ARB, GL_SWIZZLE_STRQ_ATI);   //grab perterb
      glSampleMapATI(GL_REG_2_ATI, GL_TEXTURE2_ARB, GL_SWIZZLE_STRQ_ATI);   //grab perterb
      
      glColorFragmentOp2ATI(GL_MUL_ATI,
        GL_REG_1_ATI, GL_NONE, GL_SATURATE_BIT_ATI,
        GL_CON_0_ATI, GL_RED,  GL_NONE,
        GL_REG_1_ATI, GL_RED, GL_BIAS_BIT_ATI | GL_2X_BIT_ATI);

      glColorFragmentOp2ATI(GL_MUL_ATI,
        GL_REG_3_ATI, GL_NONE, GL_SATURATE_BIT_ATI,
        GL_CON_0_ATI, GL_RED,  GL_NONE,
        GL_REG_1_ATI, GL_RED, GL_BIAS_BIT_ATI | GL_2X_BIT_ATI | GL_COMP_BIT_ATI);

      glColorFragmentOp2ATI(GL_MUL_ATI,
        GL_REG_2_ATI, GL_NONE, GL_NONE,
        GL_CON_0_ATI, GL_GREEN,  GL_NONE,
        GL_REG_2_ATI, GL_NONE, GL_BIAS_BIT_ATI | GL_2X_BIT_ATI);

      glColorFragmentOp2ATI(GL_ADD_ATI,  //absolute value
        GL_REG_0_ATI, GL_NONE, GL_NONE,
        GL_REG_3_ATI, GL_NONE, GL_NONE,
        GL_REG_1_ATI, GL_NONE, GL_NONE);

      glColorFragmentOp2ATI(GL_ADD_ATI,
        GL_REG_0_ATI, GL_NONE, GL_NONE,
        GL_REG_2_ATI, GL_RED, GL_NONE,
        GL_REG_0_ATI, GL_NONE, GL_NONE);

      glColorFragmentOp2ATI(GL_MUL_ATI,
        GL_REG_0_ATI, GL_NONE, GL_SATURATE_BIT_ATI,
        GL_REG_0_ATI, GL_NONE, GL_COMP_BIT_ATI,
        GL_REG_0_ATI, GL_NONE, GL_COMP_BIT_ATI);

      //glSampleMapATI(GL_REG_0_ATI, GL_REG_1_ATI, GL_SWIZZLE_STR_ATI);   //grab data
      
    } glEndFragmentShaderATI();
  } glDisable(GL_FRAGMENT_SHADER_ATI);
}


//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

void  testPert::draw()
{

  //draw the origional data ----------------------------------------------
  glActiveTexture(GL_TEXTURE0_ARB); {
    glEnable(GL_TEXTURE_2D);
  	glBindTexture(GL_TEXTURE_2D, texName);		
  }

  glColor3f(1,1,1);

  glPushMatrix();
	{
    glTranslatef(gluvv.rinfo.trans[0], gluvv.rinfo.trans[1], gluvv.rinfo.trans[2]);
    glTranslatef(-1, -1, 0);
    glBegin(GL_QUADS);
    {
      glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,0);
      glVertex3f(0, 0, 0);
      
      glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,1);
      glVertex3f(0, 1, 0);
      
      glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,1);
      glVertex3f(1, 1, 0);
      
      glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,0);
      glVertex3f(1, 0, 0);
    } glEnd();
  } glPopMatrix();

  glActiveTexture(GL_TEXTURE0_ARB); {
    glDisable(GL_TEXTURE_2D);
  }

  //draw the perturbation data ----------------------------------------------
  glActiveTexture(GL_TEXTURE0_ARB); {
    glEnable(GL_TEXTURE_3D);
  	glBindTexture(GL_TEXTURE_3D, ptName);		
  }

  glColor3f(1,1,1);

  glPushMatrix();
	{
    glTranslatef(gluvv.rinfo.trans[0], gluvv.rinfo.trans[1], gluvv.rinfo.trans[2]);
    glTranslatef(0, 0, 0);
    glBegin(GL_QUADS);
    {
      glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,0);
      glVertex3f(0, 0, 0);
      
      glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,2);
      glVertex3f(0, 1, 0);
      
      glMultiTexCoord2fARB(GL_TEXTURE0_ARB,2,2);
      glVertex3f(1, 1, 0);
      
      glMultiTexCoord2fARB(GL_TEXTURE0_ARB,2,0);
      glVertex3f(1, 0, 0);
    } glEnd();
  } glPopMatrix();

  glActiveTexture(GL_TEXTURE0_ARB); {
    glDisable(GL_TEXTURE_3D);
  }

  //draw the perturbed data ----------------------------------------------

  GlErr("testPert", "start of draw perturbed");

  float cons0[4] = {gluvv.light.iShadowQual, gluvv.light.amb,0,0};
  glSetFragmentShaderConstantATI(GL_CON_0_ATI, cons0);


  glActiveTexture(GL_TEXTURE0_ARB); {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texName);		
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
  }
  GlErr("testPert", "draw perturbed, tex0");
  
  glActiveTexture(GL_TEXTURE1_ARB); {
    glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, ptName);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
  }
  
  glActiveTexture(GL_TEXTURE2_ARB); {
    glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, ptName);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
  }

  GlErr("testPert", "draw perturbed, tex1");
  
  GlErr("testPert", "right before draw perturbed");

  glColor3f(1,1,1);
  
  glPushMatrix();
  {
    glTranslatef(gluvv.rinfo.trans[0], gluvv.rinfo.trans[1], gluvv.rinfo.trans[2]);
    glTranslatef(-1, 0, 0);
    glEnable(GL_FRAGMENT_SHADER_ATI);
    {                                        //perturbation shader
      glBindFragmentShaderATI(fsname);
      
      glBegin(GL_QUADS);
      {
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,0);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,0);
        glMultiTexCoord2fARB(GL_TEXTURE2_ARB,0,0);
        glVertex3f(0, 0, 0);
        
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,1);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,.2);
        glMultiTexCoord2fARB(GL_TEXTURE2_ARB,0,.27);
        glVertex3f(0, 1, 0);
        
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,1);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB,.2,.2);
        glMultiTexCoord2fARB(GL_TEXTURE2_ARB,1.27,1.27);
        glVertex3f(1, 1, 0);
        
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,0);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB,.2,0);
        glMultiTexCoord2fARB(GL_TEXTURE2_ARB,1.27,0);
        glVertex3f(1, 0, 0);
      } glEnd();
    }
    glDisable(GL_FRAGMENT_SHADER_ATI);
    
  } glPopMatrix();

  GlErr("testPert", "right after draw perturbed");

  glActiveTexture(GL_TEXTURE0_ARB); {
    glDisable(GL_TEXTURE_2D);
  }

  glActiveTexture(GL_TEXTURE1_ARB); {
    glDisable(GL_TEXTURE_3D);
  }

  glActiveTexture(GL_TEXTURE2_ARB); {
    glDisable(GL_TEXTURE_3D);
  }


  //draw the procedural data ----------------------------------------------

  glActiveTexture(GL_TEXTURE1_ARB); {
    glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, ptName);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
  }
  
  glActiveTexture(GL_TEXTURE2_ARB); {
    glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, ptName);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
  }

  GlErr("testPert", "draw perturbed, tex1");
  
  GlErr("testPert", "right before draw perturbed");

  glColor3f(1,1,1);
  
  glPushMatrix();
  {
    glTranslatef(gluvv.rinfo.trans[0], gluvv.rinfo.trans[1], gluvv.rinfo.trans[2]);
    glTranslatef(0, -1, 0);
    glEnable(GL_FRAGMENT_SHADER_ATI);
    {                                        //procedual texture shader
      glBindFragmentShaderATI(fsname2);
      
      glBegin(GL_QUADS);
      {
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,0);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,0);
        glMultiTexCoord2fARB(GL_TEXTURE2_ARB,0,0);
        glVertex3f(0, 0, 0);
        
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,1);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,.2);
        glMultiTexCoord2fARB(GL_TEXTURE2_ARB,0,.6);
        glVertex3f(0, 1, 0);
        
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,1);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB,.2,.2);
        glMultiTexCoord2fARB(GL_TEXTURE2_ARB,.6,.6);
        glVertex3f(1, 1, 0);
        
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,0);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB,.2,0);
        glMultiTexCoord2fARB(GL_TEXTURE2_ARB,.6,0);
        glVertex3f(1, 0, 0);
      } glEnd();
    }
    glDisable(GL_FRAGMENT_SHADER_ATI);
    
  } glPopMatrix();

  GlErr("testPert", "right after draw perturbed");

  glActiveTexture(GL_TEXTURE0_ARB); {
    glDisable(GL_TEXTURE_2D);
  }

  glActiveTexture(GL_TEXTURE1_ARB); {
    glDisable(GL_TEXTURE_3D);
  }

  glActiveTexture(GL_TEXTURE2_ARB); {
    glDisable(GL_TEXTURE_3D);
  }


  GlErr("testPert", "end draw");
}

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

void testPert::MakeCurrent()        //switch back to this context
{
	  glutSetWindow(gluvv.mainWindow);


}