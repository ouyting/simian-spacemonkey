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
// sampleRenderer.cpp: implementation of the sampleRenderer class.
//
//////////////////////////////////////////////////////////////////////

#include "sampleRenderer.h"
#include "glUE.h"
#include "VectorMath.h"
#include <iostream.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

sampleRenderer::sampleRenderer()
{

}

sampleRenderer::~sampleRenderer()
{

}

void sampleRenderer::draw()
{

	glPushMatrix();{
		//build transfor from global transform state
		glTranslatef(gluvv.rinfo.trans[0],
			gluvv.rinfo.trans[1],
			gluvv.rinfo.trans[2]);
    glMultMatrixf(gluvv.rinfo.xform);
    float scale[16];
    identityMatrix(scale);
    scale[0]  = gluvv.rinfo.scale;
    scale[5]  = gluvv.rinfo.scale;
    scale[10] = gluvv.rinfo.scale;
    glMultMatrixf(scale);

		//now draw some stuff, just to make sure it works
		glColor4f(1,1,1,1);
		glBegin(GL_QUADS);
		{
			glVertex3f(-.5, -.5, 0);
			glVertex3f(-.5, .5 , 0);
			glVertex3f(.5 , .5 , 0);
			glVertex3f(.5 , -.5, 0);
			
		}
		glEnd();
		
		glColor4f(1,0,0,1);
		glBegin(GL_TRIANGLES);
		{
			glVertex3f(0,0,.5);
			glVertex3f(1,0,.5);
			glVertex3f(1,1,0);
		}
		glEnd();
	}
	glPopMatrix();


}