//------------------------------------------------------------------------
//C++ 
//   Joe Kniss
//  gluvvPrimitive.h  virual class for rendering and widgets
//      --a cheap ass, two bit, scene graph
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
//               "All Your Base are Belong to Us"
//-------------------------------------------------------------------------

#ifndef __GLUVVPRIMITIVE_DOT_H
#define __GLUVVPRIMITIVE_DOT_H


class gluvvPrimitive {
 public:
  gluvvPrimitive();
  ~gluvvPrimitive();


	virtual  void          init();
  virtual  void          draw();
                    //return 1 if the key press was for you
  virtual  int           key(unsigned char k, int x, int y);
	virtual  int           special(int k, int x, int y);
                    //primitives return 0 to be released
  virtual  int           pick(int data1, int data2, int data3,
			      float x, float y, float z);
  virtual  int           pick();
  virtual  int           mouse(int button, int state, 
			       int x, int y);
  virtual  int           move(int x, int y);
  virtual  int           release();   //tells widget it is done
  
  void              setNext(gluvvPrimitive *p);  //set next in list
  gluvvPrimitive   *getNext();                   //get next in list
  void              remove(gluvvPrimitive *p);   //remove from list
  
  void              setName(const char *PName);  //just a name
  
  void              gluvvPushName();       //push name for picking 
  void              gluvvPopNames();       //pop all of the names

 private:
  gluvvPrimitive *next;
  char           *name;
};

#endif
