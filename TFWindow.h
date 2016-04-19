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
// TFWindow.h: interface for the TFWindow class.
//
//  NOTE: Right now you can only create one of these!
//       should use current window ID to pick the right window
//         -next rev
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TFWINDOW_H__211F0AFE_B4E5_4115_80EF_3F0308B27C74__INCLUDED_)
#define AFX_TFWINDOW_H__211F0AFE_B4E5_4115_80EF_3F0308B27C74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HSLPicker.h"

//---------------- a helper class -------------------------------------------
class TFPoint
{
public:
   TFPoint();
   TFPoint(int bin, float u, float v=0, float w=0);
   ~TFPoint();

   inline float *getPos(){return pos;}
   inline void   setPos(float p[3]){pos[0]=p[0]; pos[1]=p[1]; pos[2]=p[2];}
   inline void   setPosU(float u){pos[0] = u;}
   inline void   setPosV(float v){pos[1] = v;}
   inline void   setPosW(float w){pos[2] = w;}
	 inline void   setV(float a, float b, float c){v[0]=a;v[1]=b;v[2]=c;}
   inline int    getType(){return type;}
   inline int    getBin(){return bin;}
   inline void   setBin(int b){bin = b;}

   TFPoint *insert(TFPoint *p); //puts point in list
   TFPoint *getPoint(int bin);  //removes point from list
   TFPoint *findPoint(int bin); //returns a pointer to a point

   float operator[](int i){ return pos[i]; }

   float v[3];   //the user defined vector
   float pos[3]; //the values [0-1]

	 HSLPicker   cpick; //color picker

private:

   TFPoint *next, *prev;
   int bin;
   int type;
};

//--------------- the transfer function window --------------------------------
class TFWindow  
{
public:
	TFWindow();
	virtual ~TFWindow();

	void    create();
	void    destroy();

	void    display();
	void    mouse(int button, int state, int x, int y);
	void    motion(int x, int y);
	void    key(unsigned char key, int x, int y);
	void    special(int key, int x, int y);
	void    idle();
	void    reshape(int w, int h);

	int     pick();

	int  width;
	int  height;

private:
	void updateColor();	
	void updateVolume();

	void init();

	void draw1Dtf();
	void draw2Dtf();

	float bots[3];
	float tops[3];

	float pntsHeight;
	float exts[2][2];

	unsigned int deptexName;

	int  data1;
	int  data2;
	int  data3;
	int  name32;
	long name64;
	int  zpick;
	int  picking;
	
	int  pickPos[2];
	int  butt;

	TFPoint points;

};



#endif // !defined(AFX_TFWINDOW_H__211F0AFE_B4E5_4115_80EF_3F0308B27C74__INCLUDED_)
