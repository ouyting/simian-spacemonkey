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
// VolumeRenderable.h: interface for the VolumeRenderable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOLUMERENDERABLE_H__24608745_6DD6_48D4_AA5D_AE8B79E48437__INCLUDED_)
#define AFX_VOLUMERENDERABLE_H__24608745_6DD6_48D4_AA5D_AE8B79E48437__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gluvvPrimitive.h"
#include "VolumeRenderer.h"
#include "TLUT.h"

class VolumeRenderable : public gluvvPrimitive  
{
public:
	VolumeRenderable();
	virtual ~VolumeRenderable();

	void draw();	
	void init();

private:
	VolumeRenderer *volren;
	TLUT tlut;

};

#endif // !defined(AFX_VOLUMERENDERABLE_H__24608745_6DD6_48D4_AA5D_AE8B79E48437__INCLUDED_)
