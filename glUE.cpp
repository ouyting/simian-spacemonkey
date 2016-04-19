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
//
// glUE.cpp: implementation of the glUE class.
//
//////////////////////////////////////////////////////////////////////

#include "glUE.h"
#include "gluvv.h"
#include <iostream.h>
#include <string.h>

#ifdef WIN32
//3D texture
PFNGLTEXIMAGE3DPROC glTexImage3DEXT;

#ifdef GL_EXT_blend_minmax
PFNGLBLENDEQUATIONEXTPROC glBlendEquationEXT;
#endif

#ifdef GL_EXT_color_table
//palated texture
PFNGLCOLORTABLEEXTPROC  glColorTableEXT;
#endif

#ifdef GL_EXT_pixel_texture
GLPIXELTEXGENEXTFUNCPTR glPixelTexGenEXT;
#endif

//multi-texture
#ifdef GL_ARB_multitexture
PFNGLACTIVETEXTUREARBPROC       glActiveTexture;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTexture;
PFNGLMULTITEXCOORD1FARBPROC     glMultiTexCoord1fARB;
PFNGLMULTITEXCOORD1FVARBPROC    glMultiTexCoord1fvARB;
PFNGLMULTITEXCOORD2FARBPROC     glMultiTexCoord2fARB;
PFNGLMULTITEXCOORD2FVARBPROC    glMultiTexCoord2fvARB;
PFNGLMULTITEXCOORD3FARBPROC     glMultiTexCoord3fARB;
PFNGLMULTITEXCOORD3FVARBPROC    glMultiTexCoord3fvARB;
#endif

//Secondary color
#ifdef GL_EXT_secondary_color
PFNGLSECONDARYCOLOR3FEXTPROC   glSecondaryColor3fEXT;
#endif

//Register Combiners
#ifdef GL_NV_register_combiners
PFNGLCOMBINERPARAMETERFVNVPROC              glCombinerParameterfvNV; 
PFNGLCOMBINERPARAMETERFNVPROC               glCombinerParameterfNV;
PFNGLCOMBINERPARAMETERIVNVPROC              glCombinerParameterivNV;
PFNGLCOMBINERPARAMETERINVPROC               glCombinerParameteriNV;
PFNGLCOMBINERINPUTNVPROC                    glCombinerInputNV;
PFNGLCOMBINEROUTPUTNVPROC                   glCombinerOutputNV; 
PFNGLFINALCOMBINERINPUTNVPROC               glFinalCombinerInputNV;
PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC      glGetCombinerInputParameterfvNV;
PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC      glGetCombinerInputParameterivNV;
PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC     glGetCombinerOutputParameterfvNV;
PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC     glGetCombinerOutputParameterivNV;
PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV;
PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV;
#endif

//Fragment Shaders
#ifdef GL_ATI_fragment_shader
PFNGLGENFRAGMENTSHADERSATIPROC   glGenFragmentShadersATI;
PFNGLBINDFRAGMENTSHADERATIPROC   glBindFragmentShaderATI;
PFNGLDELETEFRAGMENTSHADERATIPROC glDeleteFragmentShaderATI;
PFNGLBEGINFRAGMENTSHADERATIPROC  glBeginFragmentShaderATI;
PFNGLENDFRAGMENTSHADERATIPROC    glEndFragmentShaderATI;
PFNGLPASSTEXCOORDATIPROC         glPassTexCoordATI;
PFNGLSAMPLEMAPATIPROC            glSampleMapATI;
PFNGLCOLORFRAGMENTOP1ATIPROC     glColorFragmentOp1ATI;
PFNGLCOLORFRAGMENTOP2ATIPROC     glColorFragmentOp2ATI;
PFNGLCOLORFRAGMENTOP3ATIPROC     glColorFragmentOp3ATI;
PFNGLALPHAFRAGMENTOP1ATIPROC     glAlphaFragmentOp1ATI;
PFNGLALPHAFRAGMENTOP2ATIPROC     glAlphaFragmentOp2ATI;
PFNGLALPHAFRAGMENTOP3ATIPROC     glAlphaFragmentOp3ATI;
PFNGLSETFRAGMENTSHADERCONSTANTATIPROC glSetFragmentShaderConstantATI;
#endif

//env-bump
#ifdef GL_ATIX_envmap_bumpmap
PFNGLTEXBUMPPARAMETERIVATIXPROC     glTexBumpParameterivATIX;
PFNGLTEXBUMPPARAMETERFVATIXPROC     glTexBumpParameterfvATIX;
PFNGLGETTEXBUMPPARAMETERIVATIXPROC  glGetTexBumpParameterivATIX;
PFNGLGETTEXBUMPPARAMETERFVATIXPROC  glGetTexBumpParameterfvATIX;
#endif

//wgl extensions ...
#ifdef WGL_ARB_pbuffer
PFNWGLCREATEPBUFFERARBPROC    wglCreatePbufferARB;
PFNWGLGETPBUFFERDCARBPROC     wglGetPbufferDCARB;
PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;
PFNWGLDESTROYPBUFFERARBPROC   wglDestroyPbufferARB;
PFNWGLQUERYPBUFFERARBPROC     wglQueryPbufferARB;
#endif
#ifdef WGL_ARB_pixel_format
PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB;
PFNWGLCHOOSEPIXELFORMATARBPROC      wglChoosePixelFormatARB;
#endif
#ifdef WGL_ARB_render_texture //render to texture
PFNWGLBINDTEXIMAGEARBPROC     wglBindTexImageARB;
PFNWGLRELEASETEXIMAGEARBPROC  wglReleaseTexImageARB;
PFNWGLSETPBUFFERATTRIBARBPROC wglSetBufferAttribARB;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void LoadAllExtensions()
{
	cerr << "-----------------------------------------------------" << endl;
	//-------------------------------------------------------------------
	//-------------------- 3d texture -----------------------------------
	//-------------------- Open GL spec 1.2 -----------------------------
	getOGL12();

	//-------------------------------------------------------------------
  //------------------- Register Combiners ----------------------------
	getNV20();

	//-------------------------------------------------------------------
  //------------------- ATI Radeon 8000 -------------------------------
	getATI8K();

	//-------------------------------------------------------------------
	//--------------------- wgl extensions ... ---------------------------
	getWGL();
	cerr << "-----------------------------------------------------" << endl << endl;


}

//=====================================================================
#endif

//=====================================================================
//-------------- standard utilities -----------------------------------
//=====================================================================

GLboolean QueryExtension(char *extName)
{
  /*
    ** Search for extName in the extensions string. Use of strstr()
    ** is not sufficient because extension names can be prefixes of
    ** other extension names. Could use strtok() but the constant
    ** string returned by glGetString might be in read-only memory.
    */
    char *p;
    char *end;
    int extNameLen;  

    extNameLen = strlen(extName);

    p = (char *)glGetString(GL_EXTENSIONS);
    if (0 == p) {
		cerr << "ERROR:: could not query extensions" << endl;
        return GL_FALSE;
    }

    end = p + strlen(p);
	
	if(!strcmp(extName, "Print")){
		for(int i=0; i< (int)strlen(p); ++i){
			cerr << p[i];
		} cerr << endl;
		return GL_FALSE;
	}

    while (p < end) {
        int n = strcspn(p, " ");
        if ((extNameLen == n) && (strncmp(extName, p, n) == 0)) {
            return GL_TRUE;
        }
        p += (n + 1);
    }
    return GL_FALSE;

}

//=====================================================================
int GlErr(char *location, char *glfuncname)
{
  GLenum errCode;
  const GLubyte *errString;
  
  if((errCode = glGetError()) != GL_NO_ERROR){
    errString = gluErrorString(errCode);
    cerr << "OpenGL ERROR : " << location << "::" << glfuncname << " : " 
	 << (char *) errString << endl;
    return 1;
  }
  return 0;
}

//=====================================================================
int CardProfile()
{

	if(QueryExtension("GL_ATI_fragment_shader")){
		cerr << "PLATFORM:  ATI Radeon 8000 series" << endl;
		return (int)GPATI8K;
	}
	if(QueryExtension("GL_NV_texture_shader")){
		cerr << "PLATFORM:  nVidia GeForce 3" << endl;
		return (int)GPNV20;
	}

	return 0;
}


//=====================================================================
//=====================================================================
//=====================================================================
//=====================================================================
//=====================================================================
#ifdef WIN32

void getOGL12()
{
	glTexImage3DEXT = 0;

	if(QueryExtension("GL_EXT_texture3D")){
		cerr << "3D Texture is available!" << endl;
		if(!(glTexImage3DEXT =
			(PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3DEXT"))){
			cerr << "*********************************" << endl;
			cerr << "wglGetProcAddress(\"glTexImage3DEXT\") failed!" << endl;
			cerr << "GetLastError(): " << GetLastError() << endl;
			cerr << "*********************************" << endl;
		}
	} else if((glTexImage3DEXT = (PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D")))
			cerr << "OGL1.2 3D texture is available" << endl;
	else cerr << "3D texture is NOT available" << endl;
	
	//-------------------------------------------------------------------
	//  blending 
#ifdef GL_EXT_blend_minmax
	glBlendEquationEXT = 0;
	if(!(glBlendEquationEXT =
			(PFNGLBLENDEQUATIONEXTPROC)wglGetProcAddress("glBlendEquationEXT"))){
			cerr << "*********************************" << endl;
			cerr << "wglGetProcAddress(\"glBlendEquationEXT\") failed!" << endl;
			cerr << "GetLastError(): " << GetLastError() << endl;
			cerr << "*********************************" << endl;
		}
#endif

	//-------------------------------------------------------------------
	//-------------------- palated textures -----------------------------
#ifdef GL_EXT_color_table
	glColorTableEXT = 0;
	if(QueryExtension("GL_EXT_color_table")){
		cerr << "Color Table is available" << endl;
		glColorTableEXT =
      (PFNGLCOLORTABLEEXTPROC)wglGetProcAddress("glColorTableEXT");
		if(!glColorTableEXT){
			cerr << "*********************************" << endl;
			cerr << "wglGetProcAddress(\"glColorTableEXT\") failed!" << endl;
			cerr << "GetLastError(): " << GetLastError() << endl;
			cerr << "*********************************" << endl;
		}
	} else cerr << "Color Table is not available" << endl;
#endif
	

	//HEY MOFO, THIS DOESN"T GO HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	//HEY MOFO, THIS DOESN"T GO HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
#ifdef GL_EXT_pixel_texture
	glPixelTexGenEXT = 0;
	if(QueryExtension("GL_EXT_pixel_texture")){
	  cerr << "Pixel texture is available" << endl;
	}
	else cerr << "Pixel Texture is NOT available" << endl;
#endif

	//-------------------------------------------------------------------
	//-------------------- Multi-texture --------------------------------
#ifdef GL_ARB_multitexture
	glActiveTexture       = 0;
	glClientActiveTexture = 0;
	glMultiTexCoord1fARB  = 0;
	glMultiTexCoord1fvARB = 0;
	glMultiTexCoord2fARB  = 0;
	glMultiTexCoord2fvARB = 0;
	glMultiTexCoord3fARB  = 0;
	glMultiTexCoord3fvARB = 0;

	if(QueryExtension("GL_ARB_multitexture")){
		cerr << "ARB multitexture is available!" << endl;
		glActiveTexture =
			(PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
		glClientActiveTexture =
			(PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");
		glMultiTexCoord1fARB =
			(PFNGLMULTITEXCOORD1FARBPROC)wglGetProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord1fvARB =
			(PFNGLMULTITEXCOORD1FVARBPROC)wglGetProcAddress("glMultiTexCoord1fvARB");
		glMultiTexCoord2fARB =
			(PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord2fvARB =
			(PFNGLMULTITEXCOORD2FVARBPROC)wglGetProcAddress("glMultiTexCoord2fvARB");
		glMultiTexCoord3fARB =
			(PFNGLMULTITEXCOORD3FARBPROC)wglGetProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord3fvARB =
			(PFNGLMULTITEXCOORD3FVARBPROC)wglGetProcAddress("glMultiTexCoord3fvARB");

		if ((!glActiveTexture)||(!glClientActiveTexture)||(!glMultiTexCoord1fARB)||
			(!glMultiTexCoord1fvARB)||(!glMultiTexCoord2fARB)||
			(!glMultiTexCoord2fvARB)||(!glMultiTexCoord3fARB)||
			(!glMultiTexCoord3fvARB)) {
			cerr << "*********************************" << endl;
			cerr << "wglGetProcAddress(\"glARBmultitexture\") failed!" << endl;
			cerr << "GetLastError(): " << GetLastError() << endl;
			cerr << "NOTE: You may need to get a new driver from ATI/NVIDIA" << endl;
			cerr << (int) glActiveTexture << " " << (int)glClientActiveTexture << endl;
			cerr << (int) glMultiTexCoord1fARB << " " << (int) glMultiTexCoord1fvARB << endl 
				 << (int) glMultiTexCoord2fARB << " " << (int) glMultiTexCoord2fvARB << endl
				 << (int) glMultiTexCoord3fARB << " " << (int) glMultiTexCoord3fvARB << endl;
			cerr << "*********************************" << endl;
			
		}
	} else cerr << "ARB multitexture texture is NOT available" << endl;
#endif
	//-------------------------------------------------------------------
	//------------------- GL_ARB_texture_env_combine --------------------
	if(QueryExtension("GL_ARB_texture_env_combine")){
		cerr << "GL_ARB_texture_env_combine is available!" << endl;
	} else cerr << "GL_ARB_texture_env_combine is NOT available" << endl;

	//-------------------------------------------------------------------
	//----------------- Secondary color ---------------------------------
#ifdef GL_EXT_secondary_color
	glSecondaryColor3fEXT = 0;
	if(QueryExtension("GL_EXT_secondary_color")){
		cerr << "Secondary Color is available!" << endl;
		glSecondaryColor3fEXT =
			(PFNGLSECONDARYCOLOR3FEXTPROC)wglGetProcAddress("glSecondaryColor3fEXT");
		if(!glSecondaryColor3fEXT){
			cerr << "Error aquiring Secondary Color function pointer" << endl;
		}
	} else {
		cerr << "Secondary Color is not available" << endl;
	}
#endif

}

//=====================================================================
//=====================================================================

void getNV20()
{
#ifdef GL_NV_register_combiners
	if(QueryExtension("GL_NV_register_combiners")){
		cerr << "GL_NV_register_combiners is available!" << endl;
		glCombinerParameterfvNV =
			(PFNGLCOMBINERPARAMETERFVNVPROC)wglGetProcAddress("glCombinerParameterfvNV"); 
		glCombinerParameterfNV =
			(PFNGLCOMBINERPARAMETERFNVPROC)wglGetProcAddress("glCombinerParameterfNV");
		glCombinerParameterivNV =
			(PFNGLCOMBINERPARAMETERIVNVPROC)wglGetProcAddress("glCombinerParameterivNV");
		glCombinerParameteriNV =
			(PFNGLCOMBINERPARAMETERINVPROC)wglGetProcAddress("glCombinerParameteriNV");
		glCombinerInputNV =
			(PFNGLCOMBINERINPUTNVPROC)wglGetProcAddress("glCombinerInputNV");
		glCombinerOutputNV =
			(PFNGLCOMBINEROUTPUTNVPROC)wglGetProcAddress("glCombinerOutputNV"); 
		glFinalCombinerInputNV =
			(PFNGLFINALCOMBINERINPUTNVPROC)wglGetProcAddress("glFinalCombinerInputNV");
		glGetCombinerInputParameterfvNV =
			(PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC)wglGetProcAddress("glGetCombinerInputParameterfvNV");
		glGetCombinerInputParameterivNV =
			(PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC)wglGetProcAddress("glGetCombinerInputParameterivNV");
		glGetCombinerOutputParameterfvNV =
			(PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC)wglGetProcAddress("glGetCombinerOutputParameterfvNV");
		glGetCombinerOutputParameterivNV=
			(PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC)wglGetProcAddress("glGetCombinerOutputParameterivNV");
		glGetFinalCombinerInputParameterfvNV =
			(PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC)wglGetProcAddress("glGetFinalCombinerInputParameterfvNV");
		glGetFinalCombinerInputParameterivNV =
			(PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC)wglGetProcAddress("glGetFinalCombinerInputParameterivNV");
		if(!glCombinerParameterfvNV){
			cerr << "*********************************" << endl;
			cerr << "wglGetProcAddress(\"GL_NV_register_combiners\") failed!" << endl;
			cerr << "GetLastError(): " << GetLastError() << endl;
			cerr << "*********************************" << endl;
		}
	} else cerr << "GL_NV_register_combiners is NOT available" << endl;

#endif
	//-------------------------------------------------------------------

}

//=====================================================================
//=====================================================================

void getATI8K()
{
#ifdef GL_ATIX_envmap_bumpmap
	//------------------- GL_ATIX_envmap_bumpmap ------------------------
	glTexBumpParameterivATIX = 0;
	glTexBumpParameterfvATIX = 0;
	glGetTexBumpParameterivATIX = 0;
	glGetTexBumpParameterivATIX = 0;
	
	if(QueryExtension("GL_ATIX_envmap_bumpmap")){
		cerr << "GL_ATIX_envmap_bumpmap is available!" << endl;
		glTexBumpParameterivATIX=
			(PFNGLTEXBUMPPARAMETERIVATIXPROC)wglGetProcAddress("glTexBumpParameterivATIX");
		glTexBumpParameterfvATIX=
			(PFNGLTEXBUMPPARAMETERFVATIXPROC)wglGetProcAddress("glTexBumpParameterfvATIX");
		glGetTexBumpParameterivATIX=
			(PFNGLGETTEXBUMPPARAMETERIVATIXPROC)wglGetProcAddress("glGetTexBumpParameterivATIX");
		glGetTexBumpParameterfvATIX=
			(PFNGLGETTEXBUMPPARAMETERFVATIXPROC)wglGetProcAddress("glGetTexBumpParameterfvATIX");
		
		if((!glTexBumpParameterivATIX)||(!glTexBumpParameterfvATIX)||
			(!glGetTexBumpParameterivATIX)||(!glGetTexBumpParameterfvATIX)){
			cerr << "*********************************" << endl;
			cerr << "wglGetProcAddress(\"GL_ATIX_envmap_bumpmap\") failed!" << endl;
			cerr << "GetLastError(): " << GetLastError() << endl;
			cerr << "NOTE: You may need to get a new driver from ATI/NVIDIA" << endl;
			cerr << (int)glTexBumpParameterivATIX  << " " << 	(int)glTexBumpParameterfvATIX << endl;
			cerr << (int)glGetTexBumpParameterivATIX << " " << (int)glGetTexBumpParameterivATIX << endl;
			cerr << "*********************************" << endl;
		} else { //see what the capabilites are
			int matsz;
			glGetTexBumpParameterivATIX(GL_BUMP_ROT_MATRIX_SIZE_ATIX, &matsz);
			cerr << "    -Bump rot matrix size = " << matsz << endl;
			int numtex;
			glGetTexBumpParameterivATIX(GL_BUMP_NUM_TEX_UNITS_ATIX, &numtex);
			cerr << "    -Number of available texture units = " << numtex << endl;
			int texunits[32];
			glGetTexBumpParameterivATIX(GL_BUMP_TEX_UNITS_ATIX, texunits);
			cerr << "    -Texture unit ids:" << endl;
			for(int i=0; i<numtex; ++i){
				if(texunits[i] == GL_TEXTURE0_ARB) cerr << "       GL_TEXTURE0_ARB" << endl;
				if(texunits[i] == GL_TEXTURE1_ARB) cerr << "       GL_TEXTURE1_ARB" << endl;
				if(texunits[i] == GL_TEXTURE2_ARB) cerr << "       GL_TEXTURE2_ARB" << endl;
				if(texunits[i] == GL_TEXTURE3_ARB) cerr << "       GL_TEXTURE3_ARB" << endl;
				if(texunits[i] == GL_TEXTURE4_ARB) cerr << "       GL_TEXTURE4_ARB" << endl;
				if(texunits[i] == GL_TEXTURE5_ARB) cerr << "       GL_TEXTURE5_ARB" << endl;
			}
		}
	} else  cerr << "GL_ATIX_envmap_bumpmap is NOT available!" << endl;
#endif //end radeon

#ifdef GL_ATI_fragment_shader
	//------------------- GL_ATI_fragment_shader ------------------------
	glGenFragmentShadersATI = 0;
	glBindFragmentShaderATI = 0;
	glDeleteFragmentShaderATI = 0;
	glBeginFragmentShaderATI = 0;
	glEndFragmentShaderATI = 0;
	glPassTexCoordATI = 0;
	glSampleMapATI = 0;
	glColorFragmentOp1ATI = 0;
	glColorFragmentOp2ATI = 0;
	glColorFragmentOp3ATI = 0;
	glAlphaFragmentOp1ATI = 0;
	glAlphaFragmentOp2ATI = 0;
	glAlphaFragmentOp3ATI = 0;
	glSetFragmentShaderConstantATI = 0;

	if(QueryExtension("GL_ATI_fragment_shader")){
		cerr << "GL_ATI_fragment_shader is available" << endl;
		glGenFragmentShadersATI =
			(PFNGLGENFRAGMENTSHADERSATIPROC)wglGetProcAddress("glGenFragmentShadersATI");
		glBindFragmentShaderATI =
			(PFNGLBINDFRAGMENTSHADERATIPROC) wglGetProcAddress("glBindFragmentShaderATI");
		glDeleteFragmentShaderATI=
			(PFNGLDELETEFRAGMENTSHADERATIPROC) wglGetProcAddress("glDeleteFragmentShaderATI");
		glBeginFragmentShaderATI =
			(PFNGLBEGINFRAGMENTSHADERATIPROC)  wglGetProcAddress("glBeginFragmentShaderATI");
		glEndFragmentShaderATI =
			(PFNGLENDFRAGMENTSHADERATIPROC)    wglGetProcAddress("glEndFragmentShaderATI");
		glPassTexCoordATI =
			(PFNGLPASSTEXCOORDATIPROC)         wglGetProcAddress("glPassTexCoordATI");
		glSampleMapATI =
			(PFNGLSAMPLEMAPATIPROC)            wglGetProcAddress("glSampleMapATI");
		glColorFragmentOp1ATI =
			(PFNGLCOLORFRAGMENTOP1ATIPROC)     wglGetProcAddress("glColorFragmentOp1ATI");
		glColorFragmentOp2ATI =
			(PFNGLCOLORFRAGMENTOP2ATIPROC)     wglGetProcAddress("glColorFragmentOp2ATI");
		glColorFragmentOp3ATI =
			(PFNGLCOLORFRAGMENTOP3ATIPROC)     wglGetProcAddress("glColorFragmentOp3ATI");
		glAlphaFragmentOp1ATI =
			(PFNGLALPHAFRAGMENTOP1ATIPROC)     wglGetProcAddress("glAlphaFragmentOp1ATI");
		glAlphaFragmentOp2ATI =
			(PFNGLALPHAFRAGMENTOP2ATIPROC)     wglGetProcAddress("glAlphaFragmentOp2ATI");
		glAlphaFragmentOp3ATI =
			(PFNGLALPHAFRAGMENTOP3ATIPROC)     wglGetProcAddress("glAlphaFragmentOp3ATI");
		glSetFragmentShaderConstantATI =
			(PFNGLSETFRAGMENTSHADERCONSTANTATIPROC) wglGetProcAddress("glSetFragmentShaderConstantATI");
	} else {
		cerr << "GL_ATI_fragment_shader is NOT available!" << endl;
	}

#endif

}

//=====================================================================
//=====================================================================

void getWGL()
{
#ifdef WGL_ARB_pbuffer
	if((wglCreatePbufferARB = 
		  (PFNWGLCREATEPBUFFERARBPROC)wglGetProcAddress("wglCreatePbufferARB"))){
		cerr << "Pbuffer is available!" << endl;	
	}
	else cerr << "Pbuffer is NOT available!" << endl;
	wglGetPbufferDCARB =
		(PFNWGLGETPBUFFERDCARBPROC)wglGetProcAddress("wglGetPbufferDCARB");
	wglReleasePbufferDCARB =
		(PFNWGLRELEASEPBUFFERDCARBPROC)wglGetProcAddress("wglReleasePbufferDCARB");
	wglDestroyPbufferARB =
		(PFNWGLDESTROYPBUFFERARBPROC)wglGetProcAddress("wglDestroyPbufferARB");
	wglQueryPbufferARB =
		(PFNWGLQUERYPBUFFERARBPROC)wglGetProcAddress("wglQueryPbufferARB");
#endif

#ifdef WGL_ARB_pixel_format
	if((wglGetPixelFormatAttribivARB=
		(PFNWGLGETPIXELFORMATATTRIBIVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribivARB"))){
		cerr << "WGL_ARB_pixel_format is available" << endl;
	} else cerr << "WGL_ARB_pixel_format is NOT available" << endl;
	wglGetPixelFormatAttribfvARB=
		(PFNWGLGETPIXELFORMATATTRIBFVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribfvARB");
	wglChoosePixelFormatARB=
		(PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
#endif

#ifdef WGL_ARB_render_texture //render to texture
	if((wglBindTexImageARB=
		(PFNWGLBINDTEXIMAGEARBPROC)wglGetProcAddress("wglBindTexImageARB"))){
		cerr << "WGL_ARB_render_texture is available" << endl;
	} else cerr << "WGL_ARB_render_texture is NOT available" << endl;
	wglReleaseTexImageARB=
		(PFNWGLRELEASETEXIMAGEARBPROC)wglGetProcAddress("wglReleaseTexImageARB");
	wglSetBufferAttribARB=
		(PFNWGLSETPBUFFERATTRIBARBPROC)wglGetProcAddress("wglSetBufferAttribARB");
#endif

}


#endif //ifdef WIN32