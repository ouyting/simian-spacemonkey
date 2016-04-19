//
// ============================================================
//                     === OpenGL Library === 
//
// $Date: 2001/06/27 19:27:43 $
// $Revision: 1.2 $
//
// Hacked into C++ from SGI's trackball.h---see copyright
// at end.
// 
// ============================================================
//

#include <iostream.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <math.h>

#include "Trackball.h"



//===========================================================================
//===========================================================================
// Static Initialization
//===========================================================================
//===========================================================================

//  This size should really be based on the distance from the center of
//  rotation to the point on the object underneath the mouse.  That
//  point would then track the mouse as closely as possible.  This is a
//  simple example, though, so that is left as an Exercise for the
//  Programmer.
float  Trackball::trackballSize = (float)0.9;

//  Number of iterations before we renormalize.
int  Trackball::renormCount = 97;


//===========================================================================
//===========================================================================
// Constructors/Destructor
//===========================================================================
//===========================================================================

Trackball::Trackball()
{
  lastX = 0.0;
  lastY = 0.0;
  vZero(lastQuat);
  vZero(curQuat);
  lastQuat[3] = 1.0;
  curQuat[3] = 1.0;
}


Trackball::~Trackball()
{
  //  Empty.
}



//===========================================================================
// Public Member Functions
//===========================================================================

void
Trackball::start(float x, float y)
{
  lastX = x;
  lastY = y;
  vZero(lastQuat);
  lastQuat[3] = 1.0;
}



/////////////////////////////////////////////////////////////////////////////
//
//  Ok, simulate a track-ball.  Project the points onto the virtual
//  trackball, then figure out the axis of rotation, which is the cross
//  product of P1 P2 and O P1 (O is the center of the ball, 0,0,0)
//  Note:  This is a deformed trackball-- is a trackball in the center,
//  but is deformed into a hyperbolic sheet of rotation away from the
//  center.  This particular function was chosen after trying out
//  several variations.
//
//  It is assumed that the arguments to this routine are in the range
//  (-1.0 ... 1.0)
//
void
Trackball::update(float x, float y)
{
  float a[3]; /* Axis of rotation */
  float phi;  /* how much to rotate about axis */
  float p1[3], p2[3], d[3];
  float t;

  //  Might just be able to return here.
  if (lastX == x && lastY == y) {
    /* Zero rotation */
    vZero(lastQuat);
    lastQuat[3] = 1.0;
    return;
  }

  /*
   * First, figure out z-coordinates for projection of P1 and P2 to
   * deformed sphere
   */
  vSet(p1, lastX, lastY, projectToSphere(trackballSize, lastX, lastY));
  vSet(p2, x,     y,     projectToSphere(trackballSize,  x,    y));

  /*
   *  Now, we want the cross product of P1 and P2
   */
  vCross(p2, p1, a);

  /*
   *  Figure out how much to rotate around that axis.
   */
  vSub(p1,p2,d);
  t = (float)(vLength(d) / (2.0*trackballSize));

  /*
   * Avoid problems with out-of-control values...
   */
  if (t > 1.0) t = 1.0;
  if (t < -1.0) t = -1.0;
  phi = (float)(2.0 * asin(t));

  axisToQuat(a, phi, lastQuat);

  lastX = x;
  lastY = y;

  addQuats(lastQuat, curQuat, curQuat);
}

/////////////////////////////////////////////////////////////////////////////
// joeys clear
//
void 
Trackball::clear()
{
  lastX = 0.0;
  lastY = 0.0;
  vZero(lastQuat);
  vZero(curQuat);
  lastQuat[3] = 1.0;
  curQuat[3] = 1.0;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Given an axis and angle, compute quaternion.
//
void
Trackball::axisToQuat(float a[3], float phi, float q[4])
{
  vNormal(a);
  vCopy(a,q);
  vScale(q,(float)(sin(phi/2.0)));
  q[3] = (float)(cos(phi/2.0));
}



/////////////////////////////////////////////////////////////////////////////
//
//  Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
//  if we are away from the center of the sphere.
//
float
Trackball::projectToSphere(float r, float x, float y)
{
  float d, t, z;

  d = (float)(sqrt(x*x + y*y));
  if (d < r * 0.70710678118654752440) {    /* Inside sphere */
    z = (float)(sqrt(r*r - d*d));
  } else {           /* On hyperbola */
    t = (float)(r / 1.41421356237309504880);
    z = t*t / d;
  }
  return z;
}



/////////////////////////////////////////////////////////////////////////////
//
//  Given two rotations, e1 and e2, expressed as quaternion rotations,
//  figure out the equivalent single rotation and stuff it into dest.
//
//  This routine also normalizes the result every RENORMCOUNT times it is
//  called, to keep error from creeping in.
//
//  NOTE: This routine is written so that q1 or q2 may be the same
//  as dest (or each other).
//
void
Trackball::addQuats(float q1[4], float q2[4], float dest[4])
{
  static int count=0;
  float t1[4], t2[4], t3[4];
  float tf[4];

  vCopy(q1,t1);
  vScale(t1,q2[3]);

  vCopy(q2,t2);
  vScale(t2,q1[3]);

  vCross(q2,q1,t3);
  vAdd(t1,t2,tf);
  vAdd(t3,tf,tf);
  tf[3] = q1[3] * q2[3] - vDot(q1,q2);

  dest[0] = tf[0];
  dest[1] = tf[1];
  dest[2] = tf[2];
  dest[3] = tf[3];

  if (++count > renormCount) {
    count = 0;
    normalizeQuat(dest);
  }
}



/////////////////////////////////////////////////////////////////////////////
//
//  Quaternions always obey:  a^2 + b^2 + c^2 + d^2 = 1.0
//  If they don't add up to 1.0, dividing by their magnitued will
//  renormalize them.
//
//  Note: See the following for more information on quaternions:
//
//  - Shoemake, K., Animating rotation with quaternion curves, Computer
//    Graphics 19, No 3 (Proc. SIGGRAPH'85), 245-254, 1985.
//  - Pletinckx, D., Quaternion calculus as a basic tool in computer
//    graphics, The Visual Computer 5, 2-13, 1989.
//
void
Trackball::normalizeQuat(float q[4])
{
    float  mag = (q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
    for (int i = 0; i < 4; i++) q[i] /= mag;
}



/////////////////////////////////////////////////////////////////////////////
//
//  Build a rotation matrix, given a quaternion rotation.
//
void
Trackball::buildRotMatrix(float m[4][4])
{
  m[0][0] = (float)(1.0 - 2.0 * (curQuat[1] * curQuat[1] + curQuat[2] * curQuat[2]));
  m[0][1] = (float)(2.0 * (curQuat[0] * curQuat[1] - curQuat[2] * curQuat[3]));
  m[0][2] = (float)(2.0 * (curQuat[2] * curQuat[0] + curQuat[1] * curQuat[3]));
  m[0][3] = (float)(0.0);

  m[1][0] = (float)(2.0 * (curQuat[0] * curQuat[1] + curQuat[2] * curQuat[3]));
  m[1][1]= (float)(1.0 - 2.0 * (curQuat[2] * curQuat[2] + curQuat[0] * curQuat[0]));
  m[1][2] = (float)(2.0 * (curQuat[1] * curQuat[2] - curQuat[0] * curQuat[3]));
  m[1][3] = (float)(0.0);

  m[2][0] = (float)(2.0 * (curQuat[2] * curQuat[0] - curQuat[1] * curQuat[3]));
  m[2][1] = (float)(2.0 * (curQuat[1] * curQuat[2] + curQuat[0] * curQuat[3]));
  m[2][2] = (float)(1.0 - 2.0 * (curQuat[1] * curQuat[1] + curQuat[0] * curQuat[0]));
  m[2][3] = (float)(0.0);

  m[3][0] = (float)(0.0);
  m[3][1] = (float)(0.0);
  m[3][2] = (float)(0.0);
  m[3][3] = (float)(1.0);
}



void
Trackball::reapply()
{
  addQuats(lastQuat, curQuat, curQuat);
}



//============================================================================
// Protected Member Functions
//============================================================================

//-===========================================================================
// Private Member Functions
//============================================================================

void
Trackball::vZero(float *v)
{
  v[0] = 0.0;
  v[1] = 0.0;
  v[2] = 0.0;
}



void
Trackball::vSet(float *v, float x, float y, float z)
{
  v[0] = x;
  v[1] = y;
  v[2] = z;
}



void
Trackball::vSub(const float *src1, const float *src2, float *dst)
{
  dst[0] = src1[0] - src2[0];
  dst[1] = src1[1] - src2[1];
  dst[2] = src1[2] - src2[2];
}



void
Trackball::vCopy(const float *v1, float *v2)
{
  for (int i = 0 ; i < 3 ; i++)
    v2[i] = v1[i];
}



void
Trackball::vCross(const float *v1, const float *v2, float *cross)
{
  float temp[3];

  temp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
  temp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
  temp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
  vCopy(temp, cross);
}



float
Trackball::vLength(const float *v)
{
  return (float)(sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));
}

void
Trackball::vScale(float *v, float div)
{
  v[0] *= div;
  v[1] *= div;
  v[2] *= div;
}



void
Trackball::vNormal(float *v)
{
  vScale(v,(float)(1.0/vLength(v)));
}



float
Trackball::vDot(const float *v1, const float *v2)
{
  return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}



void
Trackball::vAdd(const float *src1, const float *src2, float *dst)
{
  dst[0] = src1[0] + src2[0];
  dst[1] = src1[1] + src2[1];
  dst[2] = src1[2] + src2[2];
}



/*
 * (c) Copyright 1993, 1994, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * US Government Users Restricted Rights
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(TM) is a trademark of Silicon Graphics, Inc.
 */
