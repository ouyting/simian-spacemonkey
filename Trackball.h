//
// ============================================================
//               === ACL Visualization Library === 
//
// $Date: 2001/06/27 19:27:44 $
// $Revision: 1.2 $
//
// Hacked into C++ from SGI's trackball.h---see copyright
// at end.
// 
// ============================================================
//

#ifndef Trackball_H
#define Trackball_H

class  Trackball {
  
 public:
  Trackball();
  ~Trackball();

  void start(float x, float y);

  // Pass the x and y coordinates of the last and current positions of
  // the mouse, scaled so they are from (-1.0 ... 1.0).
  //
  // The resulting rotation is returned as a quaternion rotation in the
  // first paramater.
  void update(float x, float y);

  // Given two quaternions, add them together to get a third quaternion.
  // Adding quaternions to get a compound rotation is analagous to adding
  // translations to get a compound translation.  When incrementally
  // adding rotations, the first argument here should be the new
  // rotation, the second and third the total rotation (which will be
  // over-written with the resulting new total rotation).
  void addQuats(float q1[4], float q2[4], float dest[4]);

  // A useful function, builds a rotation matrix in Matrix based on
  // given quaternion.
  void buildRotMatrix(float m[4][4]);

  // This function computes a quaternion based on an axis (defined by
  // the given vector) and an angle about which to rotate.  The angle is
  // expressed in radians.  The result is put into the third argument.
  void axisToQuat(float a[3], float phi, float q[4]);
  void reapply();

  // This function was added by joe kniss to facilitate a home key!
  void clear();
  // This funciton was also written by joe to allow arbitary views
  void setMatrix(float rot[16]);

 private:
  static float        trackballSize;
  static int          renormCount;

  float               curQuat[4];
  float               lastQuat[4];
  float               lastX;
  float               lastY;

  float projectToSphere(float, float, float);
  void  normalizeQuat(float [4]);

  //  Vector math.  Have to move somewhere else eventually.
  void   vZero(float *v);
  void   vSet(float *v, float x, float y, float z);
  void   vSub(const float *src1, const float *src2,
              float *dst);
  void   vCopy(const float *v1, float *v2);
  void   vCross(const float *v1, const float *v2,
                float *cross);
  float  vLength(const float *v);
  void   vScale(float *v, float div);
  void   vNormal(float *v);
  float  vDot(const float *v1, const float *v2);
  void   vAdd(const float *src1, const float *src2,
              float *dst);
};

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

#endif

