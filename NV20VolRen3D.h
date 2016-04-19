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
// NV20VolRen3D.h: interface for the NV20VolRen3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NV20VOLREN3D_H__F0928F0F_2C8C_47F5_95AD_59CBF70749C9__INCLUDED_)
#define AFX_NV20VOLREN3D_H__F0928F0F_2C8C_47F5_95AD_59CBF70749C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gluvvPrimitive.h"
#include "gluvv.h"

class NV20VolRen3D : public gluvvPrimitive  
{
public:
	NV20VolRen3D();
	virtual ~NV20VolRen3D();

	void init();          //primitive call-back
	void draw();          //primitive call-back

	void createVolumes(); //load volumes

private:
	//void createTrexVols();   //old version, do not use
	void createBricks();     //use this one, handles multiple bricks & single volumes
	void loadTex1B(unsigned char *tex, unsigned int name, int sx, int sy, int sz);
	void loadTex2B(unsigned char *tex, unsigned int name, int sx, int sy, int sz);
	void loadTex4B(unsigned char *tex, unsigned int name, int sx, int sy, int sz);
	
	void create2DDepTex();   //creates the dependant textures
	void loadDepTex(unsigned char *dtex);  //loads a dependant texture [deptex name]
	void loadDepTex(unsigned char *dtex, unsigned int name); //loads data into texture name
	
	void scaleAlpha(float nsr, float osr, unsigned char *dtex);  //rescale alpha values to sample rate
	void copyScale(float sr, unsigned char *dtex);               //scale and copy to a 2d texture
	
	void updateQNShade();    //not used, qn sucks!

	//----------- test render -
	void renderSlice();      //tests dependant textures

	//----------- real renderer -
	void renderVolume();     //sets up volume rendering and then renders
	void renderSlice(float corners[4][3], int vol); //render slice with constant alpha
	void setupShaders();     //sets up texture shaders, tex-units & dep tex reads
	void resetShaders();     //resets tex-units and turns off texture shaders
	void setupRegComb();     //sets up register combiners for shading and blending
	void resetRegComb();     //turns off register combiners

	//----------- clipping planes
	void setupClips(int vol,float vo[8][3], float tx[8][3]);  //orthogonal clipping, changes volume dims
	void drawClip(int vol, float dotv,                        //draws clip slice
		            float wmv[16]);                             //world model view                            
	void resetClips();                                        //turns off clipping

	void renderBricks(GLdouble mv[16]);                       //renders multiple sub vols & single vol

	//----------- gineric slice allong axis volume renderer
	void render3DVA(float sampleFrequency,
		              GLdouble mv[16],  //Model View Matrix
		              unsigned int v,   //3D textures name
		              float vo[8][3],	  //volume vertex coords model-space coords
		              float tx[8][3],	  //texture vertex coords tex-space coords
		              float axis[3]);	  //axis to slice along world-space coords
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

  int  go;                //if everything is ok, you can render
	int  sx, sy, sz;        //not used anymore, get info from meta volume
	unsigned int *texNames;    //bricked data names
	unsigned int *shadeNames;  //bricked shade names
	unsigned int *qnNames;     //not used qn sucks
	unsigned int deptexName;   //VG dep tex [gb]
	unsigned int deptex2Name;  //H dep tex  [ar]
	unsigned int qnDeptexName; //not used qn sucks
	
	unsigned char *deptex;     //dep tex data  [gb]
	unsigned char *deptex2;    //dep tex2 data [ar]
	unsigned char *gDeptex;    //dep tex scaled to good sample rate [gb]
	unsigned char *iDeptex;    //dep tex scaled to interactive sample rate [gb]
	
	unsigned char *qnDeptex;   //not used qn sucks
	float         *qnRef;      //again not used...


	float lastSamp;            //last sample rate rendered
	float lastGoodSamp;        //last good sample rate
	float lastInteSamp;        //last interactive sample rate

};

#endif // !defined(AFX_NV20VOLREN3D_H__F0928F0F_2C8C_47F5_95AD_59CBF70749C9__INCLUDED_)
