// TestRen.h: interface for the TestRen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTREN_H__277AC19F_1072_4EB2_AB7B_0BC2255A7FDD__INCLUDED_)
#define AFX_TESTREN_H__277AC19F_1072_4EB2_AB7B_0BC2255A7FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gluvvPrimitive.h"

class TestRen : public gluvvPrimitive  
{
public:
	TestRen();
	virtual ~TestRen();

	void draw();
	void init();

	unsigned char *tex1;
	unsigned char *tex2;
	unsigned int  t1name;
	unsigned int  t2name;

	void fragShade1();
	unsigned int fs1name; //fragment shader 1 name
};

#endif // !defined(AFX_TESTREN_H__277AC19F_1072_4EB2_AB7B_0BC2255A7FDD__INCLUDED_)
