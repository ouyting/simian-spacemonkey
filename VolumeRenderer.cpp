//
//--------------------------------------------------------------------------
//
//  VolumeRenderer.h
//   simple implementations for a Texture Based volume renderer
//   6-7-00
//    jmk
//
//		     ________	   ____	  ___ 
//		    |	       \	/    | /  /
//		    +---+     \/     |/	 /
//		    +--+|  |\	   /| 	  < 
//		    |  ||  | \	/ |  |\	 \ 
//		    |	     |  \/  |  | \  \ 
//		     \_____|	    |__|	\__\
//			 Copyright  2001 
//			Joe Michael Kniss
//-------------------------------------------------------------------------
//--------------------------------------------------------------------------

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "./glUE.h"
//#include <GL/glut.h>

#include <iostream.h>
#include <stdio.h>
#include <math.h>

#include "VolumeRenderer.h"
#include "VectorMath.h"
#include "gluvv.h"

//#define MAX(x, y) ((x) > (y) ? (x) : (y))
//#define MIN(x, y) ((x) < (y) ? (x) : (y))
//#define ABS(x) (((x) > 0) ? (x) : (-x))

/* just so I can remember how to use this
******** AFFINE(i,x,I,o,O)
**
** given intervals [i,I], [o,O] and a value x which may or may not be
** inside [i,I], return the value y such that y stands in the same
** relationship to [o,O] that x does with [i,I].  Or:
**
**    y - o	    x - i     
**   -------   =   -------
**    O - o	    I - i
**
** It is the callers responsibility to make sure I-i and O-o are 
** both greater than zero -glk
*/
#define AFFINE(i,x,I,o,O) ( ((double)(O)-(o))*((double)(x)-(i)) \
			    / ((double)(I)-(i)) + (o))

//--------------------------------------------------------------------------

VolumeRenderer::VolumeRenderer(MetaVolume *vm, int subVolNum)
{
   //we dont need this crap?? not here anyway
   m_vol = vm;
#if 0 
  //data =    model->getData(subVolNum);
   size[0] = m_vol->volumes[subVolNum].xiSize;
   size[1] = m_vol->volumes[subVolNum].yiSize;
   size[2] = m_vol->volumes[subVolNum].ziSize;

   float maxsize = MAX(size[0], size[1]);
   maxsize = MAX(maxsize, size[2]);

   //sizef [0-1] ? usualy but could be arbitrary physical units (cm, in, ft)
   sizef[0] = m_vol->volumes[subVolNum].xfSize;// AFFINE(0, size[0], maxsize, 0, 1);
   sizef[1] = m_vol->volumes[subVolNum].yfSize;//AFFINE(0, size[1], maxsize, 0, 1);
   sizef[2] = m_vol->volumes[subVolNum].zfSize;//AFFINE(0, size[2], maxsize, 0, 1);

   center[0] = m_vol->volumes[subVolNum].xfPos;
   center[1] = m_vol->volumes[subVolNum].yfPos;
   center[2] = m_vol->volumes[subVolNum].zfPos;
#endif
   for(int i=0; i<MAX_VOLREN_3DTEX; ++i){
      texname3D[i] = -1; //indicates that texname3D has not been generated yet
      volume[i] = 0;
   }

   this->subVolNum = subVolNum;
   numSubVols = 0;
   
   tlut = 0;
   //bounding boxes off
   m_bb	 = 0;
   m_bbb = 0;

   tlut = NULL;
}
//========================================================================
//------------------------------------------------------------------------
//========================================================================
int VolumeRenderer::createVolume(int type, Volume *v)
{
   cout << "Creating one volume" << endl;
   this->type = type;
   volume[0] = v;
   numSubVols = 1;

   switch(type){
   case	   VolRen2DTexture:
      cerr << "2D texture mapping method is not implemented" <<endl;
      return 1;
   case VolRen3DTexture:
      cerr << "3D texture mapping method (OGL 1.2) is not implemented" <<endl;
      return 1;
   case VolRen3DExt:
      if(texname3D[0] == -1){
	 glGenTextures(1, texname3D);
	 cerr << "generating a texture = " << texname3D[0] << endl;
      }
      return create3DtextureEXT(v->currentData, *v, texname3D[0]);
   default:
      cerr << "Error : VolumeRenderer::createVolume() : type unknown" << endl;
      type = VolRenUnkown;
      return 1;
   }
}
//-------------------------------------------------------------------
int VolumeRenderer::createVolume(int type, Volume *v, int nVols)
{ //load tones of volumes in at once!!

   cout << "***Creating " << nVols << " volumes " << endl;

   this->type = type;

   for(int i=0; i<nVols; ++i){
      volume[i] = &v[i];
   }

   numSubVols = nVols;

   int err = 0;
   int sizez = 0;

   switch(type){
   case	   VolRen2DTexture:
      cerr << "2D texture mapping method is not implemented" <<endl;
      return 1;
   case VolRen3DTexture:
      cerr << "3D texture mapping method (OGL 1.2) is not implemented" <<endl;
      return 1;
   case VolRen3DExt:

		 if(texname3D[0] == -1)
			 glGenTextures(nVols, texname3D);
		 for(i = 0; i<nVols; ++i){	  
			 err |= create3DtextureEXT(v[i].currentData, v[i], texname3D[i]);
		 }
		 
		 return err;
   default:
		 cerr << "Error : VolumeRenderer::createVolume() : type unknown" << endl;
		 type = VolRenUnkown;
		 return 1;
   }
}
//------------------------------------------------------------------------
int VolumeRenderer::create3DtextureEXT(unsigned char* uctex, Volume &v,
				       GLuint texname)
{
  cout << "Creating 3D texture (EXT) " << v.xiSize << " " << v.yiSize
       << " " << v.ziSize << endl;
   int sz[3];
   
   sz[0] = v.xiSize;
   sz[1] = v.yiSize;
   sz[2] = v.ziSize;


   glEnable(GL_TEXTURE_3D_EXT);
   
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   glBindTexture(GL_TEXTURE_3D_EXT, texname);
   glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP);
#ifndef WIN32
   glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_R, GL_CLAMP);
#else
   glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_R_EXT, GL_CLAMP);
#endif
   glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   glTexImage3DEXT(GL_TEXTURE_3D_EXT, 
		   0,
		   GL_INTENSITY8,
		   sz[0], 
		   sz[1], 
		   sz[2], 
		   0, 
		   GL_LUMINANCE, 
		   GL_UNSIGNED_BYTE, 
		   (GLvoid*) uctex );

   //****** load color lookup table *****************************
   if (tlut == 0)
     tlut = new TLUT();

   cout << "3D texture (EXT) created =" << texname << endl;
	 glDisable(GL_TEXTURE_3D_EXT);
   return glVolErr("create3Dtexture()");
}
//------------------------------------------------------------------------
int VolumeRenderer::createTLUT()
{
   if(tlut == 0)
      tlut = new TLUT();
   return 1;
}

//========================================================================
//------------------------------------------------------------------------
//========================================================================
void VolumeRenderer::barfInfo()
{

   cout << "------------ Volume information ---------------\n"
	<< "-----------------------------------------------\n";
#if USE_NRRD
   cout << "NRRD size: " << (long int)nrrd_vol->num << "\n"
	<< "NRRD type: " << nrrd_vol->type << "\n"
	<< "NRRD dimensions: " << nrrd_vol->dim << "\n"
	<< "NRRD encoding: " << nrrd_vol->encoding << "\n"
	<< "NRRD Axis : size : spacing : min : max\n";
   for(int i=0; i<nrrd_vol->dim; ++i){
      cout << "	  " << nrrd_vol->label[i] << " : " 
	   << nrrd_vol->size[i] << " : "
	   << nrrd_vol->spacing[i] << " : " 
	   << nrrd_vol->axisMin[i] << " : "
	   << nrrd_vol->axisMax[i] << "\n";
   }
   cout << "VOL size: " << (long int)nrrd_uchar->num << "\n"
	<< "VOL type: " << nrrd_uchar->type << "\n"
	<< "VOL dimensions: " << nrrd_uchar->dim << "\n"
	<< "VOL encoding: " << nrrd_uchar->encoding << "\n"
	<< "VOL Axis : size : spacing : min : max\n";
   for(i=0; i<nrrd_uchar->dim; ++i){
      cout << "	  " << nrrd_uchar->label[i] << " : " 
	   << nrrd_uchar->size[i] << " : "
	   << nrrd_uchar->spacing[i] << " : " 
	   << nrrd_uchar->axisMin[i] << " : "
	   << nrrd_uchar->axisMax[i] << "\n";
   }
   cout << "Contents: " << nrrd_vol->content << "\n";
#else //TREX
   for(int i=0; i<numSubVols; ++i){
      initVol(i);
      cout << "**** Volume " << i << " ****" << endl;
      cout << "size	   : " << size[0]*size[1]*size[2] << endl;
      cout << "type	   : unsigned char" << endl;
      cout << "X size	   : " << size[0] << endl;
      cout << "Y size	   : " << size[1] << endl;
      cout << "Z size	   : " << size[2] << endl;
      cout << "X spacing   : " << spacing[0] << endl;
      cout << "Y spacing   : " << spacing[1] << endl;
      cout << "Z spacing   : " << spacing[2] << endl;
      cout << endl;
   }
#endif
   cout << "------------ End Volume info ------------------\n";
   cout << "-----------------------------------------------\n";
}

//========================================================================
//------------------- Render some volumes --------------------------------
//========================================================================


//standard 3d view alligned sliceing
void VolumeRenderer::renderVolume(float sampleRate, GLdouble mv[16])
{
	switch(type){
	case	VolRen2DTexture:
		cerr << "2D texture mapping method is not implemented" <<endl;
		return;
	case VolRen3DTexture:
		cerr << "3D texture mapping method (OGL 1.2) is not implemented" <<endl;
		return;
	case VolRen3DExt:
		
		if(numSubVols > 1){
			int *order = new int[numSubVols];
			float *zval = new float[numSubVols];
			for(int i=0; i<numSubVols; ++i){ //initialize sorting crap
				order[i] = i;
				initVol(i);
				zval[i] = (float)(center[0]*mv[2] + center[1]*mv[6] + center[2]*mv[10]);
			}
			int tmp;
			for(i=0; i< numSubVols-1; ++i){ //now sort
				for(int j=i+1; j<numSubVols; ++j){
					if(zval[order[i]] > zval[order[j]]){//test for swap
						tmp = order[i];
						order[i] = order[j];
						order[j] = tmp;
					}
				}
			}
			
			for(i=0 ; i< numSubVols; ++i){ //finaly render
				initVol(order[i]);
				glPushMatrix();
				glTranslatef(center[0], center[1], center[2]);
				render3DVolumeEXTVA(sampleRate, mv, order[i]);
				glPopMatrix();
			}
		}
		else{
			initVol(0);
			glTranslatef(center[0], center[1], center[2]);
			mv[12] += center[0];
			mv[13] += center[1];
			mv[14] += center[2];
			render3DVolumeEXTVA(sampleRate, mv, 0);
		}
		return;
	}
}


//------------ render a smaller volume ---------------------------------
// overloaded funciton gives access to subvolue-extents
void VolumeRenderer::renderVolume(float sampleRate, // renders a smaller axis
				GLdouble mv[16],    // aligned cube with axis
				float xext[2],	    // extents from {0..1}
				float yext[2],
				float zext[2])
{
  switch(type){
   case	   VolRen2DTexture:
      cerr << "2D texture mapping method is not implemented" <<endl;
      return;
   case VolRen3DTexture:
      cerr << "3D texture mapping method (OGL 1.2) is not implemented" <<endl;
      return;
   case VolRen3DExt:

      if(numSubVols > 1){
	 int *order = new int[numSubVols];
	 float *zval = new float[numSubVols];
	 for(int i=0; i<numSubVols; ++i){ //initialize sorting crap
	    order[i] = i;
	    initVol(i);
	    zval[i] = (float)(center[0]*mv[2] + center[1]*mv[6] + center[2]*mv[10]);
	 }
	 int tmp;
	 for(i=0; i< numSubVols-1; ++i){ //now sort
	    for(int j=i+1; j<numSubVols; ++j){
	       if(zval[order[i]] > zval[order[j]]){//test for swap
		  tmp = order[i];
		  order[i] = order[j];
		  order[j] = tmp;
	       }
	    }
	 }

	 for(i=0 ; i< numSubVols; ++i){ //finaly render
	    initVol(order[i]);
	    glPushMatrix();
	    //glTranslatef(center[0], center[1], center[2]);
	    GLdouble modelMatrix[16];
	    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	    render3DVolumeEXTSV(sampleRate, modelMatrix, order[i], 
				xext, yext, zext);
	    glPopMatrix();
	 }
      }
      else{

	 initVol(0);
	 //glTranslatef(center[0], center[1], center[2]);
	 GLdouble modelMatrix[16];
	 glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	 render3DVolumeEXTSV(sampleRate, modelMatrix, 0, 
			     xext, yext, zext);
      }
      return;
   }
}


//------------------------------------------------------------------------
//-------------- View Aligned slices -------------------------------------
//------------------------------------------------------------------------

void VolumeRenderer::render3DVolumeEXTVA(float sampleFrequency, 
					 GLdouble mv[16],
					 int v)
{
     float vo[8][3] = { //volume vertex edge coords
      {0,	 0,	   0},
      {sizef[0], 0,	   0},
      {0,	 sizef[1], 0},
      {sizef[0], sizef[1], 0},
      {0,	 0,	   sizef[2]},
      {sizef[0], 0,	   sizef[2]},
      {0,	 sizef[1], sizef[2]},
      {sizef[0], sizef[1], sizef[2]}};

   float tx[8][3] = { //volume texture coords/ edge to edge (bad for bricks)
      {0, 0, 0},
      {1, 0, 0},
      {0, 1, 0},
      {1, 1, 0},
      {0, 0, 1},
      {1, 0, 1},
      {0, 1, 1},
      {1, 1, 1}};

   float axis[3] = {0,0,1};

   render3DVA(sampleFrequency, mv, v, vo, tx, axis);
}

 
//==========================================================================
//------------ render a small volume ---------------------------------------
//==========================================================================

void VolumeRenderer::render3DVolumeEXTSV(float sampleFrequency, 
					 GLdouble mv[16], //model view matrix
					 int v,	      //which volume to render
					 float xext[2], //extents along axies
					 float yext[2],
					 float zext[2])
{
  float ext[3] = {origf[0]+sizef[0], origf[1]+sizef[1], origf[2]+sizef[2]};
  float x[2], y[2], z[2];
  //cerr << "smalvol ext" << xext[0] << "-" << xext[1] << " " << yext[0] << "-" << yext[1]
  //	 << " " << zext[0] <<  "-" << zext[1] << endl;
  //get the new extents based on subvolumes size
  x[0] = xext[0] > origf[0] ? (xext[0] < ext[0] ? xext[0] : ext[0]) : origf[0];
  x[1] = xext[1] > origf[0] ? (xext[1] < ext[0] ? xext[1] : ext[0]) : origf[0];
  y[0] = yext[0] > origf[1] ? (yext[0] < ext[1] ? yext[0] : ext[1]) : origf[1];
  y[1] = yext[1] > origf[1] ? (yext[1] < ext[1] ? yext[1] : ext[1]) : origf[1];
  z[0] = zext[0] > origf[2] ? (zext[0] < ext[2] ? zext[0] : ext[2]) : origf[2];
  z[1] = zext[1] > origf[2] ? (zext[1] < ext[2] ? zext[1] : ext[2]) : origf[2];
  //now see if we have something to render:
  if((x[0] == x[1]) || (y[0] == y[1]) || (z[0] == z[1]))
    return;

  //cerr << "smalvol ext" << x[0] << "-" << x[1] << " " << y[0] << "-" << y[1]
  //	 << " " << z[0] <<  "-" << z[1] << endl;
  //now put the extents in the local subvolume space
  x[0] -= origf[0];
  x[1] -= origf[0];
  y[0] -= origf[1];
  x[1] -= origf[1];
  z[0] -= origf[2];
  z[1] -= origf[2];
  /*
  float vo[8][3] = { //volume vertex edge coords
    {x[0], y[0], z[0]},
    {x[1], y[0], z[0]},
    {x[0], y[1], z[0]},
    {x[1], y[1], z[0]},
    {x[0], y[0], z[1]},
    {x[1], y[0], z[1]},
    {x[0], y[1], z[1]},
    {x[1], y[1], z[1]}};
  */
    float vo[8][3] = { //volume vertex edge coords
    {0, 0, 0},
    {1, 0, 0},
    {0, 1, 0},
    {1, 1, 0},
    {0, 0, 1},
    {1, 0, 1},
    {0, 1, 1},
    {1, 1, 1}};

  float tx[8][3] = { //volume vertex edge coords
    {x[0]/sizef[0], y[0]/sizef[1], z[0]/sizef[2]},
    {x[1]/sizef[0], y[0]/sizef[1], z[0]/sizef[2]},
    {x[0]/sizef[0], y[1]/sizef[1], z[0]/sizef[2]},
    {x[1]/sizef[0], y[1]/sizef[1], z[0]/sizef[2]},
    {x[0]/sizef[0], y[0]/sizef[1], z[1]/sizef[2]},
    {x[1]/sizef[0], y[0]/sizef[1], z[1]/sizef[2]},
    {x[0]/sizef[0], y[1]/sizef[1], z[1]/sizef[2]},
    {x[1]/sizef[0], y[1]/sizef[1], z[1]/sizef[2]}};
  
   float axis[3] = {0,0,1};

   //center it about {000}
   //glTranslatef(-(x[0]+(x[1]-x[0])/2),
   //		-(y[0]+(y[1]-y[0])/2),
   //		-(z[0]+(z[1]-z[0])/2));
   glTranslatef(-.5, -.5, -.5);
   render3DVA(sampleFrequency, mv, v, vo, tx, axis);

}


//------------------------------------------------------------------------
//------ Gineric render allong a vector ----------------------------------
//------------------------------------------------------------------------
void VolumeRenderer::render3DVA(float sampleFrequency,
		GLdouble mv[16],
		int v,
		float vo[8][3],	  //volume vertex coords model-space coords
		float tx[8][3],	  //texture vertex coords tex-space coords
		float axis[3])	 //axis to slice along world-space coords
{

   float rv[8][3];     //the rotated volume (may include a scale)
   float maxval = -10; //(tmp)
   float minval = 10;
   int minvert = 0;
   GLdouble mvinv[16];

   inverseMatrix(mvinv, mv); //invert model view matrix

   for(int i=0; i<8; ++i){   //translate model to world coords (view space)
		 translateV3(rv[i], mv, vo[i]);
		 if(maxval < MAX(maxval, rv[i][2])){
			 maxval = MAX(maxval, rv[i][2]);
		 }
		 if(minval > MIN(minval, rv[i][2])){
			 minval = MIN(minval, rv[i][2]);
			 minvert = i;  //determine the starting point for slicing
		 }
   }
	 // fix this function so that the eye is the universal reference point for all volumes!
	 //  we will get artifacts at brick boundaries otherwise!
	

   //find the slice plane point 'sp' (initial) and the slice plane normal 'sn'
   //sp is the sliceing starting point, simply the vertex farthest from the eye
   float sp[3] = {vo[minvert][0], vo[minvert][1], vo[minvert][2]}; 
   //float vpn[3] = {0,0,1};  //view plane normal, points to eye (temp variable)
   float vpn[3];
   vpn[0] = axis[0]; vpn[1] = axis[1]; vpn[2] = axis[2];
   float sn[3];		    //slice plane normal
   translateV3(sn, mvinv, vpn); //move vpn to sn (model space);
   //note: sn & sp are defined in Model Space, ie the space where the volume
   // is alligned with the (x,y,z) axies
   float normsn = (float)sqrt(sn[0]*sn[0] + sn[1]*sn[1] + sn[2]*sn[2]); //normalize
   sn[0]/=normsn;
   sn[1]/=normsn;
   sn[2]/=normsn;
   
   //now find the distance we need to slice (|max_vertex - min_vertex|)
   float maxd[3] = {0, 0, maxval}; //(tmp) only use z-coord (view space)
   float mind[3] = {0, 0, minval}; //(tmp) ditto	    (view space)
   float maxv[3], minv[3];	   //(tmp)
   translateV3(maxv, mvinv, maxd); //translate back to model space
   translateV3(minv, mvinv, mind); //ditto
   maxv[0] -= minv[0]; //subtract
   maxv[1] -= minv[1];
   maxv[2] -= minv[2];

   //now take the norm of this vector... we have the distance to be sampled
   float dist = (float)sqrt(maxv[0]*maxv[0] + maxv[1]*maxv[1] + maxv[2]*maxv[2]);

   //draw a red bounding box
   if(m_bbb) renderBBoxBrackets(sizef[0], sizef[1], sizef[2]);
   if(m_bb)  renderBBox(sizef[0], sizef[1], sizef[2]);


   glDisable(GL_LIGHTING); //light makes it look bad!

   glDisable(GL_CULL_FACE);
   glPolygonMode(GL_FRONT, GL_FILL);
   glPolygonMode(GL_BACK, GL_FILL);

   glEnable(GL_TEXTURE_3D_EXT);

   cerr << "drawing" << texname3D[v] << endl;
   glBindTexture(GL_TEXTURE_3D_EXT, texname3D[v]);

#ifdef WIN32
#if    WILDCAT
   glEnable(GL_TEXTURE_COLOR_TABLE_EXT);
#endif
#else
   glEnable(GL_TEXTURE_COLOR_TABLE_SGI);
#endif

   glEnable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

   glColor4f(1, 1, 1, 1);
   glVolErr("drawVA");
   //distance between samples
   float dis = sizef[0] / (size[0] * sampleFrequency); 
   float del[3] = {sn[0]*dis, sn[1]*dis, sn[2]*dis};

   int samples = (int)((dist) / dis);//(total distance to be sampled)/(sample spacing)

   //samples /= 40;
   //del[0] *= 40;
   //del[1] *= 40;
   //del[2] *= 40;

   float poly[6][3];   // for edge intersections
   float tcoord[6][3]; // for texture intersections
   float tpoly[6][3];  // for transformed edge intersections
   int edges;	       // total number of edge intersections

   //highly un-optimized!!!!!!!!!
   for(i = 0 ; i < samples; ++i){ //for each slice
      //increment the slice plane point by the slice distance
      sp[0] += del[0];
      sp[1] += del[1];
      sp[2] += del[2];

      edges = 0; 
      //now check each edge of the volume for intersection with.. 
      //the plane defined by sp & sn
      //front bottom edge
      edges += intersect(vo[0], vo[1], tx[0], tx[1], rv[0], rv[1], sp, sn, 
			 poly[edges], tcoord[edges], tpoly[edges]);
      //front left edge
      edges += intersect(vo[0], vo[2], tx[0], tx[2], rv[0], rv[2], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //front right edge
      edges += intersect(vo[1], vo[3], tx[1], tx[3], rv[1], rv[3], sp, sn, 
			 poly[edges], tcoord[edges], tpoly[edges]);
      //left bottom edge
      edges += intersect(vo[4], vo[0], tx[4], tx[0], rv[4], rv[0], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //right bottom edge
      edges += intersect(vo[1], vo[5], tx[1], tx[5], rv[1], rv[5], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //front top edge
      edges += intersect(vo[2], vo[3], tx[2], tx[3], rv[2], rv[3], sp, sn, 
			 poly[edges], tcoord[edges], tpoly[edges]);
      //back bottom edge
      edges += intersect(vo[4], vo[5], tx[4], tx[5], rv[4], rv[5], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //back left edge
      edges += intersect(vo[4], vo[6], tx[4], tx[6], rv[4], rv[6], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //back right edge
      edges += intersect(vo[5], vo[7], tx[5], tx[7], rv[5], rv[7], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //back top edge
      edges += intersect(vo[6], vo[7], tx[6], tx[7], rv[6], rv[7], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //left top edge
      edges += intersect(vo[2], vo[6], tx[2], tx[6], rv[2], rv[6], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //right top edge
      edges += intersect(vo[3], vo[7], tx[3], tx[7], rv[3], rv[7], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);

      if(edges == 3){ //no sort required for triangles
				//cerr << "doing a triangle" <<endl;
				glBegin(GL_TRIANGLES);
				{
					glTexCoord3fv(tcoord[0]);
					glVertex3fv(poly[0]);
					glTexCoord3fv(tcoord[1]);
					glVertex3fv(poly[1]);
					glTexCoord3fv(tcoord[2]);
					glVertex3fv(poly[2]);
				}
				glEnd();
      }
      else { //compute convex hull and sort, a little piece of magic from:
				// B.M.E. Moret & H.D. Shapiro "P to NP" pp. 453
				
				float dx, dy, tt ,theta, cen[2];  //tt= TempTheta
				cen[0] = cen[1] = 0.0;
				int next;
				//rather than swap 3 arrays, only one?
				int order[6] ={0,1,2,3,4,5};
				
				// order[6] could be an extreemly inefficient way to do this
				for( int j=0; j<edges; ++j){ //find the center of the points
					cen[0] += tpoly[j][0];
					cen[1] += tpoly[j][1];
				} //by averaging
				
				cen[0]/= edges;
				cen[1]/= edges;	 
				
				for(j=0; j<edges; ++j){ //for each vertex
					theta = -10;	       //find one with largest angle from center.. 
					next = j;
					for ( int k= j; k<edges; ++k){ 
						//... and check angle made between other edges
						dx = tpoly[order[k]][0] - cen[0];
						dy = tpoly[order[k]][1] - cen[1];
						if( (dx == 0) && (dy == 0)){ //same as center? 
							next = k;
							cout << "what teh " << endl;
							break; //out of this for-loop
						}
						tt = dy/(ABS(dx) + ABS(dy)); //else compute theta [0-4]
						if( dx < 0.0 ) tt = (float)(2.0 - tt); //check quadrants 2&3
						else if( dy < 0.0 ) tt = (float)(4.0 + tt); //quadrant 4
						if( theta <= tt ){  //grab the max theta
							next = k;
							theta = tt;
						}
					} //end for(k) angle checking
					// i am using 'tt' as a temp
					// swap polygon vertex ( is this better than another branch?)
					// I am not sure wich is worse: swapping 3 vectors for every edge
					// or: using an array to index into another array??? hmmm....
					//   should have payed more attention in class
					int tmp = order[j];
					order[j] = order[next];
					order[next] = tmp;
					
				} //end for(j) edge /angle sort
				
				glBegin(GL_POLYGON); //draw slice and texture map it
				{
					for(j=0; j< edges; ++j){
						glTexCoord3fv(tcoord[order[j]]);
						glVertex3fv(poly[order[j]]);
					}
				}
				glEnd();
				glFlush();
      }//end else compute convex hull
   }// end for(i) each slice
	 
#ifdef WIN32
#if    WILDCAT
   glDisable(GL_TEXTURE_COLOR_TABLE_EXT);
#endif
#else
   glDisable(GL_TEXTURE_COLOR_TABLE_SGI);
#endif
   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_3D_EXT);
   glEnable(GL_LIGHTING);
}


//========================================================================
//------------------------------------------------------------------------
//========================================================================

void VolumeRenderer::renderSlice(float quad[4][3], float alpha)
{
   switch(type){
   case	   VolRen2DTexture:
      cerr << "2D texture mapping method is not implemented" <<endl;
      return;
   case VolRen3DTexture:
      cerr << "3D texture mapping method (OGL 1.2) is not implemented" <<endl;
      return;
   case VolRen3DExt:
      render3dSliceEXT(quad, alpha);
      return;
   }
}

//------------------------------------------------------------------------


void VolumeRenderer::render3dSliceEXT(float quad[4][3], float a)
{
  glDisable(GL_LIGHTING); //light makes it look bad!

  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

  glPolygonMode(GL_FRONT, GL_FILL);
  glPolygonMode(GL_BACK, GL_FILL);

  glDisable(GL_TEXTURE_COLOR_TABLE_SGI);
  glEnable(GL_TEXTURE_3D_EXT);
  
  glBindTexture(GL_TEXTURE_3D_EXT, texname3D[0]);

  glColor4f(1, 1, 1, a);
  glBegin(GL_QUADS);
  {
    glNormal3f(0,0,1);

    glTexCoord3f(quad[1][0]/sizef[0],quad[1][1]/sizef[1],quad[1][2]/sizef[2]);
    glVertex3f(quad[1][0], quad[1][1], quad[1][2]);

    glTexCoord3f(quad[0][0]/sizef[0],quad[0][1]/sizef[1],quad[0][2]/sizef[2]);
    glVertex3f(quad[0][0],quad[0][1],quad[0][2]);

    glTexCoord3f(quad[2][0]/sizef[0],quad[2][1]/sizef[1],quad[2][2]/sizef[2]);
    glVertex3f(quad[2][0],quad[2][1],quad[2][2]);

    glTexCoord3f(quad[3][0]/sizef[0],quad[3][1]/sizef[1],quad[3][2]/sizef[2]);
    glVertex3f(quad[3][0],quad[3][1],quad[3][2]);
  }
  glEnd();

  glEnable(GL_LIGHTING); //light makes it look bad!

  glDisable(GL_BLEND);
}

//========================================================================
//------------------------------------------------------------------------
//========================================================================

inline void VolumeRenderer::initVol(int v)
{
   size[0] = volume[v]->xiSize;
   size[1] = volume[v]->yiSize;
   size[2] = volume[v]->ziSize;

   float maxsize = MAX(size[0], size[1]);
   maxsize = MAX(maxsize, size[2]);

   //sizef [0-1] ? usualy but could be arbitrary physical units (cm, in, ft)
   sizef[0] = volume[v]->xfSize; //AFFINE(0, size[0], maxsize, 0, 1);
   sizef[1] = volume[v]->yfSize; //AFFINE(0, size[1], maxsize, 0, 1);
   sizef[2] = volume[v]->zfSize; //AFFINE(0, size[2], maxsize, 0, 1);

   center[0] = volume[v]->xfPos;
   center[1] = volume[v]->yfPos;
   center[2] = volume[v]->zfPos;
}

//------------------------------------------------------------------------

int VolumeRenderer::glVolErr(const char *place)
{
   GLenum errCode;
   const GLubyte *errString;
   
   if((errCode = glGetError()) != GL_NO_ERROR){
      errString = gluErrorString(errCode);
      cerr << endl << "OpenGL error : VolumeRenderer::" << place << " : " 
	   << (char *) errString << endl << endl;
      return 1;
   }
   return 0;
}

//------------------------------------------------------------------------

void VolumeRenderer::renderBBox()
{
   //glDisable(GL_LIGHTING);
   glDisable(GL_TEXTURE_3D_EXT);
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_TEXTURE_3D_EXT);
   glDisable(GL_LIGHTING);

   glPolygonMode(GL_FRONT, GL_LINE);
   glPolygonMode(GL_BACK, GL_LINE);
   glLineWidth(3);

   //glBindTexture(GL_TEXTURE_2D, 0);
   glColor3f(.35, 0, .1);
   glBegin(GL_QUADS);
   {
      glVertex3f(0, 0, 0);
      glVertex3f(1, 0, 0);
      glVertex3f(1, 1, 0);
      glVertex3f(0, 1, 0);

      glVertex3f(0, 0, 1);
      glVertex3f(1, 0, 1);
      glVertex3f(1, 1, 1);
      glVertex3f(0, 1, 1);

      glVertex3f(0, 0, 0);
      glVertex3f(0, 0, 1);
      glVertex3f(0, 1, 1);
      glVertex3f(0, 1, 0);

      glVertex3f(1, 0, 0);
      glVertex3f(1, 0, 1);
      glVertex3f(1, 1, 1);
      glVertex3f(1, 1, 0);
   }
   glEnd();

   glLineWidth(1);

   glPolygonMode(GL_FRONT, GL_FILL);
   glPolygonMode(GL_BACK, GL_FILL);
   glEnable(GL_LIGHTING);
}

//------------------------------------------------------------------------

void VolumeRenderer::renderBBox(float xb, float yb, float zb)
{
   //glDisable(GL_LIGHTING);
   glDisable(GL_TEXTURE_3D_EXT);
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_TEXTURE_3D_EXT);
   glDisable(GL_LIGHTING);

   glPolygonMode(GL_FRONT, GL_LINE);
   glPolygonMode(GL_BACK, GL_LINE);
   glLineWidth(3);

   //glBindTexture(GL_TEXTURE_2D, 0);
   glColor3f(.35, 0, .1);

   glBegin(GL_QUADS);
   {
      glVertex3f(0, 0, 0);
      glVertex3f(xb, 0, 0);
      glVertex3f(xb, yb, 0);
      glVertex3f(0, yb, 0);
      
      glVertex3f(0, 0, zb);
      glVertex3f(xb, 0, zb);
      glVertex3f(xb, yb, zb);
      glVertex3f(0, yb, zb);

      glVertex3f(0, 0, 0);
      glVertex3f(0, 0, zb);
      glVertex3f(0, yb, zb);
      glVertex3f(0, yb, 0);

      glVertex3f(xb, 0, 0);
      glVertex3f(xb, 0, zb);
      glVertex3f(xb, yb, zb);
      glVertex3f(xb, yb, 0);
   }
   glEnd();

   glLineWidth(1);

   glPolygonMode(GL_FRONT, GL_FILL);
   glPolygonMode(GL_BACK, GL_FILL);
   glEnable(GL_LIGHTING);
}

//--------------------------------------------------------------------------
void VolumeRenderer::renderBBoxBrackets(float xb, float yb, float zb)
{
      //glDisable(GL_LIGHTING);
   //glDisable(GL_TEXTURE_3D);
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_TEXTURE_3D_EXT);
   glDisable(GL_LIGHTING);

   glPolygonMode(GL_FRONT, GL_LINE);
   glPolygonMode(GL_BACK, GL_LINE);
   glLineWidth(3);

   glColor3f(.35, 0, .1);
 
   drawLine(0, 0, 0,   0+xb/10, 0, 0);
   drawLine(0, 0, 0,   0, 0+yb/10, 0);
   drawLine(0, 0, 0,   0, 0, 0+zb/10);

   drawLine(xb, 0, 0,	xb-xb/10, 0, 0);
   drawLine(xb, 0, 0,	xb, 0+yb/10, 0);
   drawLine(xb, 0, 0,	xb, 0, 0+zb/10);

   drawLine(0, yb, 0,	0+xb/10, yb, 0);
   drawLine(0, yb, 0,	0, yb-yb/10, 0);
   drawLine(0, yb, 0,	0, yb, 0+zb/10);

   drawLine(0, 0, zb,	0+xb/10, 0, zb);
   drawLine(0, 0, zb,	0, 0+yb/10, zb);
   drawLine(0, 0, zb,	0, 0, zb-zb/10);

   drawLine(xb, yb, 0,	 xb-xb/10, yb, 0);
   drawLine(xb, yb, 0,	 xb, yb-yb/10, 0);
   drawLine(xb, yb, 0,	 xb, yb, 0+zb/10);

   drawLine(xb, 0, zb,	 xb-xb/10, 0, zb);
   drawLine(xb, 0, zb,	 xb, 0+yb/10, zb);
   drawLine(xb, 0, zb,	 xb, 0, zb-zb/10);

   drawLine(0, yb, zb,	 0+xb/10, yb, zb);
   drawLine(0, yb, zb,	 0, yb-yb/10, zb);
   drawLine(0, yb, zb,	 0, yb, zb-zb/10);

   drawLine(xb, yb, zb,	  xb-xb/10, yb, zb);
   drawLine(xb, yb, zb,	  xb, yb-yb/10, zb);
   drawLine(xb, yb, zb,	  xb, yb, zb-zb/10);

   glLineWidth(1);
   
   glPolygonMode(GL_FRONT, GL_FILL);
   glPolygonMode(GL_BACK, GL_FILL);
   glEnable(GL_LIGHTING);
}

//========================================================================
//--------------------------------------------------------------------------
//========================================================================

#if USE_NRRD
int VolumeRenderer::loadNRRD_Volume(const char *volname)
{
   FILE *fin;
   char *err;
   //int axis, pos;
   Nrrd *nin;

   //get file handle for the volume we want to read
   if (!(fin = fopen(volname, "r"))){
      cerr << "VolumeRenderer.cpp : loadNRRD_Volume() : Couldn't open : " 
	   << volname 
	   << " for reading\n";
      return 1;
   }

   //get nrrd handle for the volume
   if (!(nin = nrrdNewRead(fin))) {
      err = nrrdStrdupErr();
      cerr << "genVol.cc : loadNRRD_Volume() : error reading nrrd : " 
	   << err << "\n";
      return 1;
   }
   
   //note the hack here: assumes that the type is always uchar!!!!!!
   //also assumes that the texture is always a power of two.
   //conversion routiens do exist to fix this-
   nrrd_vol = nrrd_uchar = nin;

   data = (unsigned char*) nrrd_uchar->data;
   size[0] = nrrd_uchar->size[0];
   size[1] = nrrd_uchar->size[1];
   size[2] = nrrd_uchar->size[2];

   cerr << "Successfuly read " << volname << endl;
   return 0;
}
#endif


//==========================================================================
//---------- Inlines -------------------------------------------------------
//==========================================================================


inline void VolumeRenderer::translateV3(GLfloat out[3], 
					GLdouble mat[16], 
					GLfloat in[3])
{
   out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2];// + mat[12];
   out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2];// + mat[13];
   out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2];// + mat[14];
}
//--------------------------------------------------------------------------
inline GLfloat VolumeRenderer::dotV3(GLfloat one[4], GLfloat two[4])
{
   return one[0]*two[0] + one[1]*two[1] + one[2]*two[2];
}
//--------------------------------------------------------------------------
inline void VolumeRenderer::identityMatrix(GLdouble m[16])
{
   m[0]=1; m[4]=0; m[8]= 0; m[12]=0;
   m[1]=0; m[5]=1; m[9]= 0; m[13]=0;
   m[2]=0; m[6]=0; m[10]=1; m[14]=0;
   m[3]=0; m[7]=0; m[11]=0; m[15]=1;
}
//--------------------------------------------------------------------------   
inline int VolumeRenderer::intersect(
		     const float p0[3], const float p1[3], //line end points
		     const float t0[3], const float t1[3], //texture points
		     const float v0[3], const float v1[3], //view coord points
		     const float sp[3], const float sn[3], //plane point & norm
		     float pnew[3], float tnew[3], float vnew[3]) //new values 
{
   //t = (sn.(sp - p0))/(sn.(p1 - p0))
   float t = ((sn[0]*(sp[0] - p0[0]) + sn[1]*(sp[1] - p0[1]) 
	       + sn[2]*(sp[2] - p0[2])) / 
	      (sn[0]*(p1[0] - p0[0]) + sn[1]*(p1[1] - p0[1])
	       + sn[2]*(p1[2] - p0[2])));
   //note if the denominator is zero t is a NAN so we should have no problems?
   
   if( (t>=0) && (t<=1) ){
      //compute line intersection
      pnew[0] = p0[0] + t*(p1[0] - p0[0]); 
      pnew[1] = p0[1] + t*(p1[1] - p0[1]); 
      pnew[2] = p0[2] + t*(p1[2] - p0[2]);
      //compute texture interseciton
      tnew[0] = t0[0] + t*(t1[0] - t0[0]); 
      tnew[1] = t0[1] + t*(t1[1] - t0[1]); 
      tnew[2] = t0[2] + t*(t1[2] - t0[2]);
      //compute view coordinate intersections
      vnew[0] = v0[0] + t*(v1[0] - v0[0]); 
      vnew[1] = v0[1] + t*(v1[1] - v0[1]); 
      vnew[2] = v0[2] + t*(v1[2] - v0[2]);
      return 1;
   }
   return 0;
} 
//--------------------------------------------------------------------------
void VolumeRenderer::inverseMatrix( GLdouble invm[16], const GLdouble m[16] )
{
   GLfloat det = 
      m[0]*m[5]*m[10]-
      m[0]*m[6]*m[9]-
      m[1]*m[4]*m[10]+
      m[1]*m[6]*m[8]+
      m[2]*m[4]*m[9]-
      m[2]*m[5]*m[8];
   
   invm[0] = (m[5]*m[10]-m[6]*m[9])/det;
   invm[1] = (-m[1]*m[10]+m[2]*m[9])/det;
   invm[2] = (m[1]*m[6]-m[2]*m[5])/det;
   invm[3] = 0.0;
   
   invm[4] = (-m[4]*m[10]+m[6]*m[8])/det;
   invm[5] = (m[0]*m[10]-m[2]*m[8])/det;
   invm[6] = (-m[0]*m[6]+m[2]*m[4])/det;
   invm[7] = 0.0;
   
   invm[8] = (m[4]*m[9]-m[5]*m[8])/det;
   invm[9] = (-m[0]*m[9]+m[1]*m[8])/det;
   invm[10] = (m[0]*m[5]-m[1]*m[4])/det;
   invm[11] = 0.0;
   
   invm[12] = (-m[4]*m[9]*m[14]+m[4]*m[13]*m[10]+
	       m[5]*m[8]*m[14]-m[5]*m[12]*m[10]-
	       m[6]*m[8]*m[13]+m[6]*m[12]*m[9])/det;
   invm[13] = (m[0]*m[9]*m[14]-m[0]*m[13]*m[10]-
	       m[1]*m[8]*m[14]+m[1]*m[12]*m[10]+
	       m[2]*m[8]*m[13]-m[2]*m[12]*m[9])/det;
   invm[14] = (-m[0]*m[5]*m[14]+m[0]*m[13]*m[6]+
	       m[1]*m[4]*m[14]-m[1]*m[12]*m[6]-
	       m[2]*m[4]*m[13]+m[2]*m[12]*m[5])/det;
   invm[15] = 1.0;
}
//--------------------------------------------------------------------------
inline void VolumeRenderer::printV(float v[3]){
   cerr << v[0] << ", " << v[1] << ", " << v[2] << endl;
}
//--------------------------------------------------------------------------
inline void VolumeRenderer::drawLine(float x0, float y0, float z0, 
				     float x1, float y1, float z1)
{
   glBegin(GL_LINES);{
      glVertex3f(x0, y0, z0);
      glVertex3f(x1, y1, z1);
   } glEnd();	
}

//--------------------------------------------------------------------------   










