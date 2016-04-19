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
// R8kVolRen3D.h: interface for the R8kVolRen3D class.
//
//////////////////////////////////////////////////////////////////////

#ifndef R8kVolRen3D2_DOT_H
#define R8kVolRen3D2_DOT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gluvvPrimitive.h"
#include "gluvv.h"
#include "pbuffer.h"

class R8kVolRen3D : public gluvvPrimitive  
{
public:
	R8kVolRen3D();
	virtual ~R8kVolRen3D();

	void init();          //primitive call-back
	void draw();          //primitive call-back

	void createVolumes(int init=0); //load volumes

	void getModelView(float mv[16]); //get the modelview matrix - lookat matrix

private:
	//void createTrexVols();   //old version, do not use
	void createBricks(int init=0);     //use this one, handles multiple bricks & single volumes
	void loadTex1B(unsigned char *tex, unsigned int name, int sx, int sy, int sz);
	void loadTex2B(unsigned char *tex, unsigned int name, int sx, int sy, int sz);
	void loadTex3B(unsigned char *tex, unsigned int name, int sx, int sy, int sz);
	void loadTex4B(unsigned char *tex, unsigned int name, int sx, int sy, int sz);
	
	void create2DDepTex();   //creates the dependant textures
	void loadDepTex(unsigned char *dtex);  //loads a dependant texture [deptex name]
	void loadDepTex(unsigned char *dtex, unsigned int name); //loads data into texture name

	void createShadowTex();  //creates the shadow texture
	void createCubeTex();    //creates the cube map for shading
	void loadCubeTex(int good);      //updates cube map for shading
	void loadCubeFace(unsigned char *tex, unsigned int face, int sz); //load a face of the cube

	void scaleAlpha(unsigned char *dtexOut, 
		              float nsr, float osr, unsigned char *dtex);  //rescale alpha values to sample rate
	void copyScale(unsigned char *dtexOut,
		             float sr, unsigned char *dtex);               //scale and copy to a 2d texture
	
	//----------- test render -
	void renderSlice();      //tests dependant textures

	//----------- real renderer -
	void renderVolume();              //sets up volume rendering and then renders
	void renderSlice(float corners[4][3], int vol); //render slice with constant alpha

	//----------- bag of trick setups -
	void setupTexUs();              //sets up texture shaders, tex-units & dep tex reads
	void resetTexUs();              //resets tex-units and turns off texture shaders
	
	void setupFragShade(float ftb); //sets up register combiners for shading and blending (<0 ftb, >0 btf)
	void resetFragShade();          //turns off register combiners
	
	void setupPBuff();              //prepare pbuffer for rendering
	void resetPBuff();              //reset pbuffer
	
	void createFragShades(unsigned int *&names); // create and update fragment shaders
	void createFragShadow(unsigned int *&names); // create and update fragment shaders for shadows
	void createFragClip();                       // create and update fragment shaders for clips

	//----------- clipping planes -
	void setupClips(int vol,float vo[8][3], float tx[8][3]);  //orthogonal clipping, changes volume dims
	void drawClip(int vol, float dotv,                        //draws clip slice
		            float wmv[16]);                             //world model view                            
	void resetClips();                                        //turns off clipping

	//----------- handle bricking - RENDER CALL -----------
	//the axis in this has a 4th value which is the dotproduct of the light and view directions
	void renderBricks(GLdouble mv[16], GLfloat axis[4]);      //renders multiple sub vols & single vol

	//----------- gineric slice allong axis volume renderer
	void render3DVA(float sampleFrequency,
		              GLdouble mv[16],  //Model View Matrix
		              unsigned int v,   //3D textures name
		              float vo[8][3],	  //volume vertex coords model-space coords
		              float tx[8][3],	  //texture vertex coords tex-space coords
		              float axis[4]);	  //axis to slice along world-space coords axis[3] = dot(view, light)
	//----------- interect for render3DVA
	inline int intersect(
				 const float p0[3], const float p1[3], //line end points
		     const float t0[3], const float t1[3], //texture points
		     const float v0[3], const float v1[3], //view coord points
		     const float sp[3], const float sn[3], //plane point & norm
		     float pnew[3], float tnew[3], float vnew[3]); //new values 

	//----------- an inner loop renderer for render3DVA
	inline void volSlice(int edges, 
											 float tc[6][3], 
											 float pc[6][3],
											 int   order[6]);
	inline void volShadow(int edges,
		                    float tc[6][3],
												float pc[6][3],
												int   order[6],
												float ltxf[16],
												int   vol,
												double mv[16]);

	//----------- data -----------------------------------------------------
  int  go;                //if everything is ok, you can render
	int  sx, sy, sz;        //not used anymore, get info from meta volume
	// texture names
	unsigned int *texNames;    //bricked data names
	unsigned int *shadeNames;  //bricked shade names
	unsigned int shadowName;   //shadow texture
	unsigned int pbuffName;    //render to texture texName
	unsigned int deptexName;   //VG dep tex [gb]
	unsigned int deptex2Name;  //H dep tex  [ar]
	unsigned int deptex3Name;  //shadow tf dep tex
	unsigned int *fsNames;     //fragment shader names (view)
	unsigned int *pbfsNames;    //fragment shader names (pbuffer)
	unsigned int *pb2fsNames;   //fragment shader names (pbuffer2)
	unsigned int *pb3fsNames;   //fragment shader names (slice pbuff)
	unsigned int cpfsName;      //clipping plane fragement shader.
	//texture data
	unsigned char *deptex;     //dep tex data  [gb]
	unsigned char *deptex2;    //dep tex2 data [ar]
	unsigned char *gDeptex;    //dep tex scaled to good sample rate [gb]
	unsigned char *iDeptex;    //dep tex scaled to interactive sample rate [gb]
	unsigned char *gDeptex3;   //dep tex for shadow tf (good sample rate)
	unsigned char *iDeptex3;   //dep tex for shadow tf (interactive)
	
	// sample rate saves
	float lastSamp;            //last sample rate rendered
	float lastGoodSamp;        //last good sample rate
	float lastInteSamp;        //last interactive sample rate

	// THE PBUFFERs
	PBuffer *pbuff;            //pbuffer for rendering light's point of view
	PBuffer *pbuff2;           //pbuffer for rendering light's point of view (bounce)
	int      currentPbuff;     //which one are we on?
	PBuffer *sliceBuff;        //pbuffer for rendering light's point of view
	void MakeCurrent();        //switch back to this context

	//----- some shadow data ------
	float xscale;
	float yscale;
	float xbias;
	float ybias;

	//attinuation parameters
	float volview[3];  // view direction in volume/model space
	float vollt[3];    // light postion in volume/model space
	float ltrange[2];  // range to quantize to
	float fogrange[2]; // fog range

	int   currentTStep; //current time step
};

#endif // !defined(AFX_R8kVolRen3D_H__F0928F0F_2C8C_47F5_95AD_59CBF70749C9__INCLUDED_)
