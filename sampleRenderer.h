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
//
// sampleRenderer.h: interface for the sampleRenderer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPLERENDERER_H__E9D8779A_02C5_4445_B5AC_433D88B8D153__INCLUDED_)
#define AFX_SAMPLERENDERER_H__E9D8779A_02C5_4445_B5AC_433D88B8D153__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gluvv.h"
#include "gluvvPrimitive.h"

class sampleRenderer : public gluvvPrimitive  
{
public:
	sampleRenderer();
	virtual ~sampleRenderer();

	void draw();

};

#endif // !defined(AFX_SAMPLERENDERER_H__E9D8779A_02C5_4445_B5AC_433D88B8D153__INCLUDED_)
