// testPBuff.h: interface for the testPBuff class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTPBUFF_H__B586CD63_C248_44B0_8523_FCBED4E920EA__INCLUDED_)
#define AFX_TESTPBUFF_H__B586CD63_C248_44B0_8523_FCBED4E920EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gluvvPrimitive.h"
#include "PBuffer.h"

class testPBuff : public gluvvPrimitive  
{
public:
	testPBuff();
	virtual ~testPBuff();
	
	void init();
	void draw();

  void draw1();
  void draw2();
  void draw3();

private:
	PBuffer *pbuff;            //pbuffer for rendering light's point of view
	PBuffer *pbuff2;            //pbuffer for rendering light's point of view
	void MakeCurrent();        //switch back to this context

	unsigned int shadowName;

  unsigned int fsname;
  unsigned int fsname1;
  unsigned int fsname2;
  unsigned int fsname3;
  unsigned int fsname4;

};

#endif // !defined(AFX_TESTPBUFF_H__B586CD63_C248_44B0_8523_FCBED4E920EA__INCLUDED_)
