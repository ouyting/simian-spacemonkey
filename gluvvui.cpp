//------------------------------------------------------------------------
// gluvvui.cpp
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
//////////////////////////////////////////////////////////////////////

#include "gluvvui.h"
#include "glui/glui.h"
#include "gluvv.h"
#include "VectorMath.h"
#include <iostream.h>

#define __VERSION_2 1
#define __VERSION_3 1

//======================================== Static ui callback
//===========================================================
void uicb(int control){
	cerr << "cb from " << control << endl;

	gui.control_cb(control);

}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

gluvvui::gluvvui()
{
	on = 0;
}

gluvvui::~gluvvui()
{

}

//================================================= INIT !!!!
//===========================================================
int gluvvui::init()
{
	glui_window = GLUI_Master.create_glui("Simian~ SpaceMonkey");

	  /*** Add invisible panel to hold rest of controls ***/
  GLUI_Panel *panel1 = glui_window->add_panel( "", GLUI_PANEL_NONE );

	//close the menu gui
	glui_window->add_button_to_panel(panel1, ">>>>>>>> Close <<<<<<<<", _close, uicb);


	//roll out for dual domain interaction --------------------------------------------------------
	GLUI_Rollout *roll1 = glui_window->add_rollout_to_panel(panel1, "Dual Domain Interaction", false);
	{
		//radio group for brushes
		ddBrushes = glui_window->add_radiogroup_to_panel(roll1,(int*)&gluvv.probe.brush, _brushgrp, uicb);
		glui_window->add_radiobutton_to_group(ddBrushes, "OFF (k)");
		glui_window->add_radiobutton_to_group(ddBrushes, "Ellipse (b)");
		glui_window->add_radiobutton_to_group(ddBrushes, "Auto Ellipse (a)");
		glui_window->add_radiobutton_to_group(ddBrushes, "Triangle (t)");
		glui_window->add_radiobutton_to_group(ddBrushes, "1D (O)");
		glui_window->add_radiobutton_to_group(ddBrushes, "Auto 1D (o)");
		//column
		glui_window->add_column_to_panel(roll1, true);
		//buttons for clear and paint
		glui_window->add_button_to_panel(roll1, "Paint (spc)", _ddpaint, uicb);
		glui_window->add_button_to_panel(roll1, "Drop (d)", _dddrop, uicb);
		glui_window->add_button_to_panel(roll1, "Clear (n)", _ddclear, uicb);
		//spinner for brush size
		ddSizeSpin = 
			glui_window->add_spinner_to_panel(roll1, "Size", 
			GLUI_SPINNER_FLOAT, 
			NULL, _ddsize, uicb);
		ddSizeSpin->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
		ddSizeSpin->set_float_val(gluvv.probe.slider);
	}//end dual domain -----------------------------------------------------------------------------

	//roll out for shading ------------------------------------------------------------------------
	GLUI_Rollout *roll2 = glui_window->add_rollout_to_panel(panel1, "Shading", false); 
	{
		//radio group for shading
		Shaders = glui_window->add_radiogroup_to_panel(roll2,(int*)&gluvv.probe.brush, _shadegrp, uicb);
		glui_window->add_radiobutton_to_group(Shaders, "Emmisive (F9)");
		glui_window->add_radiobutton_to_group(Shaders, "Shaded (F10)");
		glui_window->add_radiobutton_to_group(Shaders, "Faux (F11)");
		Shaders->set_int_val(2);
		// ambiant contribution
		ambientSpin = 
			glui_window->add_spinner_to_panel(roll2, "Ambient", 
			GLUI_SPINNER_FLOAT, 
			NULL, _ambient, uicb);
		ambientSpin->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
		ambientSpin->set_float_val(gluvv.light.amb);
		
		//lighting
		GLUI_Rollout *rollLight = glui_window->add_rollout_to_panel(roll2, "Lights", false);
		{
			//light radius
			lightRadSpin = 
				glui_window->add_spinner_to_panel(rollLight, "Light Radius", 
				GLUI_SPINNER_FLOAT, 
				NULL, _lightRad, uicb);
			lightRadSpin->set_float_limits(.1, 1000, GLUI_LIMIT_CLAMP);
			lightRadSpin->set_float_val(normV3(gluvv.light.pos));
			//check box for light
			Light = glui_window->add_checkbox_to_panel(rollLight, "Light Widget", NULL, _lightchk, uicb);

			LtAtt = glui_window->add_checkbox_to_panel(rollLight, "Light Attinuation", NULL, _ltatt, uicb);

			LtAttThresh = 
				glui_window->add_spinner_to_panel(rollLight, "Light Attinuation", 
				GLUI_SPINNER_FLOAT, 
				NULL, _ltattthresh, uicb);
			LtAttThresh->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
			LtAttThresh->set_float_val(1.0 - gluvv.light.lattThresh);
			
			LtAttNear = 
				glui_window->add_spinner_to_panel(rollLight, "Attinuation Start", 
				GLUI_SPINNER_FLOAT, 
				NULL, _ltattnear, uicb);
			LtAttNear->set_float_limits(0, 10, GLUI_LIMIT_CLAMP);
			LtAttNear->set_float_val(gluvv.light.lattLimits[0]*2.0);
			
			LtAttFar = 
				glui_window->add_spinner_to_panel(rollLight, "Attinuation Finish", 
				GLUI_SPINNER_FLOAT, 
				NULL, _ltattfar, uicb);
			LtAttFar->set_float_limits(-10, 0, GLUI_LIMIT_CLAMP);
			LtAttFar->set_float_val(gluvv.light.lattLimits[1]*2.0);
		} //end lighting
#if __VERSION_3
		//shadow qualities
		GLUI_Rollout *rollShad = glui_window->add_rollout_to_panel(roll2, "Shadows", false);
		{	
			//check box for shadows
			Shadow = glui_window->add_checkbox_to_panel(rollShad, "Shadows", NULL, _shadowchk, uicb);
			Shadow->set_int_val(gluvv.light.shadow);
			Soft = glui_window->add_checkbox_to_panel(rollShad, "Soft Shadows", NULL, _softchk, uicb);
			Soft->set_int_val(gluvv.light.softShadow);
			showLT = glui_window->add_checkbox_to_panel(rollShad, "Light View", NULL, _showltchk, uicb);
			showLT->set_int_val(gluvv.light.showView);
			
			GShadowQualSpin = 
				glui_window->add_spinner_to_panel(rollShad, "Good Shadow Quality", 
				GLUI_SPINNER_FLOAT, 
				NULL, _gshadowqual, uicb);
			GShadowQualSpin->set_float_limits(.1, 1, GLUI_LIMIT_CLAMP);
			GShadowQualSpin->set_float_val(gluvv.light.gShadowQual);
			IShadowQualSpin = 
				glui_window->add_spinner_to_panel(rollShad, "Interactive Shadow Quality", 
				GLUI_SPINNER_FLOAT, 
				NULL, _ishadowqual, uicb);
			IShadowQualSpin->set_float_limits(.1, 1, GLUI_LIMIT_CLAMP);
			IShadowQualSpin->set_float_val(gluvv.light.iShadowQual);
		} //end shadows
#endif
		
		//fog
		GLUI_Rollout *rollFog = glui_window->add_rollout_to_panel(roll2, "Fog", false);
		{
			GLUI_Panel *panelFC = glui_window->add_panel_to_panel(rollFog, "Fog Color");
			FogColors = glui_window->add_radiogroup_to_panel(panelFC, 0, _fogcolgrp, uicb);
			glui_window->add_radiobutton_to_group(FogColors, "Fog Off");
			glui_window->add_radiobutton_to_group(FogColors, "Blue");
			glui_window->add_radiobutton_to_group(FogColors, "White");
			glui_window->add_radiobutton_to_group(FogColors, "Black");
			
			FogThresh = 
				glui_window->add_spinner_to_panel(rollFog, "Fog Intensity", 
				GLUI_SPINNER_FLOAT, 
				NULL, _fogthresh, uicb);
			FogThresh->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
			FogThresh->set_float_val(1.0 - gluvv.light.fogThresh);
			
			FogNear = 
				glui_window->add_spinner_to_panel(rollFog, "Fog Start", 
				GLUI_SPINNER_FLOAT, 
				NULL, _fognear, uicb);
			FogNear->set_float_limits(0, 10, GLUI_LIMIT_CLAMP);
			FogNear->set_float_val(gluvv.light.fogLimits[0]*2.0);
			
			FogFar = 
				glui_window->add_spinner_to_panel(rollFog, "Fog Finish", 
				GLUI_SPINNER_FLOAT, 
				NULL, _fogfar, uicb);
			FogFar->set_float_limits(-10, 0, GLUI_LIMIT_CLAMP);
			FogFar->set_float_val(gluvv.light.fogLimits[1]*2.0);
		} //end fog


		//silhouette edges
		Sill = glui_window->add_checkbox_to_panel(roll2, "Silhouette Edges", NULL, _sillchk, uicb);
		Sill->set_int_val(gluvv.light.sill);
	}
	//end shading ----------------------------------------------------------------------------------

	//perturbation ---------------------------------------------------------------------------------
	GLUI_Rollout *rollpert = glui_window->add_rollout_to_panel(panel1, "Perturbation", false);
  {
			PertW1 = 
				glui_window->add_spinner_to_panel(rollpert, "Weight 1", 
				GLUI_SPINNER_FLOAT, 
				NULL, _pertW1, uicb);
			PertW1->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
			PertW1->set_float_val(.2);
      
      PertS1 = 
				glui_window->add_spinner_to_panel(rollpert, "Scale 1", 
				GLUI_SPINNER_FLOAT, 
				NULL, _pertS1, uicb);
			PertS1->set_float_limits(0, 65, GLUI_LIMIT_CLAMP);
			PertS1->set_float_val(.2);

      PertW2 = 
				glui_window->add_spinner_to_panel(rollpert, "Weight 2", 
				GLUI_SPINNER_FLOAT, 
				NULL, _pertW2, uicb);
			PertW2->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
			PertW2->set_float_val(0);

      PertS2 = 
				glui_window->add_spinner_to_panel(rollpert, "Scale 2", 
				GLUI_SPINNER_FLOAT, 
				NULL, _pertS2, uicb);
			PertS2->set_float_limits(0, 65, GLUI_LIMIT_CLAMP);
			PertS2->set_float_val(2.1);

      PertW3 = 
				glui_window->add_spinner_to_panel(rollpert, "Weight 3", 
				GLUI_SPINNER_FLOAT, 
				NULL, _pertW3, uicb);
			PertW3->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
			PertW3->set_float_val(0);

      PertS3 = 
				glui_window->add_spinner_to_panel(rollpert, "Scale 3", 
				GLUI_SPINNER_FLOAT, 
				NULL, _pertS3, uicb);
			PertS3->set_float_limits(0, 65, GLUI_LIMIT_CLAMP);
			PertS3->set_float_val(4.5);

      PertW4 = 
				glui_window->add_spinner_to_panel(rollpert, "Weight 4", 
				GLUI_SPINNER_FLOAT, 
				NULL, _pertW4, uicb);
			PertW4->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
			PertW4->set_float_val(0);

      PertS4 = 
				glui_window->add_spinner_to_panel(rollpert, "Scale 4", 
				GLUI_SPINNER_FLOAT, 
				NULL, _pertS4, uicb);
			PertS4->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
			PertS4->set_float_val(8.7);
  }
	//end perturbation -----------------------------------------------------------------------------

	//clipping planes ------------------------------------------------------------------------------
	GLUI_Rollout *roll3 = glui_window->add_rollout_to_panel(panel1, "Clipping Plane", false);
	Clips = glui_window->add_radiogroup_to_panel(roll3,NULL, _clipgrp, uicb);
	glui_window->add_radiobutton_to_group(Clips, "OFF");
	glui_window->add_radiobutton_to_group(Clips, "X positive");
	glui_window->add_radiobutton_to_group(Clips, "X negative");
	glui_window->add_radiobutton_to_group(Clips, "Y positive");
	glui_window->add_radiobutton_to_group(Clips, "Y negative");
	glui_window->add_radiobutton_to_group(Clips, "Z positive");
	glui_window->add_radiobutton_to_group(Clips, "Z negative");
	//end clipping --------------------------------------------------------------------------------

	//sample rates --------------------------------------------------------------------------------
	GLUI_Panel *panel2 = glui_window->add_panel_to_panel(panel1, "Sample Rate");
	interactSpin = 
		glui_window->add_spinner_to_panel(panel2, "Interactive", 
		                                  GLUI_SPINNER_FLOAT, 
																			NULL, _interactSR, uicb);
	interactSpin->set_float_limits(0, 20, GLUI_LIMIT_CLAMP);
	interactSpin->set_float_val(gluvv.volren.interactSamp);
	goodSpin = 
		glui_window->add_spinner_to_panel(panel2, "High Quality", 
		                                  GLUI_SPINNER_FLOAT, 
																			NULL, _goodSR, uicb);
	goodSpin->set_float_limits(0, 20, GLUI_LIMIT_CLAMP);
	goodSpin->set_float_val(gluvv.volren.goodSamp);
	//end sample rates ---------------------------------------------------------------------------

	//histogram
	Histo = glui_window->add_checkbox_to_panel(panel1, "Histogram", NULL, _histochk, uicb);

	//back ground color --------------------------------------------------------------------------
	GLUI_Panel *panelBG = glui_window->add_panel_to_panel(panel1, "Background Color");
	bGround = glui_window->add_radiogroup_to_panel(panelBG,0, _bggrp, uicb);
	glui_window->add_radiobutton_to_group(bGround, "White");
	glui_window->add_radiobutton_to_group(bGround, "Black");
	//end back ground color ----------------------------------------------------------------------

	//normal computation buttons -----------------------------------------------------------------
	glui_window->add_button_to_panel(panel1, "Post Classification Normals", _normpostBtn, uicb);
	glui_window->add_button_to_panel(panel1, "Pre Classification Normals", _normpreBtn, uicb);
	//use normal computation buttons -------------------------------------------------------------

	on = 1;
	return 0;
}

//====================================================== Show
//===========================================================
void gluvvui::show()
{
	init();
	on = 1;
}

//====================================================== Hide
//===========================================================
void gluvvui::hide()
{
	//glutSetWindow(gluvv.mainWindow);
	glui_window->close();
	//GLUI_Master.close_all();
	on = 0;
	glutSetWindow(gluvv.mainWindow);
}

//=================================================== Visible
//===========================================================
int  gluvvui::visible()
{

	return on;
}

//========================================== Control callback
//===========================================================
void gluvvui::control_cb(int control)
{
	if(!on) return;
	
	int val;
	
	glutSetWindow(gluvv.mainWindow);

	switch(control){
	case _close:
		hide();
		break;

	//------------------------------------------------------ brushes
	case _brushgrp: //brush select
		val = ddBrushes->get_int_val();
		cerr << "brush group id = " << val << endl;
		switch(val){ //brush type-----
		case 0: //off
			gluvv.tf.brushon = 0;
			gluvv.tf.brushon = 0;
			gluvv.tf.loadme = 1;
			glutPostRedisplay();
			break;
		case 1: //ellipse brush
			gluvv.tf.brushon = 1;
			gluvv.probe.brush = EllipseBrush;
			gluvv.tf.loadme = 1; 
			glutPostRedisplay();
			break;
		case 2: //auto ellipse
			gluvv.tf.brushon = 1;
			//gluvv.probe.slider = .5;
			gluvv.probe.brush = AutoEllipseBrush;
			gluvv.tf.loadme = 1; 
			glutPostRedisplay();
			break;
		case 3: //triangle brush
			gluvv.tf.brushon = 1;
			//gluvv.probe.slider = .5;
			gluvv.probe.brush = TriangleBrush;
			gluvv.tf.loadme = 1; 
			glutPostRedisplay();
			break;
		case 4:
			gluvv.tf.brushon = 1;
			//gluvv.probe.slider = .5;
			gluvv.probe.brush = OneDBrush;
			gluvv.tf.loadme = 1; 
			glutPostRedisplay();
		break;
		case 5: //one d
			gluvv.tf.brushon = 1;
			//gluvv.probe.slider = .5;
			gluvv.probe.brush = AutoOneDBrush;
			gluvv.tf.loadme = 1; 
			glutPostRedisplay();
			break;
		}//----------------------------
		break;

		//---------------------------------------------------- painting
		case _ddpaint: //paint
			gluvv.tf.paintme = 1;
			glutPostRedisplay();
			break;
		case _dddrop: //drop
			gluvv.tf.dropme = 1;
			glutPostRedisplay();
			break;
		case _ddclear: //clear
			gluvv.tf.clearpaint = 1;
			glutPostRedisplay();
			break;
		case _ddsize: //brush size
			gluvv.probe.slider = 1.0 - ddSizeSpin->get_float_val();
			gluvv.tf.loadme = 1;
			glutPostRedisplay();
			break;

		//---------------------------------------------------- shading
		case _shadegrp: //shade mode
			val = Shaders->get_int_val();
			switch(val){  //select shade mode
			case 0: //emmisive
				gluvv.shade = gluvvShadeAmb;
				gluvv.tf.loadme = 1;
				glutPostRedisplay();
				break;
			case 1: //shaded
				gluvv.shade = gluvvShadeDSpec;
				gluvv.tf.loadme = 1;
				glutPostRedisplay();
				break;
			case 2: //faux
				gluvv.shade = gluvvShadeFaux;
				gluvv.tf.loadme = 1;
				glutPostRedisplay();
				break;
			}
			break;
		case _ambient:
			gluvv.light.amb = ambientSpin->get_float_val();
			glutPostRedisplay();
			break;
		//Lighting --------------------
		case _lightRad:
			cerr << "setting light rad" << endl;
			normalizeV3(gluvv.light.pos);
			scaleV3(lightRadSpin->get_float_val(), gluvv.light.pos);
			ltwr.genXForm();
			glutPostRedisplay();
			break;
		case _lightchk:
			ltwr.onoff();
			glutPostRedisplay();
			break;
		case _ltatt:
			gluvv.light.latt = LtAtt->get_int_val();
			glutPostRedisplay();
			break;
		case _ltattthresh:
			gluvv.light.lattThresh = 1.0 - LtAttThresh->get_float_val();
			glutPostRedisplay();
			break;
		case _ltattnear:
			gluvv.light.lattLimits[0] = LtAttNear->get_float_val()/2;
			glutPostRedisplay();
			break;
		case _ltattfar:
			gluvv.light.lattLimits[1] = LtAttFar->get_float_val()/2;
			glutPostRedisplay();
			break;
		//end Lighting ----------------
		//Shadows ---------------------
		case _shadowchk:
			gluvv.light.shadow = Shadow->get_int_val();
			glutPostRedisplay();
			break;
		case _softchk:
			gluvv.light.softShadow = Soft->get_int_val();
			glutPostRedisplay();
			break;
		case _gshadowqual:
			gluvv.light.gShadowQual = GShadowQualSpin->get_float_val();
			glutPostRedisplay();
			break;
		case _ishadowqual:
			gluvv.light.iShadowQual = IShadowQualSpin->get_float_val();			
			glutPostRedisplay();
			break;
		case _showltchk:
			gluvv.light.showView = showLT->get_int_val();
			glutPostRedisplay();
			break;
		//end Shadows -----------------
		//Silhouettes -----------------
		case _sillchk:
			gluvv.light.sill = Sill->get_int_val();
			glutPostRedisplay();
			break;
		//Fog -------------------------
		case _fogcolgrp:
			switch(FogColors->get_int_val()){
			case 0:
				gluvv.light.fog = 0;
				break;
			case 1:
				gluvv.light.fog = 1;
				gluvv.light.fogColor[0] = 0;
				gluvv.light.fogColor[1] = 0;
				gluvv.light.fogColor[2] = .5;
				break;
			case 2:
				gluvv.light.fog = 1;
				gluvv.light.fogColor[0] = .9;
				gluvv.light.fogColor[1] = .9;
				gluvv.light.fogColor[2] = .9;
				break;
			case 3:
				gluvv.light.fog = 1;
				gluvv.light.fogColor[0] = .1;
				gluvv.light.fogColor[1] = .1;
				gluvv.light.fogColor[2] = .1;				
				break;
			}
			glutPostRedisplay();
			break;
		case _fogthresh:
			gluvv.light.fogThresh = 1.0 - FogThresh->get_float_val();
			glutPostRedisplay();
			break;
		case _fognear:
			gluvv.light.fogLimits[0] = FogNear->get_float_val()/2;
			glutPostRedisplay();
			break;
		case _fogfar:
			gluvv.light.fogLimits[1] = FogFar->get_float_val()/2;
			glutPostRedisplay();
			break;
		//end Fog ---------------------
		//---------------------------------------------------- perturbation
    case _pertW1:
      gluvv.pert.weights[0] = PertW1->get_float_val();
			glutPostRedisplay();
      break;
    case _pertS1:
      gluvv.pert.scales[0] = PertS1->get_float_val();
			glutPostRedisplay();
      break;
    case _pertW2:
      gluvv.pert.weights[1] = PertW2->get_float_val();
			glutPostRedisplay();
      break;
    case _pertS2:
      gluvv.pert.scales[1] = PertS2->get_float_val();
			glutPostRedisplay();
      break;
    case _pertW3:
      gluvv.pert.weights[2] = PertW3->get_float_val();
			glutPostRedisplay();
      break;
    case _pertS3:
      gluvv.pert.scales[2] = PertS3->get_float_val();
			glutPostRedisplay();
      break;
    case _pertW4:
      gluvv.pert.weights[3] = PertW4->get_float_val();
			glutPostRedisplay();
      break;
    case _pertS4:
      gluvv.pert.scales[3] = PertS4->get_float_val();
			glutPostRedisplay();
      break;
    //---------------------------------------------------- sample rates
    case _interactSR: //interactive sample rate
			gluvv.volren.interactSamp = interactSpin->get_float_val();
			glutPostRedisplay();
			break;
		case _goodSR:  //high quality sample rate
			gluvv.volren.goodSamp = goodSpin->get_float_val();
			glutPostRedisplay();
			break;
		
		//---------------------------------------------------- clipping
		case _clipgrp: //clipping plane selection
			val = Clips->get_int_val();
			switch(val){
			case 0: //off
				gluvv.clip.on = 0;
				glutPostRedisplay();
				break;
			case 1: //x pos
				gluvv.clip.on = 1;
				gluvv.clip.oaxis = VolRenAxisXPos;
				glutPostRedisplay();
				break;
			case 2: //x neg
				gluvv.clip.on = 1;
				gluvv.clip.oaxis = VolRenAxisXNeg;
				glutPostRedisplay();
				break;
			case 3: //y pos
				gluvv.clip.on = 1;
				gluvv.clip.oaxis = VolRenAxisYPos;
				glutPostRedisplay();
				break;
			case 4: //y neg
				gluvv.clip.on = 1;
				gluvv.clip.oaxis = VolRenAxisYNeg;
				glutPostRedisplay();
				break;
			case 5: //z pos
				gluvv.clip.on = 1;
				gluvv.clip.oaxis = VolRenAxisZPos;
				glutPostRedisplay();
				break;
			case 6: //z neg
				gluvv.clip.on = 1;
				gluvv.clip.oaxis = VolRenAxisZNeg;
				glutPostRedisplay();
				break;
			}
		
		//---------------------------------------------------- histo
		case _histochk:
			gluvv.tf.histOn = Histo->get_int_val();
			glutPostRedisplay();
			break;

		//---------------------------------------------------- normals
		case _normpostBtn:
			cerr << "Loading post-classification normals" << endl;
      gluvv.volren.usePostCNorm = 1;
      gluvv.volren.loadNorms    = 1;
      glutPostRedisplay();
			break;
		case _normpreBtn:
			cerr << "Loading pre-classification normals" << endl;
			glutPostRedisplay();
			break;

		//--------------------------------------------------- back ground color
		case _bggrp:
			gluvv.env.bgColor = bGround->get_int_val();
			glutPostRedisplay();
			break;
	}
}
