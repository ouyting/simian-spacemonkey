//                   ________    ____   ___ 
//                  |        \  /    | /  /
//                  +---+     \/     |/  /
//                  +--+|  |\    /|     < 
//                  |  ||  | \  / |  |\  \ 
//                  |      |  \/  |  | \  \ 
//                   \_____|      |__|  \__\
//                       Copyright  2001 
//                      Joe Michael Kniss
//               "All Your Base are Belong to Us"
//
// VolumeRenderable.cpp: implementation of the VolumeRenderable class.
//
//////////////////////////////////////////////////////////////////////

#include "VolumeRenderable.h"
#include "gluvv.h"
#include "glUE.h"
#include <iostream.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VolumeRenderable::VolumeRenderable()
{
	volren = 0;
}

VolumeRenderable::~VolumeRenderable()
{

}


void VolumeRenderable::draw()
{
	if(!volren) return;

	glPushMatrix();{
		glTranslatef(gluvv.rinfo.trans[0], //translate
			           gluvv.rinfo.trans[1], 
								 gluvv.rinfo.trans[2]);
		glMultMatrixf(gluvv.rinfo.xform);  //rotate
		glTranslatef(-gluvv.mv->xfSize/2,  //center
			           -gluvv.mv->yfSize/2,
								 -gluvv.mv->zfSize/2);
		double mv[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, mv);
		(volren->getColorMap())->scaleAlpha(gluvv.volren.sampleRate);
		if(gluvv.volren.loadTLUT){
			gluvv.volren.loadTLUT = 0;
			(volren->getColorMap())->loadTransferTableRGBA();
		}
		volren->renderVolume(gluvv.volren.sampleRate, mv);
		GlErr("VolumeRenderable::" , "draw()");
	} glPopMatrix();
}

void VolumeRenderable::init()
{
	if(!gluvv.mv){
		cerr << "ERROR: VolumeRenderable::init(), no MetaVolume defined" << endl;
		return;
	}
	volren = new VolumeRenderer(gluvv.mv, 0);
	if(gluvv.mv->numSubVols == 1){
		volren->createVolume(VolRen3DExt, gluvv.mv->volumes);
	} else {
		volren->createVolume(VolRen3DExt, gluvv.mv->volumes, gluvv.mv->numSubVols);
	}
	volren->createTLUT();
	gluvv.volren.tlut = (volren->getColorMap());
	(volren->getColorMap())->rgbBlackBody();
	(volren->getColorMap())->rgbCyanMagenta();
	(volren->getColorMap())->rgbSpectral();
	(volren->getColorMap())->alphaRamp(0, 255, 0.0, 0.1);
	(volren->getColorMap())->loadTransferTableRGBA();
	//tlut.rgbBlackBody();
	//tlut.alphaRamp(0,255,0,1);
	//tlut.loadTransferTableRGBA();
}