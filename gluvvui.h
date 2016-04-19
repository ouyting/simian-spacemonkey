//------------------------------------------------------------------------
// gluvvui.h
//   Joe Kniss
//      a user interface for gluvv state
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
//                   <<< jmk@cs.utah.edu >>>
//               "All Your Base are Belong to Us"
//-------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLUVVUI_H__0F59D5EE_EACE_48EE_BF27_476583F9441D__INCLUDED_)
#define AFX_GLUVVUI_H__0F59D5EE_EACE_48EE_BF27_476583F9441D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "glui/glui.h"

class gluvvui  
{
public:
	gluvvui();
	virtual ~gluvvui();
	
	int init();
	
	void show();
	void hide();
  int  visible();

	void control_cb(int control);


private:
	enum{
		_unknown,
		_close,
		_brushgrp,
		_ddpaint,
		_dddrop,
		_ddclear,
		_ddsize,
		_shadegrp,
		_ambient,
		_lightRad,
		_ltatt,
		_ltattthresh,
		_ltattnear,	
		_ltattfar,
		_shadowchk,
		_softchk,
		_gshadowqual,
		_ishadowqual,
    _pertW1,
    _pertW2,
    _pertW3,
    _pertW4,
    _pertS1,
    _pertS2,
    _pertS3,
    _pertS4,
		_fogcolgrp,
		_fogthresh,
		_fognear,
		_fogfar,
		_lightchk,
		_sillchk,
		_showltchk,
		_interactSR,
		_goodSR,
		_clipgrp,
		_histochk,
		_normpostBtn,
		_normpreBtn,
		_bggrp

	};


	int on;
	GLUI            *glui_window;
	GLUI_Panel      *obj_panel;
	GLUI_RadioGroup *ddBrushes;
	GLUI_Spinner    *ddSizeSpin;
	GLUI_RadioGroup *Shaders;
	GLUI_Spinner    *lightRadSpin;
	GLUI_Spinner    *ambientSpin;
	GLUI_Checkbox   *Light;
	GLUI_Checkbox   *LtAtt;
	GLUI_Spinner    *LtAttThresh;
	GLUI_Spinner    *LtAttNear;
	GLUI_Spinner    *LtAttFar;
	GLUI_Checkbox   *Shadow;
	GLUI_Checkbox   *Soft;
	GLUI_Spinner    *GShadowQualSpin;
	GLUI_Spinner    *IShadowQualSpin;	
	GLUI_RadioGroup *FogColors;
	GLUI_Spinner    *FogThresh;
	GLUI_Spinner    *FogNear;
	GLUI_Spinner    *FogFar;
	GLUI_Checkbox   *Sill;
	GLUI_Checkbox   *showLT;
	GLUI_Spinner    *PertW1;
	GLUI_Spinner    *PertW2;
	GLUI_Spinner    *PertW3;
	GLUI_Spinner    *PertW4;
  GLUI_Spinner    *PertS1;
	GLUI_Spinner    *PertS2;
	GLUI_Spinner    *PertS3;
	GLUI_Spinner    *PertS4;
  GLUI_Spinner    *interactSpin;
	GLUI_Spinner    *goodSpin;
	GLUI_RadioGroup *Clips;
	GLUI_Checkbox   *Histo;
	GLUI_RadioGroup *bGround;

};


extern gluvvui          gui;


#endif // !defined(AFX_GLUVVUI_H__0F59D5EE_EACE_48EE_BF27_476583F9441D__INCLUDED_)
