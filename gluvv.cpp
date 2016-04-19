//------------------------------------------------------------------------
//
//   Joe Kniss
//    A glut implementation of the Volume Renderer
//   gluvv.cpp "GL Utilites for Virtual reality Volume rendering"
//     3-4-01
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
//-------------------------------------------------------------------------

#include <iostream.h>
#include <string.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>		//  must be included before any OpenGL
#endif
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <GL/glut.h>       //glut includes gl and glu for you!

#include "Trackball.h"     //SGI quaternion trackball

#include "VectorMath.h"    //usefull vector/matrix operations

#include "gluvv.h"         //global definitions

#include "glUE.h"          //gl utilities and extensions

#include "TFWindow.h"      //transfer function window

#include "gluvvui.h"       //the user interface

//----------------------------------------------------------------
//----------- Renderable includes --------------------------------
#include "VolumeRenderable.h" //Old school renderer
#include "NV20VolRen.h"       //2D texture renderer
#include "NV20VolRen3D.h"     //standard renderer
#include "NV20VolRen3D2.h"   //shadow renderer
#include "TFWidgetRen1.h"     //transfer function renderer
#include "CPWidgetRen.h"      //clipping plane renderer
#include "DPWidgetRen.h"      //data probe renderer
#include "LTWidgetRen.h"      //light widget renderer
#include "TestRen.h"          //for testing stuff
#include "R8kVolRen3D.h"      //Radeon 8000 series volume renderer (1st render to texture)
#include "R8kVolRen3D_sav.h"      //Radeon 8000 series volume renderer (old shadow renderer)
#include "R8kVolRen3D_cpy.h"      //Radeon 8000 series volume renderer (new copy shadow)
#include "R8kVolRen3D_r2t.h"      //Radeon 8000 series volume renderer (new render to texture)
#include "testPBuff.h"        //test render to texture
#include "testPert.h"         //test texture perterbation

//----------------------------------------------------------------
//---- the global data structures --------------------------------
gluvvGlobal      gluvv;       //the global gluvv data structure (gluvv.h)
Trackball        tball;       //global trackball
Trackball        lball;       //for moving the light (now using light widget)
gluvvPrimitive   renderables; //all renderables attatch to this one
gluvvPrimitive   widgets; //all renderables attatch to this one
TFWindow         tfwin;       //separate transfer function window (not used??)
LTWidgetRen      ltwr;        //need the light widget to be accesable to other things
gluvvui          gui;         //the user interface

//----------------------------------------------------------------
//---- GLUT Stuff -------------------------------------------------
int     initGLUT(int argc, char **argv);  //initialize gl window
void    glutdisp();                                 //display callback
void    mouse(int button, int state, int x, int y); //mouse callback
void    motion(int x, int y);                       //motion callback
void    glutkey(unsigned char key, int x, int y);   //key callback
void    glutspecial(int key, int x, int y);         //special key callback
void    glutidle();                                 //idle callback
void    reshape(int w, int h);                      //reshape callback

//----------------------------------------------------------------
//---- GL Stuff ---------------------------------------------------
void    initGL();  //initialize gl state

//----------------------------------------------------------------
//---- Call Backs -------------------------------------------------
void    display(int eye);

//----------------------------------------------------------------
//---- Utils ------------------------------------------------------
void    initGluvv();                  //"Constructor for gluvv data structures"
void    initRenderables();            //this gets called after gl context extists
int     parse(int argc, char **argv); //returns 0 if no errors
void    gluvvQuit();                  //essentialy a destructor
int     pick();                       //pick objects
int     initData();                   //make sure data is in the right format

//---- Misc -------------------------------------------------------
int     checkExts();  //returns 1 if everything is available, 0 else
int     blurnorms;    //flag set if normals are to be blurred

//=================================================================
//=================================================================

//================================================= the Main func.
//===============================================================
int main(int argc, char **argv)
{
	//===== Initialization ========================
	//------------------------------------init gluvv
	initGluvv();

  //------------------------------------parse 
  if(parse(argc, argv)) return 1;
	
	//------------------------------------init data
	if(initData())
	{
		char c;
		cerr << "Data read failed.  Make sure you have the correct path specified" << endl
			<< "Hit any key to exit" << endl;
		cin >> c;
		return 1;
	}
	gluvv.mv->cacheTStep();


  //------------------------------------init glut
  initGLUT(argc, argv);

	if(!checkExts())
	{
		cerr << "Sorry, this platform is not supported" << endl;
		cerr << "  Please make sure you have a GeForce 3 or Radeon 8K with the correct driver" << endl;
		return 0;
	}

	//------------------------------------create renderables
	//------------------------------- create volume renderer
	if(WILDCAT){ //wildcat
		VolumeRenderable *volr = new VolumeRenderable;
		renderables.setNext((gluvvPrimitive*)volr);
	}

	else if(gluvv.plat == GPNV202D){ //GF3 with 2D textures only
		NV20VolRen *nv20vr = new NV20VolRen;
		renderables.setNext((gluvvPrimitive*)nv20vr);
	} 
	
	else if(gluvv.plat == GPNV20) { //standard renderer
		if(!gluvv.light.shadow){
			NV20VolRen3D *nv20vr = new NV20VolRen3D;
			renderables.setNext((gluvvPrimitive*)nv20vr);
		}
		else{ //create the shadow renderer
			NV20VolRen3D2 *nv20vr = new NV20VolRen3D2;
			renderables.setNext((gluvvPrimitive*)nv20vr);
		}
	}
		
	else if(gluvv.plat == GPATI8K){
		cerr << "creating the radeon volume renderer" << endl;
#if 1 //================================================================== vol ren

#if 1 //this is the advanced scatter shader //-------------------- r2t

#if 1 //++++++++++++++++++++++++++++++++++++++++++ old
    R8kVolRen3D * r8kvr = new R8kVolRen3D;
		renderables.setNext((gluvvPrimitive*)r8kvr);
#else //++++++++++++++++++++++++++++++++++++++++++ new
    R8kVolRen3D_r2t * r8kvr = new R8kVolRen3D_r2t;
		renderables.setNext((gluvvPrimitive*)r8kvr);
#endif //+++++++++++++++++++++++++++++++++++++++++

#else //this is the old shadow shader //-------------------------- copies

#if 1 //++++++++++++++++++++++++++++++++++++++++++ old
		R8kVolRen3D_sav * r8kvr = new R8kVolRen3D_sav;
		renderables.setNext((gluvvPrimitive*)r8kvr);
#else //++++++++++++++++++++++++++++++++++++++++++ new
		R8kVolRen3D_cpy * r8kvr = new R8kVolRen3D_cpy;
		renderables.setNext((gluvvPrimitive*)r8kvr);
#endif //+++++++++++++++++++++++++++++++++++++++++

#endif //---------------------------------------------------------

#else //================================================================== test

#if 1 //---------------------------------------------------------- pbuffers
    testPBuff * testpb = new testPBuff();
		renderables.setNext((gluvvPrimitive*)testpb);
#else //---------------------------------------------------------- perturbations
    testPert * testpt = new testPert();
    renderables.setNext((gluvvPrimitive*)testpt);
#endif //---------------------------------------------------------

#endif //=================================================================
	}

	//------------------------------- create widgets
#if 1
  TFWidgetRen *tfwr = new TFWidgetRen;
	renderables.setNext((gluvvPrimitive*)tfwr);
	//widgets.setNext((gluvvPrimitive*)tfwr);

	CPWidgetRen *cpwr = new CPWidgetRen;
	renderables.setNext((gluvvPrimitive*)cpwr);	
	//widgets.setNext((gluvvPrimitive*)cpwr);	

	DPWidgetRen *dpwr = new DPWidgetRen;
	renderables.setNext((gluvvPrimitive*)dpwr);
	//widgets.setNext((gluvvPrimitive*)dpwr);

	renderables.setNext((gluvvPrimitive*)&ltwr);
	//widgets.setNext((gluvvPrimitive*)&ltwr);
#endif

#if 0
	TestRen *testr = new TestRen();
	renderables.setNext((gluvvPrimitive*)testr);
#endif
	//------------------------------------init renderables
	// - always after init glut!!
	initRenderables();

  //------------------------------------start
  cerr << "Entering Main Loop\n";

	glutMainLoop();
	
	cerr << "Buh Bye!" << endl;
	return 0;
}
//=================================================================
//=================================================================

//=============================================== init Gluvv
//==========================================================
void    initGluvv(){                  //"Constructor for gluvv data structures"
	//-------------------- initialize global state ---------------
  gluvv.mv          = 0;
  gluvv.mv1         = 0;
  gluvv.mv2         = 0;
  gluvv.mv3         = 0;
	
	gluvv.debug       = 0;
	gluvv.dmode       = GDM_V1;

  renderables.setName("Dummy Node");
  gluvv.picking = 0;
	gluvv.mprobe  = 1;
	gluvv.shade   = gluvvShadeFaux;

	gluvv.reblend = GB_NONE;

  //------------------------------------window
  gluvv.win.width  = 512;     //size
  gluvv.win.height = 512;
  gluvv.win.xPos   = 100;     //position
  gluvv.win.yPos   = 100;
  //------------------------------------environment
  gluvv.env.eye[0]     = 0;   //eye
  gluvv.env.eye[1]     = 0;
  gluvv.env.eye[2]     = -7;
  gluvv.env.at[0]      = 0;   //at
  gluvv.env.at[1]      = 0;
  gluvv.env.at[2]      = 0;
  gluvv.env.up[0]      = 0;   //up
  gluvv.env.up[1]      = 1;
  gluvv.env.up[2]      = 0;
  gluvv.env.frustum[0] = -.2;  //left
  gluvv.env.frustum[1] = .2;   //right
  gluvv.env.frustum[2] = -.2;  //bottom
  gluvv.env.frustum[3] = .2;   //top
  gluvv.env.clip[0]    = 1;    //front
  gluvv.env.clip[1]    = 20;   //back
	gluvv.env.bgColor    = 0;    //white background
	//------------------------------------mouse
	gluvv.mouse.alt     = 0;
	gluvv.mouse.ctrl    = 0;
	gluvv.mouse.shift   = 0;
	//------------------------------------lights & lighting
  gluvv.light.pos[0] = gluvv.light.startpos[0] = 0;  
  gluvv.light.pos[1] = gluvv.light.startpos[1] = 0;
  gluvv.light.pos[2] = gluvv.light.startpos[2] = -5;
	gluvv.light.buffsz[0] = 1024; //shadow buffer size
	gluvv.light.buffsz[1] = 1024;
	gluvv.light.shadow    = 0;   //shadows off
	gluvv.light.softShadow= 1;   //soft shadows
	gluvv.light.showView  = 0;   //show the view from the light
	gluvv.light.sill      = 0;   //show silhouette edges
	gluvv.light.amb       = .05;  //shadow strenght
	gluvv.light.intens    = .75;
	gluvv.light.latt      = 0;
	gluvv.light.lattLimits[0] = .5;  //attinuation limits (start)
	gluvv.light.lattLimits[1] = -.5; //attinuation limits (finish)
	gluvv.light.lattThresh    = 1;   //amount of attinuation 
	gluvv.light.gShadowQual  = .5;    //good shadow quality
	gluvv.light.iShadowQual  = .20;  //interactive shadow quality
	gluvv.light.shadowTF     = 0;    //not using a second, shadow tf
	gluvv.light.showShadowTF = 0;    //don't show us the shadow tf
	gluvv.light.csz          = 32;   //size of the cube map
	gluvv.light.cubeKey[0]   = 0;   //x+ cube key not created yet
	gluvv.light.cubeKey[1]   = 0;   //x- cube key not created yet
	gluvv.light.cubeKey[2]   = 0;   //y+ cube key not created yet
	gluvv.light.cubeKey[3]   = 0;   //y- cube key not created yet
	gluvv.light.cubeKey[4]   = 0;   //z+ cube key not created yet
	gluvv.light.cubeKey[5]   = 0;   //z- cube key not created yet
	gluvv.light.load         = 1;   //load lighting info
	gluvv.light.gload        = 1;   //load the good lighting
	gluvv.light.fog          = 0;   //fog is off
	gluvv.light.fogThresh    = .5;  //fog blend threshold
	gluvv.light.fogLimits[0] = .5;  //near fog ramp (start)
	gluvv.light.fogLimits[1] = -.5; //far fog ramp (finish)
  //------------------------------------render info
  identityMatrix(gluvv.rinfo.xform);  //init the rotation
  gluvv.rinfo.scale       = 1;        //identity for scale
  gluvv.rinfo.trans[0]    = 0;        //no translation
  gluvv.rinfo.trans[1]    = 0;
  gluvv.rinfo.trans[2]    = 0;
	//------------------------------------volume rendering stuff
  gluvv.volren.interactSamp= .6;      //Interactive sample rate
  gluvv.volren.goodSamp    = 2.5;     //High quality sample ..
  gluvv.volren.sampleRate  = gluvv.volren.goodSamp; //samples per voxel
  gluvv.volren.shade       = 0;       //shade off (this flag no longer used)
	gluvv.volren.tlut        = 0;       //not used?? (1D tf, old school)
	gluvv.volren.deptex      = 0;       //1st & 2nd axies transfer function
	gluvv.volren.deptex2     = 0;       //3rd axis transfer function
  gluvv.volren.deptex3     = 0;       //scattering (auxilary 2D) transfer
  gluvv.volren.loadTLUT    = 0;       //no need to load the tf
	gluvv.volren.scaleAlphas = 1;       //scale alphas to the sample rate
	gluvv.volren.gamma       = 1;       //gamma identitiy
	gluvv.volren.timestep    = 0;       //current timestep
	gluvv.volren.usePostCNorm= 0;       //use pre-computed Normals to start with
	gluvv.volren.loadNorms   = 0;       //no need to reload normals
	//------------------------------------transfer function
	gluvv.tf.tfWindow        = 0;       //not in separate window
	gluvv.tf.tfwin           = &tfwin;  //if it was this would handle the window
	gluvv.tf.loadme          = 0;       //no need to load the tf
	gluvv.tf.paintme         = 0;       //nothing to paint into it
	gluvv.tf.dropme          = 0;       //not widgets to drop
	gluvv.tf.clearpaint      = 0;       //already cleared
	gluvv.tf.brushon         = 0;       //brush is off
	gluvv.tf.slider1         = 1;       //3rd axis slider high
	gluvv.tf.slider1hi       = 1;       //these are sliders for special cases
	gluvv.tf.slider1lo       = 0;
	gluvv.tf.slider2         = 1;
	gluvv.tf.histOn          = 1;       //histogram off
	gluvv.tf.ptexsz[0]       = 256;     //size of the transfer function axies
	gluvv.tf.ptexsz[1]       = 256;
	gluvv.tf.ptexsz[2]       = 1;       //3rd axis handled sparately
	gluvv.tf.numelts         = 4;       // RGBA colors
	//------------------------------------clip plane
	gluvv.clip.mousepnt[0] = 0;
	gluvv.clip.mousepnt[1] = 0;
	gluvv.clip.mousepnt[2] = 0;
	gluvv.clip.ortho       = 1;
	gluvv.clip.oaxis       = VolRenAxisXPos;
	//------------------------------------data probe
	gluvv.probe.brush = NoBrush;
	//------------------------------------trackball
  tball.clear();
	lball.clear();

	//------------------------------------misc
	blurnorms = 0;
}

//================================================ init GLUT
//==========================================================
int initGLUT(int argc, char **argv)
{
  char displaymode[256];
  //char displaymode[] = "1";
  if(gluvv.plat == GPOctane)
	{
    strcpy(displaymode, "rgba=8 depth");
  }
  else if(gluvv.plat == GPOctane2)
	{
    strcpy(displaymode, "rgba=8 double depth");
  }
  else if(gluvv.plat == GPInfinite)
	{
    strcpy(displaymode, "double rgba depth");
	}
  else
	{
    strcpy(displaymode, "double rgba depth");
  }

  glutInit(&argc, argv);
  glutInitDisplayString(displaymode);
  //glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(gluvv.win.width, gluvv.win.height);
  glutInitWindowPosition(gluvv.win.xPos, gluvv.win.yPos);
  gluvv.mainWindow =
		glutCreateWindow("GL Utilites for Virtual Reality Volume Rendering");
  
  //print out all of the extensions for the card!
	cerr << endl
			 << ">>>>>>>>>>>>> GL Extensions <<<<<<<<<<<<<<" << endl
			 << "------------------------------------------" << endl;
  QueryExtension("Print");
	cerr << "------------------------------------------" << endl << endl;
#ifdef WIN32
	LoadAllExtensions();
#endif

  // print out some window attributes for our (ribbed) pleasure. 
  {
    int r, g, b, a, ar, ag, ab, aa, d, s;
    GLboolean db,st, aux;
    
    glGetIntegerv(GL_RED_BITS,           &r);
    glGetIntegerv(GL_GREEN_BITS,         &g);
    glGetIntegerv(GL_BLUE_BITS,          &b);
    glGetIntegerv(GL_ALPHA_BITS,         &a);
    glGetIntegerv(GL_ACCUM_RED_BITS,     &ar);
    glGetIntegerv(GL_ACCUM_GREEN_BITS,   &ag);
    glGetIntegerv(GL_ACCUM_BLUE_BITS,    &ab);
    glGetIntegerv(GL_ACCUM_ALPHA_BITS,   &aa);
    glGetIntegerv(GL_DEPTH_BITS,         &d);
    glGetIntegerv(GL_STENCIL_BITS,       &s);
    glGetBooleanv(GL_DOUBLEBUFFER,       &db);
    glGetBooleanv(GL_STEREO,             &st);
    glGetBooleanv(GL_AUX_BUFFERS,        &aux);
    
    cerr << "Window Attributes" << endl;
    cerr << "-----------------" << endl;
    cerr << "color:    " << r + g + b + a << " bits";
    cerr << " (" << r << "," << g << "," << b << "," << a << ")" << endl;
    cerr << "accum:    " << ar+ag+ab+aa   << " bits";
    cerr << " (" << ar << "," << ag << "," << ab << "," << aa << ")" << endl;
    cerr << "depth:    " << d << " bits" << endl;
    cerr << "stencil:  " << s << " bits" << endl;
    cerr << "double:   "; 
    if(db) cerr << "YES" << endl;
    else   cerr << "NO" << endl;
    cerr << "aux:      ";
    if(aux) cerr << "YES" << endl;
    else    cerr << "NO" << endl;
    cerr << "stereo :  ";
    if(st) cerr << "YES" << endl;
    else   cerr << "NO" << endl;
    cerr << "-----------------" << endl;
  }
   
  initGL();
  
  glutDisplayFunc(glutdisp);
  GLUI_Master.set_glutReshapeFunc(reshape);
  GLUI_Master.set_glutMouseFunc(mouse);
  glutMotionFunc(motion);
  GLUI_Master.set_glutKeyboardFunc(glutkey);   
	GLUI_Master.set_glutSpecialFunc(glutspecial);
	GLUI_Master.set_glutIdleFunc(glutidle);
  return 0;
}
//=================================================================
//=================================================================

//=================================================== init GL
//===========================================================
void initGL( void )
{
  /* Black background and interpolated shading. */
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);

  /* Set up lighting. mostly for widgets */
  static float ambient[]             = {0.4, 0.4, 0.4, 4.0};
  static float diffuse[]             = {0.5, 1.0, 1.0, 1.0};
  static float front_mat_shininess[] = {60.0};
  static float front_mat_specular[]  = {0.2, 0.2,  0.2,  1.0};
  static float front_mat_diffuse[]   = {0.5, 0.28, 0.38, 1.0};
  static float lmodel_ambient[]      = {0.2, 0.2,  0.2,  1.0};
  static float lmodel_twoside[]      = {GL_FALSE};
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, gluvv.light.startpos);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,  lmodel_ambient);
  glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
  
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
  
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, front_mat_shininess);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  front_mat_specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   front_mat_diffuse);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDisable(GL_CULL_FACE);
  
	//glEnable(GL_LINE_SMOOTH);
	glLineWidth(3);

  GlErr("gluvv::","initGL()");

	gluvv.env.diff[0] = front_mat_diffuse[0];
	gluvv.env.diff[1] = front_mat_diffuse[1];
	gluvv.env.diff[2] = front_mat_diffuse[2];
	gluvv.env.diff[3] = front_mat_diffuse[3];
	gluvv.env.spec[0] = front_mat_specular[0];
	gluvv.env.spec[1] = front_mat_specular[1];
	gluvv.env.spec[2] = front_mat_specular[2];
	gluvv.env.spec[3] = front_mat_specular[3];

}
//=================================================================
//=================================================================

//========================================== init Renderables
//===========================================================
void initRenderables()
{
	gluvvPrimitive *tmp = renderables.getNext();
	while(tmp){
		tmp->init();
		tmp = tmp->getNext();
	}
}
//=================================================================
//=================================================================

//================================================= glut Disp
//===========================================================
void glutdisp()
{
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(gluvv.env.eye[0],  //eye
		         gluvv.env.eye[1], 
						 gluvv.env.eye[2],  
						 gluvv.env.at[0],   //at
						 gluvv.env.at[1], 
						 gluvv.env.at[2],  
						 gluvv.env.up[0], 
						 gluvv.env.up[1], 
						 gluvv.env.up[2]);          //up
   
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(gluvv.env.frustum[0], //left
						 gluvv.env.frustum[1], //right
						 gluvv.env.frustum[2], //top
						 gluvv.env.frustum[3], //bottom
						 gluvv.env.clip[0],    //front
						 gluvv.env.clip[1]);   //back
    
   glMatrixMode(GL_MODELVIEW);
   //glDrawBuffer(GL_FRONT);
   //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glDrawBuffer(GL_BACK);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 glDepthFunc(GL_LESS);
	 glDisable(GL_BLEND);

   display(1); // our local display function
   
   glFlush();
   //glutSwapBuffers();
   GlErr("gluvv:","glutdisp()");
}
//=================================================================
//=================================================================

//================================================== Display
//==========================================================
void display(int eye)
{

  glPushMatrix();{
#if 0 
    //build current transformation-------------
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
    //done with transformation-----------------
#endif

    //cycle through scene graph ---------------
    gluvvPrimitive *tmp = renderables.getNext();
    while(tmp){
      tmp->draw();
      tmp = tmp->getNext();
    }

#if 1
		if(gluvv.reblend == GB_UNDER){ //have to do this for front to back volumes
			tmp = renderables.getNext();
			while(tmp){
			 tmp->draw();
				tmp = tmp->getNext();
			}
#if 1
			if(gluvv.env.bgColor == 0){
				glDisable(GL_LIGHTING);
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);				
				gluvv.reblend = GB_NONE;
				glDisable(GL_DEPTH_TEST);
				glColor4f(1,1,1,1);
				glBegin(GL_QUADS);{
					glVertex3f(-10,-10,-2);
					glVertex3f(-10,10,-2);
					glVertex3f(10,10,-2);
					glVertex3f(10,-10,-2);
				} glEnd();
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_LIGHTING);
        glDisable(GL_BLEND);
			}
#endif
		}
#endif

		gluvv.reblend = GB_NONE;

  } glPopMatrix();


	glutSwapBuffers();
}
//=================================================================
//=================================================================

//================================================== Mouse
//========================================================
void mouse(int button, int state, int x, int y)
{
  gluvv.mouse.button = button;
  gluvv.mouse.state  = state;
	
	if(glutGetModifiers() & GLUT_ACTIVE_CTRL) gluvv.mouse.ctrl = 1;
	else gluvv.mouse.ctrl = 0;
	if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) gluvv.mouse.shift = 1;
	else gluvv.mouse.shift = 0;
	if(glutGetModifiers() & GLUT_ACTIVE_ALT) gluvv.mouse.alt = 1;
	else gluvv.mouse.alt = 0;


  if(state == GLUT_DOWN){  //try picking something
    gluvv.pick.pos[0] = x;
    gluvv.pick.pos[1] = gluvv.win.height - y;
    if(pick()){
      if(gluvv.pick.prim){
				if(gluvv.pick.prim->pick() &&
					gluvv.pick.prim->mouse(button, state, x, y))
					return;
      }
    }
  }

	//cerr << "hi" << endl;
  if(state == GLUT_UP){ //send pick events & reset title
    if(gluvv.pick.prim)
      gluvv.pick.prim->mouse(button, state, x, y);
    else{
      glutSetWindowTitle("Simian - Space Monkey 1.0");
    }
    gluvv.mouse.button = -1;
  }
	
	//standard button actions...
  switch(button){//////////////////////////////
  case GLUT_LEFT_BUTTON: //+++++++++++++++++++++++++++++++++
    switch(state){//)))))))))))))))))))
    case GLUT_UP:
      gluvv.volren.sampleRate = gluvv.volren.goodSamp;
      glutPostRedisplay();
      break;
    case GLUT_DOWN:
			if(gluvv.mouse.ctrl && gluvv.mouse.shift){
				glutSetWindowTitle("Rotate **LIGHT**");
				lball.start((2.0 * x - gluvv.win.width) / gluvv.win.width,
					(2.0 * y - gluvv.win.height) / gluvv.win.height);
				gluvv.mouse.pos[0] = gluvv.mouse.last[0] = x;
				gluvv.mouse.pos[1] = gluvv.mouse.last[1] = y;
				gluvv.mouse.ctrl = 1;
				gluvv.mouse.shift= 1;
			} else {
				glutSetWindowTitle("Rotate");
				tball.start((2.0 * x - gluvv.win.width) / gluvv.win.width,
					(2.0 * y - gluvv.win.height) / gluvv.win.height);
				gluvv.mouse.pos[0] = gluvv.mouse.last[0] = x;
				gluvv.mouse.pos[1] = gluvv.mouse.last[1] = y;
				gluvv.volren.sampleRate = gluvv.volren.interactSamp;
			}
      glutPostRedisplay();
      break;
    }//)))))))))))))))))))))))))))))))))
    break; //++++++++++++++++++++++++++++++++++++++++++++++
		case GLUT_RIGHT_BUTTON: //+++++++++++++++++++++++++++++
			if(gluvv.mouse.ctrl){ //simulate a middle button
				gluvv.mouse.button = GLUT_MIDDLE_BUTTON;
				switch(state){//))))))))))))))))))))))
				case GLUT_UP:
					gluvv.volren.sampleRate = gluvv.volren.goodSamp;
					glutPostRedisplay();
					break;
				case GLUT_DOWN:
					glutSetWindowTitle("Translate");
					gluvv.mouse.pos[0] = gluvv.mouse.last[0] = x;
					gluvv.mouse.pos[1] = gluvv.mouse.last[1] = y;
					gluvv.volren.sampleRate = gluvv.volren.interactSamp;
					glutPostRedisplay();
					break;
				}//))))))))))))))))))))))))))))))))))
			}
			else {
				switch(state){//))))))))))))))))))))
				case GLUT_UP:
					gluvv.volren.sampleRate = gluvv.volren.goodSamp;
					glutPostRedisplay();
					break;
				case GLUT_DOWN:
					glutSetWindowTitle("Zoom");
					gluvv.mouse.pos[0] = gluvv.mouse.last[0] = x;
					gluvv.mouse.pos[1] = gluvv.mouse.last[1] = y;
					gluvv.volren.sampleRate = gluvv.volren.interactSamp;
					glutPostRedisplay();
					break;
				}//))))))))))))))))))))))))))))))))
			}
			break;//++++++++++++++++++++++++++++++++++++++++++++
			case GLUT_MIDDLE_BUTTON://++++++++++++++++++++++++++
				switch(state){//)))))))))))))))))))))))
				case GLUT_UP:
					gluvv.volren.sampleRate = gluvv.volren.goodSamp;
					glutPostRedisplay();
					break;
				case GLUT_DOWN:
					glutSetWindowTitle("Translate");
					gluvv.mouse.pos[0] = gluvv.mouse.last[0] = x;
					gluvv.mouse.pos[1] = gluvv.mouse.last[1] = y;
					gluvv.volren.sampleRate = gluvv.volren.interactSamp;
					glutPostRedisplay();
					break;
				}//))))))))))))))))))))))))))))))))))))
				break;//+++++++++++++++++++++++++++++++++++++++++
				default:
					break;
  }////////////////////////////////////////////
}
//=================================================================
//=================================================================

//================================================== Motion
//=========================================================
void motion(int x, int y)
{
  float dx, dy;
	
  if(gluvv.pick.prim){ //send events to a picked object
    if(gluvv.pick.prim->move(x, y))
			return;
  }

  switch(gluvv.mouse.button){////////////////////
  case GLUT_LEFT_BUTTON: //++++++++++++++++++
    switch(gluvv.mouse.state){////////////
    case GLUT_DOWN:
			if(gluvv.mouse.ctrl && gluvv.mouse.shift){
				lball.update((2.0 * x - gluvv.win.width) /gluvv.win.width,
					(2.0 * y - gluvv.win.height) /gluvv.win.height);
				float lxf[16];
				lball.buildRotMatrix((float (*)[4])lxf);
				translateV3(gluvv.light.pos, lxf, gluvv.light.startpos);
				glLightfv(GL_LIGHT0, GL_POSITION, gluvv.light.pos);
			} else {
				tball.update((2.0 * x - gluvv.win.width) /gluvv.win.width,
					(2.0 * y - gluvv.win.height) /gluvv.win.height);
				tball.buildRotMatrix((float (*)[4])gluvv.rinfo.xform);
			}
			gluvv.mouse.last[0] = x;
			gluvv.mouse.last[1] = y;
      glutPostRedisplay();
      break;
    case GLUT_UP: 
      break;
    }/////////////////////////////////////
    break; //+++++++++++++++++++++++++++++++
		case GLUT_RIGHT_BUTTON://+++++++++++++++
			switch(gluvv.mouse.state){////////////
			case GLUT_DOWN:
				dx = (x - gluvv.mouse.last[0])/(float)gluvv.win.width;
				dy = (y - gluvv.mouse.last[1])/(float)gluvv.win.height;
				
				if(abs(dy) > abs(dx)){ //scale object
					gluvv.rinfo.scale += 5.0 * dy;
					if(gluvv.rinfo.scale < .05)
						gluvv.rinfo.scale = .05;
					if(gluvv.rinfo.scale > 10.0)
						gluvv.rinfo.scale = 10;
				} else { //zoom in
					gluvv.env.frustum[0] *= 1+dx;
					gluvv.env.frustum[1] *= 1+dx;
					gluvv.env.frustum[2] *= 1+dx;
					gluvv.env.frustum[3] *= 1+dx;
				}
				
				gluvv.mouse.last[0] = x;
				gluvv.mouse.last[1] = y;
				glutPostRedisplay();
				break;
			case GLUT_UP: 
				break;
			}/////////////////////////////////////
			break; //++++++++++++++++++++++++++++++
			case GLUT_MIDDLE_BUTTON://+++++++++++++++
				switch(gluvv.mouse.state){////////////
				case GLUT_DOWN:
					dx = (x - gluvv.mouse.last[0])/(float)gluvv.win.width;
					dy = (y - gluvv.mouse.last[1])/(float)gluvv.win.height;
					gluvv.rinfo.trans[0] += dx *
						(gluvv.env.frustum[1]-gluvv.env.frustum[0]) * 
						gluvv.env.eye[2]/gluvv.env.clip[0];
					gluvv.rinfo.trans[1] += dy *
						(gluvv.env.frustum[3]-gluvv.env.frustum[2]) * 
						gluvv.env.eye[2]/gluvv.env.clip[0];
					gluvv.mouse.last[0] = x;
					gluvv.mouse.last[1] = y;
					glutPostRedisplay();
					break;
				case GLUT_UP: 
					break;
				}/////////////////////////////////////
				break;//+++++++++++++++++++++++++++++++
				
				default:
					break;
  }//////////////////////////////////////////////
}
//=================================================================
//=================================================================

//================================================== GLUT key
//===========================================================
void glutkey(unsigned char key, int x, int y)
{
  if(key == 27){ //excape key
    gluvvQuit();
  }
  if(gluvv.pick.prim)
    if(gluvv.pick.prim->key(key, x, y)) 
      return;

  switch(key){ ///////////////////////////////////
  case 'm':
		if(gui.visible()) gui.hide();
		else gui.show();
		break;
	case ',': //decrease interactive sample rate
    gluvv.volren.interactSamp *= .95;
    cerr << "*Interactive Sample Rate = " 
			<< gluvv.volren.interactSamp << endl;
    break;
  case '.': //increase interactive sample rate
    gluvv.volren.interactSamp *= 1/.95;
    cerr << "*Interactive Sample Rate = " 
			<< gluvv.volren.interactSamp << endl;
    break;
  case '<': //decrease good sample rate
    gluvv.volren.goodSamp *= .95;
    cerr << "*High Quality Sample Rate = " 
			<< gluvv.volren.goodSamp << endl;
    break;
  case '>': //increase good sample rate
    gluvv.volren.goodSamp *= 1/.95;
    cerr << "*High Quality Sample Rate = " 
			<< gluvv.volren.goodSamp << endl;
    break;
#if 0
	case 'a':  //enable/disable alpha scale based on sample rate
		if(gluvv.volren.scaleAlphas) gluvv.volren.scaleAlphas = 0;
		else gluvv.volren.scaleAlphas = 1;
		gluvv.volren.loadTLUT = 1;
		glutPostRedisplay();
		break;
#endif
	case 'c': //enable/disable clipping plane
		if(gluvv.clip.on) gluvv.clip.on = 0;
		else gluvv.clip.on = 1;
		glutPostRedisplay();
		break;
	case 'k':
		gluvv.tf.brushon = 0;
		gluvv.tf.loadme = 1;
		glutPostRedisplay();
		break;
	case 'b': //enable/disable transfer function brush		
		if(gluvv.tf.brushon && (gluvv.probe.brush == EllipseBrush)) gluvv.tf.brushon = 0;
		else gluvv.tf.brushon = 1;
		gluvv.probe.brush = EllipseBrush;
		gluvv.tf.loadme = 1; 
		glutPostRedisplay();
		break;
	case 't': //automaticaly sized triangle brush
		if(gluvv.tf.brushon && (gluvv.probe.brush == TriangleBrush)) gluvv.tf.brushon = 0;
		else gluvv.tf.brushon = 1;
		gluvv.probe.slider = .5;
		gluvv.probe.brush = TriangleBrush;
		gluvv.tf.loadme = 1; 
		glutPostRedisplay();
		break;
	case 'a': //automaticly sized ellipse brush
		if(gluvv.tf.brushon && (gluvv.probe.brush == AutoEllipseBrush)) gluvv.tf.brushon = 0;
		else gluvv.tf.brushon = 1;
		gluvv.probe.slider = .5;
		gluvv.probe.brush = AutoEllipseBrush;
		gluvv.tf.loadme = 1; 
		glutPostRedisplay();
		break;
	case 'o': //auto one d brush
		if(gluvv.tf.brushon && (gluvv.probe.brush == AutoOneDBrush)) gluvv.tf.brushon = 0;
		else gluvv.tf.brushon = 1;
		gluvv.probe.slider = .5;
		gluvv.probe.brush = AutoOneDBrush;
		gluvv.tf.loadme = 1; 
		glutPostRedisplay();
		break;
	case 'O': //one d brush
		if(gluvv.tf.brushon && 
			((gluvv.probe.brush == AutoOneDBrush)||(gluvv.probe.brush = OneDBrush))) 
			gluvv.tf.brushon = 0;
		else gluvv.tf.brushon = 1;
		gluvv.probe.slider = .5;
		gluvv.probe.brush = OneDBrush;
		gluvv.tf.loadme = 1; 
		glutPostRedisplay();
		break;
	case 'h': //toggle histogram
		if(gluvv.tf.histOn) gluvv.tf.histOn = 0;
		else gluvv.tf.histOn = 1;
		glutPostRedisplay();
		break;
	case 's': //toggle shade modes
	case 'S':
		if(gluvv.shade == gluvvShadeAmb) gluvv.shade = gluvvShadeDiff;
		else if(gluvv.shade == gluvvShadeDiff) gluvv.shade = gluvvShadeDSpec;
		else if(gluvv.shade == gluvvShadeDSpec) gluvv.shade = gluvvShadeFaux;
		else if(gluvv.shade == gluvvShadeFaux) gluvv.shade = gluvvShadeAmb;
		gluvv.tf.loadme = 1;
		glutPostRedisplay();
		break;
	case ' ': //paint action
		gluvv.tf.paintme = 1;
		glutPostRedisplay();
		break;
	case 'd': //drop action
		gluvv.tf.dropme = 1;
		glutPostRedisplay();
		break;
	case 'n': //clear painted tf
		gluvv.tf.clearpaint = 1;
		glutPostRedisplay();
		break;
	case 'l': //toggle light widget on and off
		ltwr.onoff();
		glutPostRedisplay();
		break;
	case '-':
	case '_': //change the current timestep - down
		gluvv.volren.timestep -= 1;
		if(gluvv.volren.timestep < gluvv.mv->tstart){
			gluvv.volren.timestep = gluvv.mv->tstart;
			cerr << "We are at the first time step :" << gluvv.volren.timestep << endl;
		} else {
			cerr << "Current time step: " << gluvv.volren.timestep << endl;
			if(!gluvv.mv->swapTStep(gluvv.volren.timestep)){
				gluvv.mv->readAll(gluvv.volren.timestep);
				if(gluvv.mv1)
					gluvv.mv1->readAll(gluvv.volren.timestep);
				if(gluvv.mv2)
					gluvv.mv2->readAll(gluvv.volren.timestep);
				if(gluvv.mv3)
					gluvv.mv3->readAll(gluvv.volren.timestep);
				initData();
				gluvv.mv->cacheTStep();
			}
		}
		glutPostRedisplay();
		break;
	case '=':
	case '+': //change the current timesetp - up
		gluvv.volren.timestep += 1;
		if(gluvv.volren.timestep > gluvv.mv->tstop){
			gluvv.volren.timestep = gluvv.mv->tstop;				
			cerr << "We are at the last time step :" << gluvv.volren.timestep << endl;
		} else {
			cerr << "Current time step: " << gluvv.volren.timestep << endl;
			if(!gluvv.mv->swapTStep(gluvv.volren.timestep)){
				gluvv.mv->readAll(gluvv.volren.timestep);
				if(gluvv.mv1)
					gluvv.mv1->readAll(gluvv.volren.timestep);
				if(gluvv.mv2)
					gluvv.mv2->readAll(gluvv.volren.timestep);
				if(gluvv.mv3)
					gluvv.mv3->readAll(gluvv.volren.timestep);
				initData();
				gluvv.mv->cacheTStep();
			}
		}
		glutPostRedisplay();
		break;
	default:
    cerr << "Unused Key press: '" << (char)key << "' = " << (int) key 
			<< " at " << x << ", " << y << endl;
    break;
  } //////////////////////////////////////////////
}

void glutspecial(int key, int x, int y)
{
	if(gluvv.pick.prim)
    if(gluvv.pick.prim->special(key, x, y)) 
      return;

	switch(key){
  case GLUT_KEY_F1:  //open tf window
		if(!gluvv.tf.tfWindow){
			tfwin.create();
		} else {
			tfwin.destroy();
		}
		break;
	case GLUT_KEY_F9:  //ambiant shade mode
		gluvv.shade = gluvvShadeAmb;
		gluvv.tf.loadme = 1;
		glutPostRedisplay();
		break;
	case GLUT_KEY_F10: //shaded
		gluvv.shade = gluvvShadeDSpec;
		//gluvv.tf.loadme = 1;
		glutPostRedisplay();
		break;
	case GLUT_KEY_F11: //faux shaded
		gluvv.shade = gluvvShadeFaux;
		gluvv.tf.loadme = 1;
		glutPostRedisplay();
		break;
	case GLUT_KEY_F12: //????
		gluvv.shade = gluvvShadeDiff;
		//gluvv.tf.loadme = 1;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}
//=================================================================
//=================================================================

//================================================== GLUT idle
//============================================================
void glutidle()
{
  //glutPostRedisplay();
}

//================================================== Reshape
//==========================================================
void reshape(int w, int h)
{
	 cerr << "new window : " << w << ", " << h << endl;
   glViewport(0,0, (GLint) w -1, (GLint) h-1);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluvv.env.frustum[0] *= (float)w/(float)gluvv.win.width;
   gluvv.env.frustum[1] *= (float)w/(float)gluvv.win.width;
   gluvv.env.frustum[2] *= (float)h/(float)gluvv.win.height;
   gluvv.env.frustum[3] *= (float)h/(float)gluvv.win.height;

   glFrustum(gluvv.env.frustum[0], 
	     gluvv.env.frustum[1], 
	     gluvv.env.frustum[2], 
	     gluvv.env.frustum[3], 
	     gluvv.env.clip[0], 
	     gluvv.env.clip[1]);

   glMatrixMode(GL_MODELVIEW);

   gluvv.win.width  = w;
   gluvv.win.height = h;
}
//=================================================================
//=================================================================

//=================================================== Pick
//========================================================
//================================================== begin
//--- lesson in picking ---------------------------------------
// rules for gluvv picking say that first you push your 
//  'this' pointer (1 int for 32 bit, 2 for 64) 
//       -(must be a subclass of gluvvPrimitive!)
//  finaly push three (3) ints on the stack 
//       -(sub object names - anything you want)
// to recap: you push 4 ints (total) in 32 bit, and 5 (total)in 64
//--------------------------------------------------------------

int pick()
{
  gluvv.pick.data1  = 0;
  gluvv.pick.data2  = 0;
  gluvv.pick.name32 = 0;
  gluvv.pick.name64 = 0;
  gluvv.pick.z      = 0xffffffff;
  if(gluvv.pick.prim){
    gluvv.pick.prim->release();
    gluvv.pick.prim   = 0;
  }
	
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
	
  GLfloat projection[16];
  glGetFloatv(GL_PROJECTION_MATRIX, projection);
  
  int pickBufferSize = 128;
  GLuint *pickBuffer = new GLuint[pickBufferSize];
	
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  {
    glLoadIdentity();
    GLdouble pickbox = 4;
    gluPickMatrix((GLdouble)gluvv.pick.pos[0], (GLdouble)gluvv.pick.pos[1], 
			pickbox, pickbox, viewport);
    
    glMultMatrixf(projection);
    glMatrixMode(GL_MODELVIEW);
    glSelectBuffer(pickBufferSize, pickBuffer);
    glRenderMode(GL_SELECT);
		
    glInitNames();
    glPushName(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // ---- RENDER -------------------------------
    //cycle through scene graph ------------------
    gluvv.picking = 1;
    display(1);
    gluvv.picking = 0;
  }
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
	
  glFlush();
	
  // now see what we got!
  GLint hits = glRenderMode(GL_RENDER);
  //cerr << "hits = " << hits;
  GLuint nnames, z, minz = 0xffffffff;
  int gotone = 0;
	
  if(hits > 0){
    
    //i tracks the number of hits, j tracks postion in pick buffer
    int i, j; 
    for(i=0, j=0; (j<pickBufferSize && i<hits); i++){
      //first item on stack is the number of items
      nnames = pickBuffer[j++]; 
      z = pickBuffer[j++];      //second is the z value for this hit
      if(nnames > 1 && z <= minz){
				gotone = 1;
				minz= z;
				gluvv.pick.z = minz;
				
#if (_MIPS_SZPTR == 64) //64 bit
				j += nnames - 5;
				//reconstruct your 'this' pointer
				unsigned long o1 = pickBuffer[j++];
				unsigned long o2 = pickBuffer[j++];
				gluvv.pick.name64     = (o1<<32)|o2;
				gluvv.pick.data1      = pickBuffer[j++];
				gluvv.pick.data2      = pickBuffer[j++];
				gluvv.pick.data3      = pickBuffer[j++];
				gluvv.pick.prim       = (gluvvPrimitive*)gluvv.pick.name64;
#else
				j += nnames - 3;
				gluvv.pick.name32 = pickBuffer[j++];
				gluvv.pick.data1  = pickBuffer[j++];
				gluvv.pick.data2  = pickBuffer[j++];
				gluvv.pick.data3  = pickBuffer[j++];
				gluvv.pick.prim   = (gluvvPrimitive*)gluvv.pick.name32;
#endif
				//cerr << "here: " << gluvv.pick.name32  << " " << gluvv.pick.data1 
				 //  << " " << gluvv.pick.data2
				  // << " " << gluvv.pick.data3 << endl;
      }
      else{
				j += nnames + 1;
      }
    }
  }
	
  
  delete[] pickBuffer;
	
  if(gotone) return 1;
	
  return 0;
  
}
//=================================================== Pick
//========================================================
//==================================================== end

//======================================= check extensions
//========================================================
int checkExts()
{
	//use this to check if we can support the things we want to do
	// in the future this should check to see what render modes
	//  that we can use.
	// Many thanks to Nate Robins for testing this on his GF2!!!!!

	gluvv.plat = (gluvvPlatform) CardProfile();
	
	if(!glTexImage3DEXT)
		return 0;
	if(!glActiveTexture)
		return 0;
	//if(!glCombinerParameterfvNV)
		//return 0;

	return 1;
}
//=================================== end check extensions
//========================================================

//============================================== init Data
//========================================================
int initData()
{
	int ret = 0;

	cerr << "Initializing Data" << endl;
	switch(gluvv.dmode){
	case GDM_V1:
	case GDM_V1G:
	case GDM_V2:
	case GDM_V2G:
	case GDM_V3:
  case GDM_V3G:
	case GDM_V4:
		ret += gluvv.mv->mergeMV(blurnorms,1,0,gluvv.mv1, gluvv.mv2, gluvv.mv3);
		break;
	case GDM_V1GH:
	case GDM_V2GH:
		ret += gluvv.mv->mergeMV(blurnorms,1,1,gluvv.mv1, gluvv.mv2, gluvv.mv3);
		break;
	case GDM_VGH:
	case GDM_VGH_VG:
	case GDM_VGH_V:
		gluvv.mv->normalsVGH(blurnorms,1);
		break;
	}
	
	int bricksz = 128*128*256;

	switch(gluvv.dmode){
	case GDM_V1:
	case GDM_VGH_V:
		bricksz = 128*128*256;
		break;
	case GDM_V1G:
	case GDM_V2:
	case GDM_VGH_VG:
		bricksz = 128*128*256;
		break;
	default:
		bricksz = 128*128*256;
	}

	gluvv.mv->padOut();
	gluvv.mv->brick(bricksz);
	gluvv.mv->printInfo();

	cerr << " -done with data initialization" << endl;
	return ret;
}
//=================================================================
//=================================================================


//================================================== Parse
//========================================================
int parse(int argc, char **argv)
{
	//Yeah yeah, I should use lex and yack for this, so shoot me...
  if(argc < 3){
    cerr << argv[0] << "  Usage:\n";
    cerr << " File flags:\n";
    cerr << "     -t   [file name.trex] Trex file\n";
		cerr << "    <-t2  [file name.trex]> Optional 2nd Channel Trex file\n";
		cerr << "    <-t3  [file name.trex]> Optional 3rd Channel Trex file\n";
		cerr << "    <-t4  [file name.trex]> Optional 4th Channel Trex file\n";
		cerr << "    <-addG>                 Add a gradient measure to N channel data\n";
		cerr << "    <-addH>                 Add a hessian measure to N channel data\n";
		cerr << "     -n   [file name.nrrd]  Nrrd file\n";
		cerr << "     -vgh [file name.nrrd]  VGH Nrrd file\n";
		cerr << "    <-useVG>                Only use V&G from VGH Nrrd file (2B)\n";
		cerr << "    <-useV>                 Only use V from VGH Nrrd file (1B)\n";
		cerr << " Other options:\n";
		cerr << "    <-blurNormals>          Blurr normals\n";           
		//cerr << " Platform flags:\n";
		//cerr << "     -w       Intense3D Wildcat\n";
		//cerr << "     -o       SGI Octane\n";
    //cerr << "     -o2      SGI Octane2\n";
    //cerr << "     -i       SGI Infinite Reality\n";
    //cerr << "     -nv15    nVidia GeForce 2\n";
    //cerr << "     -nv20    nVidia GeForce 3\n (DEFAULT)";
		//cerr << "     -nv202D  nVidia GeForce 3 with 2D textures only\n";
		gluvvQuit();		
  }
  for(int i=0; i< argc; ++i){ //////////////////////// for
    //-------------- trex flags ------------------------------
		if(!strcmp(argv[i], "-t")){ //parse a trex file**********************************
      ++i;
      if(i>=argc){
				cerr << "parse Error: -t specified without a file name \n";
				return 1;
      }
			cerr << "  Loading TREX file " << argv[i] << endl;
      gluvv.mv = new MetaVolume(argv[i]);
			if(!gluvv.mv->readAll(gluvv.mv->tstart)) return 1;
			gluvv.volren.timestep = gluvv.mv->tstart;
    }
		else if(!strcmp(argv[i], "-t2")){ //parse a trex file****************************
      ++i;
      if(i>=argc){
				cerr << "parse Error: -t2 specified without a file name \n";
				return 1;
      }
			cerr << "  Loading TREX file " << argv[i] << endl;
      gluvv.mv1 = new MetaVolume(argv[i]);
			if(!gluvv.mv1->readAll(gluvv.mv1->tstart)) return 1;
			gluvv.dmode = GDM_V2;
    }
		else if(!strcmp(argv[i], "-t3")){ //parse a trex file****************************
      ++i;
      if(i>=argc){
				cerr << "parse Error: -t3 specified without a file name \n";
				return 1;
      }
			cerr << "  Loading TREX file " << argv[i] << endl;
      gluvv.mv2 = new MetaVolume(argv[i]);
			if(!gluvv.mv2->readAll(gluvv.mv2->tstart)) return 1;
			gluvv.dmode = GDM_V3;
    }
		else if(!strcmp(argv[i], "-t4")){ //parse a trex file****************************
      ++i;
      if(i>=argc){
				cerr << "parse Error: -t4 specified without a file name \n";
				return 1;
      }
			cerr << "  Loading TREX file " << argv[i] << endl;
      gluvv.mv3 = new MetaVolume(argv[i]);
			if(!gluvv.mv3->readAll(gluvv.mv3->tstart)) return 1;
			gluvv.dmode = GDM_V4;
    }
		else if(!strcmp(argv[i], "-addG")){ //add a gradient comp************************
			switch(gluvv.dmode){
			case GDM_V1:
				gluvv.dmode = GDM_V1G;
				break;
			case GDM_V2:
				gluvv.dmode = GDM_V2G;
				break;
			case GDM_V3:
				gluvv.dmode = GDM_V3G;
				break;
			default:
				cerr << "parse Error: you cannot add gradient measure to this data type" << endl;
				return 1;
			}
		}
		else if(!strcmp(argv[i], "-addH")){//add a hessian comp***************************
			switch(gluvv.dmode){
			case GDM_V1G:
				gluvv.dmode = GDM_V1GH;
				break;
			case GDM_V2G:
				gluvv.dmode = GDM_V2GH;
				break;
			default:
				cerr << "parse Error: you cannot add hessian measure to this data type" << endl;
				return 1;
			}
		}
		//----------------- Nrrd flags --------------------------
		else if(!strcmp(argv[i], "-n")){ //parse a nrrd file******************************
      ++i;
      if(i>=argc){
				cerr << "parse Error: -n specified without a file name \n";
				return 1;
      }
			cerr << "  Loading NRRD file " << argv[i] << endl;
      gluvv.mv = new MetaVolume(argv[i], 1);
			if(!gluvv.mv->readNrrd(gluvv.debug)) return 1;
			if(gluvv.mv->nelts == 1){
				gluvv.dmode = GDM_V1;
			} else if(gluvv.mv->nelts == 2){
				gluvv.dmode = GDM_V2;
			} else if(gluvv.mv->nelts == 3){
				gluvv.dmode = GDM_V3;
			} else if(gluvv.mv->nelts == 4){
				gluvv.dmode = GDM_V4;
			}
    }
		else if(!strcmp(argv[i], "-vgh")){ //parse an old school vgh file****************
      ++i;
      if(i>=argc){
				cerr << "parse Error: -vgh specified without a file name \n";
				return 1;
      }
			cerr << "  Loading NRRD file " << argv[i] << endl;
      gluvv.mv = new MetaVolume(argv[i], 1);
			if(!gluvv.mv->readNrrd(gluvv.debug)) return 1;
			if((gluvv.dmode != GDM_VGH_VG)&&(gluvv.dmode != GDM_VGH_V))
				gluvv.dmode = GDM_VGH;
			if(gluvv.mv->nelts == 1){  //dont let idiots load a vgh file that isnt one
				if(gluvv.dmode == GDM_VGH_V){
					gluvv.dmode = GDM_V1;
				}
				if(gluvv.dmode == GDM_VGH_VG){
					gluvv.dmode = GDM_V1G;
				}
				else{
					gluvv.dmode = GDM_V1GH;
				}
			} else if(gluvv.mv->nelts != 3){ //I cannot recover from this one, explode!!
				cerr << "parse Error: -vgh flag is for nrrd volume with pre-computed VGH"<< endl;
				return 1;
			}
    } 
		else if(!strcmp(argv[i], "-useVG")){ //only use VG from a VGH file*****
			gluvv.dmode = GDM_VGH_VG;
		}
		else if(!strcmp(argv[i], "-useV")){ //only use VG from a VGH file******
			gluvv.dmode = GDM_VGH_V;
		}
		//------------------ misc flags --------------------
		else if((!strcmp(argv[i],"-blurNormals"))||  //blur normals************
			      (!strcmp(argv[i],"-blurnormals"))){
			blurnorms = 1;			
		}
		else if((!strcmp(argv[i],"-shadow"))){
			gluvv.light.shadow = 1;
		}
		//------------------ platform flags --------------------
		else if(!strcmp(argv[i], "-w")){ //wildcat card************************
      gluvv.plat = GPWildcat;
    }
    else if(!strcmp(argv[i], "-o")){ //ocatain*****************************
      gluvv.plat = GPOctane;
    }
    else if(!strcmp(argv[i], "-o2")){//octain2*****************************
      gluvv.plat = GPOctane2;
    }
    else if(!strcmp(argv[i], "-i")){ //ir**********************************
      gluvv.plat = GPInfinite;
    }
    else if(!strcmp(argv[i], "-nv15")){ //Geforce2*************************
      gluvv.plat = GPNV15;
    }
    else if(!strcmp(argv[i], "-nv20")){ //Geforce3*************************
      gluvv.plat = GPNV20;
    }
    else if(!strcmp(argv[i], "-nv202D")){ //Geforce3 w/2D textures*********
      gluvv.plat = GPNV202D;
    }
		else if(!strcmp(argv[i], "-d")){ //Print some debug info***************
      gluvv.debug = 1;
    }
  }/////////////////////////////////////////////////// for
  return 0;
}
//=================================================================
//=================================================================

//================================================== gluvv Quit
//==== delete crap here =======================================
void gluvvQuit()
{
  cerr << endl;
  cerr << "cleaning up\n";
  
  cerr << "Exiting, by by\n";
  exit(0); //clean exit
}  
//=================================================================
//=================================================================
//=================================================================
//=================================================================
//=================================================================
//=================================================================
//=================================================================
//=================================================================
//=================================================================
//=================================================================

