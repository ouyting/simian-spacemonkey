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

#ifndef _VOLRENAUX_DOT_H
#define _VOLRENAUX_DOT_H

#include "gluvv.h"
#include <iostream.h>
#include "VectorMath.h"

//====================================================== PostCNorms
//=================================================================
// Compute the post-classification normals
void VRA_PostCNorms(unsigned char *gradout);

#endif