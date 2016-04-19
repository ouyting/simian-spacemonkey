// testPBuff.cpp: implementation of the testPBuff class.
//
//////////////////////////////////////////////////////////////////////

#include "testPBuff.h"
#include <iostream.h>
#include "gluvv.h"
#include "glUE.h"

#define _RENDER_TO_TEXTURE 1

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

testPBuff::testPBuff()
{
  pbuff = 0;
  pbuff2 = 0;
}

testPBuff::~testPBuff()
{

}

void testPBuff::init()
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

    if(!(pbuff2 = new PBuffer(gluvv.light.buffsz[0], gluvv.light.buffsz[1], 
			                       GLUT_SINGLE | GLUT_RGBA)))
		{
			cerr << "ERROR: PBuffer intialization failed!" << endl;
		} 
		else 
		{
			cerr << "you are in the P-Buffer2 tester initialize" << endl;
			pbuff2->Initialize(true, true); //create a shared pbuffer, with render to texture
			pbuff2->MakeCurrent(); 
			{   
				glClearColor( 0.0, 0.0, 1.0, 1.0 );			//this is not the color it gets cleared with
				glMatrixMode( GL_PROJECTION );
				glOrtho(0,1,0,1,-1,3);
				glMatrixMode(GL_MODELVIEW);
				gluLookAt(0,0,2,
					        0,0,0,
									0,1,0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        fsname1 = glGenFragmentShadersATI(1);
        
        glEnable(GL_FRAGMENT_SHADER_ATI);
        {                                        //clipping plane shader
          glBindFragmentShaderATI(fsname1);
          glBeginFragmentShaderATI();
          {
            glSampleMapATI(GL_REG_1_ATI, GL_TEXTURE1_ARB, GL_SWIZZLE_STR_ATI);   //grab data
            
            glColorFragmentOp1ATI(GL_MOV_ATI,
              GL_REG_0_ATI, GL_NONE, GL_SATURATE_BIT_ATI,
              GL_REG_1_ATI, GL_NONE, GL_NONE);
            
          } glEndFragmentShaderATI();
        } glDisable(GL_FRAGMENT_SHADER_ATI);
      
      } MakeCurrent();
			cerr << "PBuffer intialization succeded" << endl;
   
    }

	
	glGenTextures(1, &shadowName);   //the copy texture. forget about the name
	unsigned char *stex = new unsigned char[gluvv.light.buffsz[0]*gluvv.light.buffsz[1]*4];

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, shadowName);		
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

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

  
  fsname = glGenFragmentShadersATI(1);
	
	glEnable(GL_FRAGMENT_SHADER_ATI);
	{                                        //clipping plane shader
		glBindFragmentShaderATI(fsname);
		glBeginFragmentShaderATI();
		{
      glSampleMapATI(GL_REG_4_ATI, GL_TEXTURE4_ARB, GL_SWIZZLE_STR_ATI);   //grab data
      glSampleMapATI(GL_REG_5_ATI, GL_TEXTURE5_ARB, GL_SWIZZLE_STR_ATI);   //grab data
				
      glColorFragmentOp1ATI(GL_MOV_ATI, //no op.
				  GL_REG_0_ATI, GL_RED_BIT_ATI, GL_SATURATE_BIT_ATI,
					GL_REG_4_ATI, GL_RED, GL_NONE);
      
      glColorFragmentOp1ATI(GL_MOV_ATI, //no op.
				  GL_REG_0_ATI, GL_GREEN_BIT_ATI, GL_SATURATE_BIT_ATI,
					GL_REG_5_ATI, GL_GREEN, GL_NONE);
 
		} glEndFragmentShaderATI();
	} glDisable(GL_FRAGMENT_SHADER_ATI);

  fsname2 = glGenFragmentShadersATI(1);
  
  glEnable(GL_FRAGMENT_SHADER_ATI);
  {                                        //clipping plane shader
    glBindFragmentShaderATI(fsname2);
    glBeginFragmentShaderATI();
    {
      glSampleMapATI(GL_REG_4_ATI, GL_TEXTURE4_ARB, GL_SWIZZLE_STR_ATI);   //grab data
      
      glColorFragmentOp1ATI(GL_MOV_ATI,
        GL_REG_0_ATI, GL_NONE, GL_SATURATE_BIT_ATI,
        GL_REG_4_ATI, GL_NONE, GL_NONE);
      
    } glEndFragmentShaderATI();
  } glDisable(GL_FRAGMENT_SHADER_ATI);

	GlErr("TestRen::draw2()", "build fragment shader");
}

//===================================================================================
//===================================================================================
//===================================================================================

void testPBuff::draw()
{
   //draw1();

   draw2();

   //draw3();
}

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================


void testPBuff::draw1()  //this doesn't work at all
{
  static int currentPbuff = 0;

	if(gluvv.tf.histOn){
		cerr << "using copy method" << endl;
	} else {
		cerr << "using render to texture" << endl;
	}

	for(int i=0; i<1; ++i){

    if(currentPbuff == 0)
    {
      pbuff->MakeCurrent();
      {
        cerr << "buffer 1" << endl;
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
        glPushMatrix();
        {
          glTranslatef(.5,.5,0);
          glMultMatrixf(gluvv.rinfo.xform);
          glColor3f(0,1,0);
          glutSolidTeapot(.3);
        } glPopMatrix();
        if(gluvv.tf.histOn){             //don't worry about this, just tests copies
          //cerr << "Copying" << endl;
          glBindTexture(GL_TEXTURE_2D, shadowName);
          glCopyTexSubImage2D(GL_TEXTURE_2D, 0, (int)0, (int)0, 
            (int)0, (int)0,
            (int)gluvv.light.buffsz[0], (int)gluvv.light.buffsz[1]);
        }
      } MakeCurrent();
    }		
    else 
    {
      cerr << "buffer 2" << endl;
      pbuff2->MakeCurrent();
      {
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
        glPushMatrix();
        {
          glTranslatef(.5,.5,0);
          glMultMatrixf(gluvv.rinfo.xform);
          glColor3f(1, 0, 0);
          glutSolidSphere(.3, 5, 5);
        } glPopMatrix();
        if(gluvv.tf.histOn){
          //cerr << "Copying" << endl;
          glBindTexture(GL_TEXTURE_2D, shadowName);
          glCopyTexSubImage2D(GL_TEXTURE_2D, 0, (int)0, (int)0, 
            (int)0, (int)0,
            (int)gluvv.light.buffsz[0], (int)gluvv.light.buffsz[1]);
        }
      } MakeCurrent();

    }

		glActiveTexture(GL_TEXTURE5_ARB); {
			glEnable(GL_TEXTURE_2D);
			if(gluvv.tf.histOn){
				glBindTexture(GL_TEXTURE_2D, shadowName);
			} 
      else 
      {
				//cerr << "Binding p-buffer" << endl;
        if(currentPbuff == 0)              //this should be allowed!!!!
        {
          cerr << "buffer 1 bind" << endl;
  				wglBindTexImageARB(pbuff->buffer, WGL_FRONT_LEFT_ARB);
        }
        else
        {
          cerr << "buffer 2 bind" << endl;
  				wglBindTexImageARB(pbuff2->buffer, WGL_FRONT_LEFT_ARB);
        }
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			}
		}

		glPushMatrix();
		{
			glTranslatef(gluvv.rinfo.trans[0], gluvv.rinfo.trans[1], gluvv.rinfo.trans[2]);
      if(currentPbuff == 0)
        glTranslatef(-1, 0, 0);
			glBegin(GL_QUADS);
			{
				glMultiTexCoord2fARB(GL_TEXTURE5_ARB,0,0);
				//glTexCoord2f(0,0);
				glVertex3f(-.5, -.5, 0);
				
				glMultiTexCoord2fARB(GL_TEXTURE5_ARB,0,1);
				//glTexCoord2f(0,1);
				glVertex3f(-.5, 0, 0);
				
				glMultiTexCoord2fARB(GL_TEXTURE5_ARB,1,1);
				//glTexCoord2f(1,1);
				glVertex3f(0, 0, 0);
				
				glMultiTexCoord2fARB(GL_TEXTURE5_ARB,1,0);
				//glTexCoord2f(1,0);
				glVertex3f(0, -.5, 0);
			} glEnd();
		} glPopMatrix();

    glActiveTexture(GL_TEXTURE5_ARB); 
    {  
      if(currentPbuff == 0)
      {
        wglReleaseTexImageARB(pbuff->buffer, WGL_FRONT_LEFT_ARB);
        currentPbuff = 1;
      }
      else 
      {
        wglReleaseTexImageARB(pbuff2->buffer, WGL_FRONT_LEFT_ARB);
        currentPbuff = 0;
      }
      glDisable(GL_TEXTURE_2D);
    }
	}
}


//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

void testPBuff::draw2()   //this one works
{
  static int currentPbuff = 0;

	for(int i=0; i<3; ++i){

      pbuff->MakeCurrent();
      {
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
        glPushMatrix();
        {
          glTranslatef(.5,.5,0);
          glMultMatrixf(gluvv.rinfo.xform);
          glColor3f(0,1,0);
          glutSolidTeapot(.3);
        } glPopMatrix();
        if(gluvv.tf.histOn){
          //cerr << "Copying" << endl;
          glBindTexture(GL_TEXTURE_2D, shadowName);
          glCopyTexSubImage2D(GL_TEXTURE_2D, 0, (int)0, (int)0, 
            (int)0, (int)0,
            (int)gluvv.light.buffsz[0], (int)gluvv.light.buffsz[1]);
        }
      } MakeCurrent();

      pbuff2->MakeCurrent();
      {
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
        glPushMatrix();
        {
          glTranslatef(.5,.5,0);
          glMultMatrixf(gluvv.rinfo.xform);
          glColor3f(1, 0, 0);
          glutSolidSphere(.3, 5, 5);
        } glPopMatrix();
        if(gluvv.tf.histOn){
          //cerr << "Copying" << endl;
          glBindTexture(GL_TEXTURE_2D, shadowName);
          glCopyTexSubImage2D(GL_TEXTURE_2D, 0, (int)0, (int)0, 
            (int)0, (int)0,
            (int)gluvv.light.buffsz[0], (int)gluvv.light.buffsz[1]);
        }
      } MakeCurrent();


		glActiveTexture(GL_TEXTURE5_ARB); {
			glEnable(GL_TEXTURE_2D);
				//cerr << "Binding p-buffer" << endl;
  				wglBindTexImageARB(pbuff->buffer, WGL_FRONT_LEFT_ARB);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}

    glActiveTexture(GL_TEXTURE4_ARB); {
			glEnable(GL_TEXTURE_2D);
				//cerr << "Binding p-buffer" << endl;
  				wglBindTexImageARB(pbuff2->buffer, WGL_FRONT_LEFT_ARB);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}

    glEnable(GL_FRAGMENT_SHADER_ATI);
		glBindFragmentShaderATI(fsname);

		glPushMatrix();
		{
			glTranslatef(gluvv.rinfo.trans[0], gluvv.rinfo.trans[1], gluvv.rinfo.trans[2]);
      glTranslatef(0,1,0);

      glBegin(GL_QUADS);
			{
				glMultiTexCoord2fARB(GL_TEXTURE5_ARB,0,0);
				glMultiTexCoord2fARB(GL_TEXTURE4_ARB,0,0);
				//glTexCoord2f(0,0);
				glVertex3f(-.5, -.5, 0);
				
				glMultiTexCoord2fARB(GL_TEXTURE5_ARB,0,1);
				glMultiTexCoord2fARB(GL_TEXTURE4_ARB,0,1);
				//glTexCoord2f(0,1);
				glVertex3f(-.5, 0, 0);
				
				glMultiTexCoord2fARB(GL_TEXTURE5_ARB,1,1);
				glMultiTexCoord2fARB(GL_TEXTURE4_ARB,1,1);
				//glTexCoord2f(1,1);
				glVertex3f(0, 0, 0);
				
				glMultiTexCoord2fARB(GL_TEXTURE5_ARB,1,0);
				glMultiTexCoord2fARB(GL_TEXTURE4_ARB,1,0);
				//glTexCoord2f(1,0);
				glVertex3f(0, -.5, 0);
			} glEnd();
		} glPopMatrix();

    glDisable(GL_FRAGMENT_SHADER_ATI);

		glActiveTexture(GL_TEXTURE5_ARB); {
  		wglReleaseTexImageARB(pbuff->buffer, WGL_FRONT_LEFT_ARB);
      glDisable(GL_TEXTURE_2D);
    }

		glActiveTexture(GL_TEXTURE4_ARB); {
      wglReleaseTexImageARB(pbuff2->buffer, WGL_FRONT_LEFT_ARB);
      glDisable(GL_TEXTURE_2D);
    }
  }
}

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

void testPBuff::draw3()    //this one does not work, it is messed up on so many levels
{


  pbuff->MakeCurrent();
      {
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

          glEnable(GL_DEPTH_TEST);
          glDepthMask(GL_FALSE);

        glPushMatrix();
        {
          glTranslatef(.5,.5,0);
          glMultMatrixf(gluvv.rinfo.xform);
          glColor3f(0,1,0);
          glutSolidTeapot(.3);
        } glPopMatrix();
      } MakeCurrent();


      pbuff2->MakeCurrent();
      {
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

        glDisable(GL_BLEND);

        //why doesn't this draw?
        glPushMatrix();
        {
          glTranslatef(.5,.5,0);
          glMultMatrixf(gluvv.rinfo.xform);
          glColor4f(1, 1, 1, 1);
          glutSolidSphere(.4, 5, 5);
        } glPopMatrix();

#if 1
        glActiveTexture(GL_TEXTURE1_ARB); {
          glEnable(GL_TEXTURE_2D);
          //cerr << "Binding p-buffer" << endl;
          wglBindTexImageARB(pbuff->buffer, WGL_FRONT_LEFT_ARB);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
        
        //glDisable(GL_CULL_FACE);
        //glDisable(GL_DEPTH_TEST);
        glEnable(GL_FRAGMENT_SHADER_ATI);
        glBindFragmentShaderATI(fsname1);

        //glPushMatrix();
        //{
          glBegin(GL_QUADS);
          {
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,0);
            glVertex3f(0, 0, 0);
            
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,1);
            glVertex3f(0, .5, 0);
            
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1,1);
            glVertex3f(.5, .5, 0);
            
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1,0);
            glVertex3f(.5, 0, 0);
          } glEnd();
        //} glPopMatrix();
        

        //glDisable(GL_FRAGMENT_SHADER_ATI);
        
        
        glActiveTexture(GL_TEXTURE1_ARB); {
          wglReleaseTexImageARB(pbuff->buffer, WGL_FRONT_LEFT_ARB);
          glDisable(GL_TEXTURE_2D);
        }
#endif
        
        //it draw's here just fine...

        glPushMatrix();
        {
          //glTranslatef(.5,.5,0);
          glMultMatrixf(gluvv.rinfo.xform);
          glColor3f(1, 0, 0);
          glutSolidSphere(.3, 5, 5);
        } glPopMatrix();
      } MakeCurrent();
     
      
#if 0
		glActiveTexture(GL_TEXTURE5_ARB); {
#if 0
      glEnable(GL_TEXTURE_2D);
				//cerr << "Binding p-buffer" << endl;
  				wglBindTexImageARB(pbuff->buffer, WGL_FRONT_LEFT_ARB);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#else
        glDisable(GL_TEXTURE_2D);
#endif
		}
#endif

    glActiveTexture(GL_TEXTURE4_ARB); {
			glEnable(GL_TEXTURE_2D);
				//cerr << "Binding p-buffer" << endl;
  			wglBindTexImageARB(pbuff2->buffer, WGL_FRONT_LEFT_ARB);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}

    glEnable(GL_FRAGMENT_SHADER_ATI);
		glBindFragmentShaderATI(fsname2);

		glPushMatrix();
		{
			glTranslatef(gluvv.rinfo.trans[0], gluvv.rinfo.trans[1], gluvv.rinfo.trans[2]);
      glTranslatef(0,1,0);

      glBegin(GL_QUADS);
			{
				//glMultiTexCoord2fARB(GL_TEXTURE5_ARB,0,0);
				glMultiTexCoord2fARB(GL_TEXTURE4_ARB,0,0);
				//glTexCoord2f(0,0);
				glVertex3f(-.5, -.5, 0);
				
				//glMultiTexCoord2fARB(GL_TEXTURE5_ARB,0,1);
				glMultiTexCoord2fARB(GL_TEXTURE4_ARB,0,1);
				//glTexCoord2f(0,1);
				glVertex3f(-.5, 0, 0);
				
				//glMultiTexCoord2fARB(GL_TEXTURE5_ARB,1,1);
				glMultiTexCoord2fARB(GL_TEXTURE4_ARB,1,1);
				//glTexCoord2f(1,1);
				glVertex3f(0, 0, 0);
				
				//glMultiTexCoord2fARB(GL_TEXTURE5_ARB,1,0);
				glMultiTexCoord2fARB(GL_TEXTURE4_ARB,1,0);
				//glTexCoord2f(1,0);
				glVertex3f(0, -.5, 0);
			} glEnd();
		} glPopMatrix();

    glDisable(GL_FRAGMENT_SHADER_ATI);
#if 0
		glActiveTexture(GL_TEXTURE5_ARB); {
  		wglReleaseTexImageARB(pbuff->buffer, WGL_FRONT_LEFT_ARB);
      glDisable(GL_TEXTURE_2D);
    }
#endif
		glActiveTexture(GL_TEXTURE4_ARB); {
      wglReleaseTexImageARB(pbuff2->buffer, WGL_FRONT_LEFT_ARB);
      glDisable(GL_TEXTURE_2D);
    }
}

//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================
//===================================================================================

void testPBuff::MakeCurrent()        //switch back to this context
{
	  glutSetWindow(gluvv.mainWindow);


}