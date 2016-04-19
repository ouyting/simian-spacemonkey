//------------------------------------------------------------------------
//
//   Joe Kniss
//    A glut implementation of the Volume Renderer
//   gluvv.h "GL Utilites for Virtual reality Volume rendering"
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
//                    << jmk@cs.utah.edu >>
//               "All Your Base are Belong to Us"
//-------------------------------------------------------------------------

#ifndef __GLUVV_DOT_H
#define __GLUVV_DOT_H

#include "gluvvPrimitive.h"
#include "MetaVolume.h"
#include "TLUT.h"
#include "TFWindow.h"
#include "LTWidgetRen.h"

struct gluvvWindow{  //Window size and location attributes
  unsigned int          width;  
  unsigned int          height;
  unsigned int          xPos;
  unsigned int          yPos;   //screen pos
};

struct gluvvEnv{     //GL rendering Environment parameters
  float                 eye[3];    // eye location
  float                 at[3];     // lookat point
  float                 up[3];     // up vector
                                   // frustum[0] = left,   frustum[1] = right
  float                 frustum[4];// frustum[2] = bottom, frustum[3] = top
  float                 clip[2];   // clip[0] = front, clip[1] = back
	float                 diff[4];   // diffuse color (material)
	float                 spec[4];   // speculare color (material)
	int                   bgColor;   // what color is the back ground? 0==white 1==black
};

struct gluvvLight{   //lighting information
  float                 pos[3];    //light position
	float                 startpos[3];
  float                 color[3];  //light color
	float                 amb;       //(ambient contribution)
	float                 intens;
	float                 mv[16];    //the light model view
	float                 pj[16];    //the light projection
	float                 xf[16];    //pj*mv
	int                   buffsz[2]; //x&y sizes of shadow buffer
	int                   shadow;    //are shadows enabled??
	int                   softShadow;//are soft shadows enabled??
	int                   showView;  //show the view from the light
	int                   shadowTF;  //use a separate shadow transfer function
	int                   showShadowTF;  //render the shadow tf from your view
	int                   sill;      //do silhouette edges
	float                 gShadowQual; //good shadow quality
	float                 iShadowQual; //interactive shadow quality
	unsigned int          cubeName;    //cube map name
	float                 *cubeKey[6]; //decode the cubemap normals
	int                   csz;         //dimension of one axis of the cube map
	int                   load;        //reload the lighting info
	int                   gload;       //this is the good load, not interacting anymore
	int                   fog;         //is fog on?
	float                 fogColor[3]; //fog color
	float                 fogThresh;   //fog blend threshold
	float                 fogLimits[2];//limits of fog calculations
	int                   latt;        //light attinuation is on?
	float                 lattThresh;   //attinuation threshold
	float                 lattLimits[2];//limits of attinuation calculations
};

extern LTWidgetRen      ltwr; //the lighting widget

struct gluvvRInfo{   //rendering transform parameters
  float                 xform[16];    //rotation from trackball
  float                 scale;        //isotropic scale value
  float                 trans[3];     //translation
};

struct gluvvMouse{   //mouse state information
  int                   button;  //GLUT mouse button
  int                   state;   //GLUT mouse state
  int                   pos[2];  //x,y
  int                   last[2]; //last mouse position
	int                   shift;   //is shift down
	int                   ctrl;    //is control down
	int                   alt;     //is alt down
};

struct gluvvKey{     //key board state
  unsigned char         key;     //GLUT key
  int                   shift;   //is shift down
  int                   ctrl;    //is control down
  int                   alt;     //is alt down
};

struct gluvvPick{    //picking information
  int                   name32;   //name for 32 bit compiles
  int                   name64;   //name for 64 bit compiles
  int                   data1;    //data pushed on with name
  int                   data2;    // ''
  int                   data3;    // ''
  int                   pos[2];   //screen space coords
  unsigned int          z;        //depth of pick
  gluvvPrimitive       *prim;     //primitive pointer (same as name*)
};

struct gluvvVolRen{  //volume rendering paramters
	float                 sampleRate;   //current sample rate for rendering
  float                 interactSamp; //interactive sample rate
  float                 goodSamp;     //high quality sample rate
  int                   shade;        //render the volume shaded
	TLUT                 *tlut;         //texture lookup table
	unsigned char        *deptex;       //2d dependent texture 256x256xRGBA [gb]
	unsigned int          deptexName;   // '' name [gb]
	unsigned char        *deptex2;      //2d dependent texture ... [ar]
	unsigned int          deptex2Name;  // '' name [ar]
	unsigned char        *deptex3;      //2d dependent texture for shadows
	unsigned int          deptex3Name;  // '' name shadows
	int                   loadTLUT;     //reload the LUT if flag == 1
	int                   scaleAlphas;  //scale alphas with sample rate
	float                 gamma;        //constant alpha scale
	int                   timestep;     //what timestep are we on?
	int                   usePostCNorm; //using post classification normals???
	int                   loadNorms;    //reload the normals
};

typedef enum {  //Major axies of volume coords
	VolRenAxisUnknown,
    VolRenAxisXPos,
    VolRenAxisXNeg,
    VolRenAxisYPos,
    VolRenAxisYNeg,
    VolRenAxisZPos,
    VolRenAxisZNeg
} VolRenMajorAxis;

struct gluvvTF{   //transfer funciton widget
  int                   tfWindow;     //idenftifier for transfer function window
	TFWindow             *tfwin;        //the tf window class
	int                   loadme;       //reload the tf
	int                   paintme;      //paint the brush into the tf
	int                   dropme;       //drop the brush widget into the tf
	int                   clearpaint;   //clear the painted tf
	int                   ptexsz[3];    
	int                   numelts;      //RGBA ??
	int                   brushon;      //brush on or off?
	float                 slider1;      //slider for third axis emphasis
	float                 slider1hi;
	float                 slider1lo;
	float                 slider2;      //slider for fourth axis emphasis
	int                   histOn;       //use the histogram??
};

struct gluvvClip{  //clipping plane widget
	int                   on;           //flag for clip on(1) or off(0).
	int                   ortho;        //ortho graphic mode (axis aligned)
	VolRenMajorAxis       oaxis;        //ortho graphic axis to render along
	float                 xform[16];    //orientation of clip plane
	unsigned int          pname;        //name of plane
	float                 alpha;        //how to blend it with the volume
	float                 pos[3];       //position of clip plane (world coords)
	float                 vpos[3];      //      '' in volume space
	float                 dir[3];       //direction of clip plane normal
	float                 mousepnt[3];  //a selected point
	float                 corners[4][3];//corners of plane widget
};

struct gluvvPert{ //perturbation info
  int                   on;
  int                   numHarm;
  float                 weights[10];
  float                 scales[10];
};

typedef enum{ //gluvv brush modes
	NoBrush,           
	EllipseBrush,
	AutoEllipseBrush,
	TriangleBrush,
	OneDBrush,
	AutoOneDBrush
} gluvvBrush;

struct gluvvProbe{  //data probe widget
	float                 vpos[3];      //probes position in the volume
	float                 slider;       //slider pos [0--1]
	gluvvBrush            brush;        //brush type
};

typedef enum {  //Gluvv Shade modes (shadows are handled by the light struct)
	gluvvShadeUnknown, 
	gluvvShadeAmb,     //ambient
	gluvvShadeDiff,    //diffuse
	gluvvShadeDSpec,   //diffuse + specular
	gluvvShadeFaux,    //Faux shading
	gluvvShadeArb,     //arbitrary shading via pixel texture
	gluvvShadeMIP
} gluvvShade;

typedef enum {       //Gluvv platforms supported
  GPGineric,         //default
  GPOctane,          //-o
  GPOctane2,         //-o2
  GPInfinite,        //-i SGI Infinite Reality
  GPNV15,            //-nv15 GeForce 2
  GPNV20,            //-nv20 - 3D textures GeForce 3
  GPNV202D,          //-nv20 - 2D textures GeForce 3
  GPWildcat,         //Intense3D Wildcat
	GPATI8K            //ATI Radeon 8000 (R200)
} gluvvPlatform;

typedef enum {  //Gluvv Data modes                  #of bytes in data texture
	GDM_V1,     //1 value only, scalar                (1B)
	GDM_V1G,    //1 value and gradient                (2B)
	GDM_V1GH,   //1 value, gradient, and hessian      (3B)
	GDM_V2,     //2 values                            (2B)
	GDM_V2G,    //2 values with gradient              (3B)
	GDM_V2GH,   //2 values with gradient and hessian  (4B)
	GDM_V3,     //3 values                            (3B)
	GDM_V3G,    //3 values with gradient              (4B)
	GDM_V4,     //4 values                            (4B)
	GDM_VGH,    //VGH data (pre-computed)             (3B)
	GDM_VGH_VG, //VGH data - only use VG              (2B)
	GDM_VGH_V,  //VGH data - only use V               (1B)
	GDM_UNKNOWN //I have no idea what the data type is(?B)
} gluvvDataMode;

typedef enum {
	GB_NONE,
	GB_UNDER,
	GB_OVER,
	GB_ZERO
} gluvvBlend;

struct gluvvGlobal{ //------ GLUVV GLOBAL -----------------------------------
	int                   debug;
  gluvvWindow           win;    //widnow
  gluvvEnv              env;    //environment
  gluvvLight            light;  //light
  gluvvRInfo            rinfo;  //render info
  gluvvMouse            mouse;  //mouse
  gluvvPlatform         plat;   //plaform
  int                   picking;//picking toggle
  gluvvPick             pick;   //picked object
	gluvvVolRen           volren; //volume rendering stuff
	gluvvTF               tf;     //transfer function
	gluvvClip             clip;   //clipping plane tf
	gluvvProbe            probe;  //data probe widget
	int                   mprobe; //using data probe or clip plane for probing??
	MetaVolume           *mv;     //the volumes
	MetaVolume           *mv1;
	MetaVolume           *mv2;
	MetaVolume           *mv3;
	int                   mainWindow;  //mainWindow Identifier
	gluvvShade            shade;       //shading enable/disable flag
	gluvvDataMode         dmode;       //what data mode are we in?
	gluvvBlend            reblend;     //re-render scene with blend
  gluvvPert             pert;        //perturbation parameters
}; //-------------------------------------------------------------------------


//----------------- gluvvGlobal gluvv ----------------------------------------
//----------------------------------------------------------------------------
//               This needs to be declared in the "main" function!
//----------------------------------------------------------------------------
extern gluvvGlobal      gluvv;




#endif


