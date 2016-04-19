#ifndef PBUFFERS_H
#define PBUFFERS_H

#include <windows.h>
#include "GL/gl.h"
#include <GL/glut.h>
#include "GL/wglext.h"
#include <GL/glext.h>

#define MAX_PFORMATS 256
#define MAX_ATTRIBS  32

class PBuffer
{
private:
    HDC          myDC;      // Handle to a device context.
    HGLRC        myGLctx;   // Handle to a GL context.
    unsigned int mode;      // Flags indicating the type of pbuffer.
public:
    int          width;
    int          height;
    PBuffer( int width, int height, unsigned int mode );
    ~PBuffer();
    void HandleModeSwitch();
    void MakeCurrent();
    void Initialize(bool share = false);
    HPBUFFERARB  buffer;    // Handle to a pbuffer.

};

#endif