// TestRen.cpp: implementation of the TestRen class.
//
//////////////////////////////////////////////////////////////////////

#include "TestRen.h"
#include <GL/glut.h>
#include "gluvv.h"
#include "glUE.h"
#include <iostream.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TestRen::TestRen()
{
	fs1name = -1;
}

TestRen::~TestRen()
{

}

void TestRen::init()
{
	int regs;
	glGetIntegerv(GL_NUM_FRAGMENT_REGISTERS_ATI, &regs);
	cerr << "number of registers = " << regs << endl;

	int cons;
	glGetIntegerv(GL_NUM_FRAGMENT_CONSTANTS_ATI, &cons);
	cerr << "number of constants = " << cons << endl;

	int pass;
	glGetIntegerv(GL_NUM_PASSES_ATI, &pass);
	cerr << "number of passes = " << pass << endl;

	int ipp; //instructions per pass
	glGetIntegerv(GL_NUM_INSTRUCTIONS_PER_PASS_ATI, &ipp);
	cerr << "number instructions per pass = " << ipp << endl;

	int ti;
	glGetIntegerv(GL_NUM_INSTRUCTIONS_TOTAL_ATI, &ti);
	cerr << "number of instructions total = " << ti << endl;

	GLboolean pair;
  glGetBooleanv(GL_COLOR_ALPHA_PAIRING_ATI, &pair);
	if(pair)
		cerr << "Color Alpha pairing enabled" << endl;
	else 
		cerr << "Color Alpha pairing disabled" << endl;

	//my r8k has 6 registers, 8 constants, 2 passes, 8 instructions per pass, 16 total

	tex1 = new unsigned char[128*128*128*4];
	glGenTextures(1, &t1name);
	for(int i=0; i<128; ++i){
		for(int j=0; j<128; ++j){
			for(int k=0; k<128; ++k){
				tex1[i*128*128*4 + j*128*4 + k*4 + 0] = k/127.0 * 255;
				tex1[i*128*128*4 + j*128*4 + k*4 + 1] = j/127.0 * 255;
				tex1[i*128*128*4 + j*128*4 + k*4 + 2] = i/127.0 * 255;
				tex1[i*128*128*4 + j*128*4 + k*4 + 3] = 85;
			}
		}
	}

		cerr << " Down Loading Data Texture 1" << t1name << " ... ";
		glEnable(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, t1name);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		glTexImage3DEXT(GL_TEXTURE_3D,
			0,
			GL_RGBA8,
			128,
			128,
			128,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			tex1);
		
		glFlush();
		GlErr("NV20VolRen3D", "Create Texture - Data Texture");	
		cerr << "Done" << endl;	

	tex2 = new unsigned char[128*128*128*4];
	glGenTextures(1, &t2name);
	for(i=0; i<128; ++i){
		for(int j=0; j<128; ++j){
			for(int k=0; k<128; ++k){
				tex2[i*128*128*4 + j*128*4 + k*4 + 0] = 0;//k/127.0 * 255;
				tex2[i*128*128*4 + j*128*4 + k*4 + 1] = 0;//j/127.0 * 255;
				tex2[i*128*128*4 + j*128*4 + k*4 + 2] = 255;//i/127.0 * 255;
				tex2[i*128*128*4 + j*128*4 + k*4 + 3] = 85;
			}
		}
	}

		cerr << " Down Loading Data Texture 2 " << t1name << " ... ";
		glEnable(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, t2name);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		glTexImage3DEXT(GL_TEXTURE_3D,
			0,
			GL_RGBA8,
			128,
			128,
			128,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			tex2);
		
		glFlush();
		GlErr("NV20VolRen3D", "Create Texture - Data Texture");	
		cerr << "Done" << endl;	

		glDisable(GL_TEXTURE_3D);

	fragShade1();
	
}

void TestRen::draw()
{
	//fragShade1();

	glPushMatrix();{

		glTranslatef(gluvv.rinfo.trans[0], gluvv.rinfo.trans[1], gluvv.rinfo.trans[2]);
		glMultMatrixf(gluvv.rinfo.xform);
		glTranslatef(-.5, -.5, -.5);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glActiveTexture(GL_TEXTURE0_ARB);{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glBindTexture(GL_TEXTURE_3D, t1name);
			glEnable(GL_TEXTURE_3D);
		}
		glActiveTexture(GL_TEXTURE1_ARB);{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glBindTexture(GL_TEXTURE_3D, t2name);
			glEnable(GL_TEXTURE_3D);
		}

		glEnable(GL_FRAGMENT_SHADER_ATI);{
			glBindFragmentShaderATI(fs1name);

			//glActiveTexture(GL_TEXTURE0_ARB);{
				//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NONE);
				//glEnable(GL_TEXTURE_3D);
				//glBindTexture(GL_TEXTURE_3D, t1name);
			//}

		  glActiveTexture(GL_TEXTURE1_ARB);{
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glBindTexture(GL_TEXTURE_3D, t2name);
				glEnable(GL_TEXTURE_3D);
			}
			glColor4f(1,0,0,.5);
			glBegin(GL_QUADS);{
				glMultiTexCoord3fARB(GL_TEXTURE0_ARB,0,0,.5);
				glVertex3f(0,0,0);
				glMultiTexCoord3fARB(GL_TEXTURE0_ARB,0,1,.5);
				glVertex3f(0,1,0);
				glMultiTexCoord3fARB(GL_TEXTURE0_ARB,1,1,.5);
				glVertex3f(1,1,0);
				glMultiTexCoord3fARB(GL_TEXTURE0_ARB,1,0,.5);
				glVertex3f(1,0,0);
			} glEnd();

			glActiveTexture(GL_TEXTURE1_ARB);{
				glDisable(GL_TEXTURE_3D);
			}
			glActiveTexture(GL_TEXTURE0_ARB);{
				glDisable(GL_TEXTURE_3D);
			}

		} glDisable(GL_FRAGMENT_SHADER_ATI);
		
			GlErr("TestRen::draw()", "end fragment shader 1");
		
		glColor4f(0,1,0,.5);
		glBegin(GL_QUADS);{
			glVertex3f(0.5,0.5,0);
			glVertex3f(0.5,1.5,0);
			glVertex3f(1.5,1.5,0);
			glVertex3f(1.5,0.5,0);
		} glEnd();
		
		glDisable(GL_BLEND);
	} glPopMatrix();
}

void TestRen::fragShade1()
{
	cerr << "building the fragment shader" << endl;
	glEnable(GL_FRAGMENT_SHADER_ATI);{
		if(fs1name == -1)
			fs1name = glGenFragmentShadersATI(1);
		cerr << "fs1name = " << fs1name << endl;
		glBindFragmentShaderATI(fs1name);

		glBeginFragmentShaderATI();{
			//----- routing instructions ------
			//grab the tex coords for tex 0
			glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
			//glPassTexCoordATI(GL_REG_1_ATI, GL_TEXTURE1_ARB, GL_SWIZZLE_STR_ATI);
			//----- end routing instructions --
		
			//----- math ----------------------
			glColorFragmentOp2ATI(GL_MUL_ATI, 
				                    GL_REG_0_ATI, GL_NONE, GL_SATURATE_BIT_ATI,
				                    GL_REG_0_ATI, GL_NONE, GL_NONE,
														GL_ONE,       GL_NONE, GL_NONE);
			glAlphaFragmentOp2ATI(GL_MUL_ATI, 
				                    GL_REG_0_ATI, GL_2X_BIT_ATI|GL_SATURATE_BIT_ATI,
				                    GL_REG_0_ATI, GL_NONE, GL_NONE,
														GL_ONE,       GL_NONE, GL_NONE);
			//----- end math ------------------
#if 1
			//----- routing instructions ------
			//grab the tex coords for tex 1 from tex0
			glSampleMapATI(GL_REG_1_ATI, GL_REG_0_ATI, GL_SWIZZLE_STR_ATI);
			//glPassTexCoordATI(GL_REG_0_ATI, GL_REG_0_ATI, GL_SWIZZLE_STRQ_ATI);
			//----- end routing instructions --
			
			//----- math ----------------------
			glColorFragmentOp2ATI(GL_MUL_ATI, 
				                    GL_REG_0_ATI, GL_NONE, GL_SATURATE_BIT_ATI,
				                    GL_REG_1_ATI, GL_NONE, GL_NONE,
														GL_ONE,       GL_NONE, GL_NONE);
			glAlphaFragmentOp2ATI(GL_MUL_ATI, 
				                    GL_REG_0_ATI, GL_SATURATE_BIT_ATI,
				                    GL_REG_1_ATI, GL_NONE, GL_NONE,
														GL_ONE,       GL_NONE, GL_NONE);
			//----- end math ------------------
#endif
		} glEndFragmentShaderATI();
	} glDisable(GL_FRAGMENT_SHADER_ATI);
	GlErr("TestRen::draw()", "build fragment shader 1");

}

