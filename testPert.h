// testPert.h: interface for the testPert class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTPERT_H__1EB08036_B36B_4BF1_8333_B416DCF27827__INCLUDED_)
#define AFX_TESTPERT_H__1EB08036_B36B_4BF1_8333_B416DCF27827__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gluvvPrimitive.h"
#include "PBuffer.h"

class testPert : public gluvvPrimitive  
{
public:
	testPert();
	virtual ~testPert();

  void init();
  void draw();

  PBuffer *pbuff;
  void MakeCurrent();        //switch back to this context

  unsigned int texName;
  unsigned int ptName;
  unsigned int ptTexName; 

  unsigned int fsname;  
  unsigned int fsname2;  

};

#endif // !defined(AFX_TESTPERT_H__1EB08036_B36B_4BF1_8333_B416DCF27827__INCLUDED_)
