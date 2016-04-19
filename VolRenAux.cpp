//------------------------------------------------------------------------
//
//   Joe Kniss
//    VolRenAux Auxilary Volume Rendering Tools
//   VolRenAux.h
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
//                   <<< jmk@cs.utah.edu >>>
//               "All Your Base are Belong to Us"
//-------------------------------------------------------------------------

#include "VolRenAux.h"

//====================================================== PostCNorms
//=================================================================
// Compute the post-classification normals
void VRA_PostCNorms(unsigned char *gradout)
{
	if((gluvv.tf.ptexsz[0] != 256)||(gluvv.tf.ptexsz[1] != 256)||(gluvv.tf.ptexsz[2] != 1))
	{
		cerr << "Error: VolRenAux::VRA_PostCNorms()-" << endl;
		cerr << "   This gradient measure is not implemented yet" << endl;
	}

	int sx = gluvv.mv->xiSize;
	int sy = gluvv.mv->yiSize;
	int sz = gluvv.mv->ziSize;

	unsigned char *data;

	if(gluvv.mv->numSubVols != 1)
	{
		if(!gluvv.mv->wholeVol){
			cerr << "Error: VolRenAux::VRA_PostCNorms()-" << endl;
			cerr << "  This gradient measure is not implemented for bricked volumes" << endl;	
		} else {
			data = gluvv.mv->wholeVol->currentData;
		}
	} 
	else
	{
		data = gluvv.mv->volumes[0].currentData;
	}

  unsigned char *vol = new unsigned char[sx*sy*sz];

  int e = gluvv.mv->nelts;

  int i;

	switch(gluvv.dmode)
	{
		case GDM_V1: //one byte texture
		case GDM_VGH_V:
      for(i=0; i<sx*sy*sz; ++i)
      {
        vol[i] = gluvv.volren.deptex[data[i*e]*4 + 3];
      }
      break;
		case GDM_V1G: //two byte texture
		case GDM_V2:
		case GDM_VGH_VG:
			break;
		case GDM_V1GH: //3 or 4 byte data == 4 byte texture
		case GDM_V2G:
		case GDM_V2GH:
		case GDM_V3:
		case GDM_V3G:
		case GDM_V4:
		case GDM_VGH:
			break;
		default:	

      break;
	}

  float *gv = new float[sx*sy*sz*3];
  unsigned char *m = new unsigned char[sx*sy*sz];

  AGradArb<unsigned char>(gv, sx, sy, sz, 1, 1, data);
  //derivative3D(m, gv, sx, sy, sz, vol);
  //cerr << "Blurring normals" << endl;
  //blurV3D(gv, (float)1.0, (float).3, (float).25, (float).2, sx, sy, sz);
  scalebias(gradout, gv, sx, sy, sz);

  delete[] gv;
  delete[] m;
  delete[] vol;



}
