//<html><a href="../src/VolumeRenderer.cpp">source</a> <plaintext>
// ============================================================
//                          === TRex === 
//
// $Date: 2001/06/27 21:05:16 $
// $Revision: 1.3 $
//
// Authors: J. Kniss
//
//  Implementations for a volume renderer.
//
//                   ________    ____   ___ 
//                  |        \  /    | /  /
//                  +---+     \/     |/  /
//                  +--+|  |\    /|     < 
//                  |  ||  | \  / |  |\  \ 
//                  |      |  \/  |  | \  \ 
//                   \_____|      |__|  \__\
//                       Copyright  2001 
//                      Joe Michael Kniss
//-------------------------------------------------------------------------
//   This class implements all of the basic functionality for
//    volume rendering, workis in conjunction with a TLUT
//--------------------------------------------------------------------------

#ifndef __VolumeRenderer_H
#define __VolumeRenderer_H

//note you can remove anything to do with NRRD:
#define USE_NRRD 0 //take out nrrd references so we dont have to link libnrrd

#if USE_NRRD
#include <nrrd.h>
#endif

#include "MetaVolume.h"

#ifdef _WIN32
#include <windows.h>		//  must be included before any OpenGL
#endif
#include <GL/gl.h>

#include "TLUT.h"

#define MAX_VOLREN_2DTEX 512
#define MAX_VOLREN_3DTEX 30
#define MAX_VOLREN_BRICKS 128


//========== NOTES ===========================================
// Standard brick vertex ordering:
//  
//     (011)        (111)
//       6 +---------+ 7   Where 1's are the size of the brick
//        /|        /|      allong that axis
//       / |       / |
// (001)/  |(101) /  |
//   4 +---------+ 5 |
//     |   |     |   |(110) z axis
//     | 2 +-----+---+ 3    ^   
//     |  /(010) |  /       |   y axis
//     | /       | /        | /
//     |/        |/         |/
//   0 +---------+ 1        +-------> x axis 
//  (000)      (100)
//

//volren type, use this to specify the type of texturing to use
enum { //right now only 3DExt is available 
   VolRenUnkown,
   VolRen2DTexture,
   VolRen3DTexture,
   VolRen3DExt
};


class VolumeRenderer{

public:

   VolumeRenderer(){}
#if USE_NRRD
   VolumeRenderer(const char *filename){loadNRRD_Volume(filename);}
#endif
   
   VolumeRenderer(MetaVolume *vm, //Constructor made just for TRex 
		  int subVolNum); 
   
   ~VolumeRenderer(){if(tlut) delete tlut;}

   //---- download 3d textures once OpenGL has been initialized
   int             createVolume(int type, Volume *v);  

   //---- download multiple 3d textures      (Volume *v[nVols]) (bricks)
   int             createVolume(int type, Volume *v, int nVols);
   
   int             createTLUT(); //use this if you want the VolumeRenderer
                                 //to create a tlut for you

   //-------- to be put in draw function ---------
   
   void            renderVolume(float sampleRate,// render whole volume with a 
				                        GLdouble mv[16]);// specific sample rate, good 
                                                 // for imperfect cubes (best)


   void            renderVolume(float sampleRate, // renders a smaller axis
				                        GLdouble mv[16],  // aligned cube with axis
				                        float xext[2],    // extents from {0..1}
				                        float yext[2],
				                        float zext[2]);

                                                 //render an arbitrary slice
   void            renderSlice(float quad[4][3], float alpha);
   
   //-------- Set & Get Volume rendering "features" ----------
                                                 //be sure to load the
   TLUT           *getColorMap(){return tlut;}   //color map that you want, the
                                                 //default is linear grey scale

                   // (non-0)=on, 0=off (default = off)
   void            useBBox(int on_off){m_bb= on_off;}
   void            useBBoxBrackets(int on_off){m_bbb= on_off;}  

   void            barfInfo(); //report volume information

#if USE_NRRD 
   //intilialize volume
   int             loadNRRD_Volume(const char *volname);//jmk&gk's file library
#endif

   //========================================================================
private: //==================================================================
   
   int              glVolErr(const char *place);  //easy way to report errors
   int              create3DtextureEXT(unsigned char *uctex,//down load 3D 
																       Volume &v,           //to texture memory
																		   GLuint texname); 
   void             renderBBox();                            //1x1x1 bbox
   void             renderBBox(float xb, float yb, float zb);//bbox with extnts
                    //render only the corners of the bounding box
   void             renderBBoxBrackets(float xb, float yb, float zb);

   void             render3DVolumeEXTVA(float sampleFrequency, //view aligned 
																				GLdouble mv[16], //model view matrix
																				int v);      //which volume to render


   void             render3DVolumeEXTSV(float sampleFrequency, //small volume
																				GLdouble mv[16], //model view matrix
																				int v,         //which volume to render
																				float xext[2], //extents along axies
																				float yext[2],
																				float zext[2]);

   void             render3DVA(float sampleFrequency,
															 GLdouble mv[16],
															 int v,
															 float vo[8][3],   //volume vertex coords
															 float tx[8][3],   //texture vertex coords
															 float axis[3]);   //axis to slice along

                                                  //render one arbitrary slice
   void             render3dSliceEXT(float quad[4][3], float a);
   //grab info from the volume, load local vars 
   // this includes size[] int, sizef[] float, origf[] orign of subvolume
   //  center[] float center of subvolume
   inline void      initVol(int volumeNumber);   

   TLUT                   *tlut;       //texture lookup table
   MetaVolume             *m_vol;      //holds data
   //note: a volume is associated with a 3Dtexture (volume[i]<->texname3D[i])
   Volume                 *volume[MAX_VOLREN_3DTEX];//current volume(s)
   int                     numSubVols; //how many sub volumes
   unsigned char          *data;       //points to subvolume in TRexVolModel
   int                     subVolNum;  //which sub volume are we? (not used)
   int                     size[3];    //origional volume(subVol) 
   float                   sizef[3];   //relative size [0-1]
   float                   origf[3];   //origin of volume
   float                   center[3];  //center of the volume...
   float                   spacing[3]; //distance between samples(not used yet)
   int                     type;       //how are we rendering this volume 
                                       //see 'VolRenType' enum above

   GLuint                  texname2D[MAX_VOLREN_2DTEX]; //for 2D texture method
   
   GLuint                  texname3D[MAX_VOLREN_3DTEX];   //3D texture method

   int                     m_bb;       //do we render the bounding box??
   int                     m_bbb;      //do be render the bounding box brackets

#if USE_NRRD
   Nrrd                   *nrrd_vol;
   Nrrd                   *nrrd_uchar;
#endif
   
   //useful vector operations
   inline GLfloat dotV3(GLfloat one[4], GLfloat two[4]);
   inline void identityMatrix(GLdouble m[16]);
   inline void translateV3(GLfloat out[3], GLdouble mat[16], GLfloat in[3]);
   inline int intersect(const float p0[3], const float p1[3], //line end points
											  const float t0[3], const float t1[3], //texture points
											  const float v0[3], const float v1[3], //view coord points
											  const float sp[3], const float sn[3], //plane point & norm
												float pnew[3], float tnew[3], float vnew[3]);//new values 
   void inverseMatrix( GLdouble invm[16], const GLdouble m[16] );
   inline void printV(float v[3]);
   
   inline void drawLine(float x0, float y0, float z0, 
			float x1, float y1, float z1);
};


#endif










