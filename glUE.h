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
//    GL Utills and Extensions v0.01(beta)
//
// glUE.h: interface for the glUE class.
//  The purpose of the "class" is to provide access to extensions 
//    without having to go through the hassle of aquiring them
//    in various places.  They are defined as "extern" so that a
//    single function pointer is kept around, ie. this file can
//    be included in multiple files.  The "glue" class should be 
//    created right after the gl window is created.  
//
//  NOTE: Many of the extensions defined by this header will require
//    "exparamental" drivers from the chip/card manufacurer.  If an
//    extension that you want is not detected, this is probably why.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLUE_H__F345B9AF_7BCA_4BAB_BC9E_949B280622AE__INCLUDED_)
#define AFX_GLUE_H__F345B9AF_7BCA_4BAB_BC9E_949B280622AE__INCLUDED_

//---------------------- Sort of lame card select --------------
// This is being phased out, do not use it
#define WILDCAT  0
#define RADEON   0
#define R200     0 
#define GEFORCE3 0
#define GEFORCE2 0
//--------------------------------------------------------------

#ifdef WIN32
#include <windows.h>		//  must be included before any OpenGL
#endif
#include <GL/glut.h>    //  glut includes gl.h
//#include <GL/gl.h>
#include <GL/glu.h>

#ifdef WIN32
//------- This include file is nVidia specific!!!-----------
#include <GL/glExt.h>

//------- This include file is windows specific!!!----------
#include <GL/wglext.h>

//------ This include file is ATI specific!!! --------------
#include <GL/glATI.h>
#include <GL/wglATI.h>

//==================================================================================
//------------------ Extension Definitions -----------------------------------------
//==================================================================================
// NOTE: To quickly test if an extension is available, simply test if the function 
//       pointer is non zero. ie:
//     if(<functionName>){ ...<functionName>(..); ...} else { ... do something else}
//
//   
//------------------ 3d texture ----------------------------------------------------
typedef void (APIENTRY * PFNGLTEXIMAGE3DPROC) (GLenum target, GLint level,
                                               GLint internalformat,
                                               GLsizei width, GLsizei height,
                                               GLsizei depth, GLint border,
                                               GLenum format, GLenum type,
                                               const GLvoid *pixels);
extern PFNGLTEXIMAGE3DPROC  glTexImage3DEXT;

#ifdef GL_EXT_blend_minmax
extern PFNGLBLENDEQUATIONEXTPROC glBlendEquationEXT;
#endif

//------------------ Palated texture -----------------------------------------------
#ifdef GL_EXT_color_table
typedef void (APIENTRY * PFNGLCOLORTABLEEXTPROC) (GLenum target,
                                                  GLenum internalformat,
                                                  GLsizei width,
                                                  GLenum format,
                                                  GLenum type,
                                                  const GLvoid *table);
extern PFNGLCOLORTABLEEXTPROC  glColorTableEXT;
#endif // end palated texture

//------------------ Multi-texture -------------------------------------------------
#ifdef GL_ARB_multitexture
extern PFNGLACTIVETEXTUREARBPROC       glActiveTexture;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTexture;
extern PFNGLMULTITEXCOORD1FARBPROC     glMultiTexCoord1fARB;
extern PFNGLMULTITEXCOORD1FVARBPROC    glMultiTexCoord1fvARB;
extern PFNGLMULTITEXCOORD2FARBPROC     glMultiTexCoord2fARB;
extern PFNGLMULTITEXCOORD2FVARBPROC    glMultiTexCoord2fvARB;
extern PFNGLMULTITEXCOORD3FARBPROC     glMultiTexCoord3fARB;
extern PFNGLMULTITEXCOORD3FVARBPROC    glMultiTexCoord3fvARB;
#endif //end multi texture

//----------------- Secondary color ------------------------------------------------
#ifdef GL_EXT_secondary_color
extern PFNGLSECONDARYCOLOR3FEXTPROC   glSecondaryColor3fEXT;
#endif

//------------------   nVidia Stuff ------------------------------------------------
//------------------ Register Combiners --------------------------------------------
#ifdef GL_NV_register_combiners
extern PFNGLCOMBINERPARAMETERFVNVPROC              glCombinerParameterfvNV; 
extern PFNGLCOMBINERPARAMETERFNVPROC               glCombinerParameterfNV;
extern PFNGLCOMBINERPARAMETERIVNVPROC              glCombinerParameterivNV;
extern PFNGLCOMBINERPARAMETERINVPROC               glCombinerParameteriNV;
extern PFNGLCOMBINERINPUTNVPROC                    glCombinerInputNV;
extern PFNGLCOMBINEROUTPUTNVPROC                   glCombinerOutputNV; 
extern PFNGLFINALCOMBINERINPUTNVPROC               glFinalCombinerInputNV;
extern PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC      glGetCombinerInputParameterfvNV;
extern PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC      glGetCombinerInputParameterivNV;
extern PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC     glGetCombinerOutputParameterfvNV;
extern PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC     glGetCombinerOutputParameterivNV;
extern PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV;
extern PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV;
#endif

//------------------      ATI Stuff ------------------------------------------------
//------------------ env-bump map/ pixel texture -----------------------------------
//   NOTE: This extension is only for ATI Radions, it can be used to achieve 
//          pixel texture as well as reflective bump mapping
//              but, fragment shaders are the bomb!!
#ifdef GL_ATIX_envmap_bumpmap
extern PFNGLTEXBUMPPARAMETERIVATIXPROC     glTexBumpParameterivATIX;
extern PFNGLTEXBUMPPARAMETERFVATIXPROC     glTexBumpParameterfvATIX;
extern PFNGLGETTEXBUMPPARAMETERIVATIXPROC  glGetTexBumpParameterivATIX;
extern PFNGLGETTEXBUMPPARAMETERFVATIXPROC  glGetTexBumpParameterfvATIX;
#endif //end EMBM

#ifdef GL_ATI_fragment_shader
extern PFNGLGENFRAGMENTSHADERSATIPROC   glGenFragmentShadersATI;
extern PFNGLBINDFRAGMENTSHADERATIPROC   glBindFragmentShaderATI;
extern PFNGLDELETEFRAGMENTSHADERATIPROC glDeleteFragmentShaderATI;
extern PFNGLBEGINFRAGMENTSHADERATIPROC  glBeginFragmentShaderATI;
extern PFNGLENDFRAGMENTSHADERATIPROC    glEndFragmentShaderATI;
extern PFNGLPASSTEXCOORDATIPROC         glPassTexCoordATI;
extern PFNGLSAMPLEMAPATIPROC            glSampleMapATI;
extern PFNGLCOLORFRAGMENTOP1ATIPROC     glColorFragmentOp1ATI;
extern PFNGLCOLORFRAGMENTOP2ATIPROC     glColorFragmentOp2ATI;
extern PFNGLCOLORFRAGMENTOP3ATIPROC     glColorFragmentOp3ATI;
extern PFNGLALPHAFRAGMENTOP1ATIPROC     glAlphaFragmentOp1ATI;
extern PFNGLALPHAFRAGMENTOP2ATIPROC     glAlphaFragmentOp2ATI;
extern PFNGLALPHAFRAGMENTOP3ATIPROC     glAlphaFragmentOp3ATI;
extern PFNGLSETFRAGMENTSHADERCONSTANTATIPROC glSetFragmentShaderConstantATI;
#endif


//------------------ WGL Extensions ------------------------------------------------
#ifdef WGL_ARB_pbuffer
extern PFNWGLCREATEPBUFFERARBPROC    wglCreatePbufferARB;
extern PFNWGLGETPBUFFERDCARBPROC     wglGetPbufferDCARB;
extern PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;
extern PFNWGLDESTROYPBUFFERARBPROC   wglDestroyPbufferARB;
extern PFNWGLQUERYPBUFFERARBPROC     wglQueryPbufferARB;
#endif 
#ifdef WGL_ARB_pixel_format
extern PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
extern PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC      wglChoosePixelFormatARB;
#endif
#ifdef WGL_ARB_render_texture //render to texture
extern PFNWGLBINDTEXIMAGEARBPROC     wglBindTexImageARB;
extern PFNWGLRELEASETEXIMAGEARBPROC  wglReleaseTexImageARB;
extern PFNWGLSETPBUFFERATTRIBARBPROC wglSetBufferAttribARB;
#endif


#endif // #ifdef WIN32


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================

//------------------ Capture and Report gl Errors ----------------------------------
//  NOTE: the calling convention for GLErr is: 
//            location = "Classname.function()" to identify where you made the call
//            glfuncname = "Name" of the gl Function which you are testing for errors
//        Returns 0 if no error, 1 if there was an error. 

int GlErr(char *location, char *glfuncname);

//------------------ Query extensions ----------------------------------------------
//  Returns true (!=0) if the extension defined by the extName string is available for 
//  the card.  If the extName string == "Print" all of the extensions available on the 
//  card are printed to standard out.

GLboolean QueryExtension(char *extName);

//------------------ Load all Extensions -------------------------------------------
//  Attempts to load all of the extensions for the defined card flag
//   Needs to be adapted for LINUX !!!

#ifdef WIN32
void LoadAllExtensions();
//----- get individual sets of extensions
void getOGL12();
void getNV20();
void getATI8K();
void getWGL();
#endif

//------------------ Card Profile --------------------------------------------------
//  Figure out what card we are using, by checking for unique extensions
//
//  returns an integer from the gluvv.plat enum (gluvvPlatform)

int CardProfile();


//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================

#endif // !defined(AFX_GLUE_H__F345B9AF_7BCA_4BAB_BC9E_949B280622AE__INCLUDED_)
