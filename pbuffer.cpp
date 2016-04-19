#include <stdio.h>
#include "pbuffer.h"
#include "glUE.h"
//#include "glh_nveb.h"
//#include "glh_extensions.h"

#define DEBUGGING 0

PBuffer::PBuffer( int w, int h, unsigned int mode ) : width(w), height(h), mode(mode), myDC(NULL), myGLctx(NULL), buffer(NULL)
{
}

PBuffer::~PBuffer()
{
	if ( buffer )
	{
		wglDeleteContext( myGLctx );
		wglReleasePbufferDCARB( buffer, myDC );
		wglDestroyPbufferARB( buffer );
	}
}

// Check to see if the pbuffer was lost.
// If it was lost, destroy it and then recreate it.
void PBuffer::HandleModeSwitch()
{
	int lost = 0;
	
	wglQueryPbufferARB( buffer, WGL_PBUFFER_LOST_ARB, &lost );
	
	if ( lost )
	{
		this->~PBuffer();
		Initialize();
	}
}

// This function actually does the creation of the p-buffer.
// It can only be called once a window has already been created.
void PBuffer::Initialize(bool share, bool rend2tex)
{
#if DEBUGGING
	FILE *fp = fopen( "debug.txt", "w" );
	if ( !fp )
	{
		fprintf( stderr, "Couldn't open file: debug.txt\n" );
		exit( -1 );
	}
#endif
	
	HDC hdc = wglGetCurrentDC();
	HGLRC hglrc = wglGetCurrentContext();
	
	// Query for a suitable pixel format based on the specified mode.
	int   iattributes[2*MAX_ATTRIBS];
	float fattributes[2*MAX_ATTRIBS];
	int nfattribs = 0;
	int niattribs = 0;
	
	// Attribute arrays must be "0" terminated - for simplicity, first
	// just zero-out the array entire, then fill from left to right.
	for ( int a = 0; a < 2*MAX_ATTRIBS; a++ )
	{
		iattributes[a] = 0;
		fattributes[a] = 0;
	}
	
	// Since we are trying to create a pbuffer, the pixel format we
	// request (and subsequently use) must be "p-buffer capable".
	iattributes[2*niattribs  ] = WGL_DRAW_TO_PBUFFER_ARB;
	iattributes[2*niattribs+1] = true;
	niattribs++;
	
	if ( mode & GLUT_INDEX )
	{
		iattributes[2*niattribs  ] = WGL_PIXEL_TYPE_ARB;
		iattributes[2*niattribs+1] = WGL_TYPE_COLORINDEX_ARB;  // Yikes!
		niattribs++;
	}
	else
	{
		iattributes[2*niattribs  ] = WGL_PIXEL_TYPE_ARB;
		iattributes[2*niattribs+1] = WGL_TYPE_RGBA_ARB;
		niattribs++;
	}
	
	if ( mode & GLUT_DOUBLE )
	{
		iattributes[2*niattribs  ] = WGL_DOUBLE_BUFFER_ARB;
		iattributes[2*niattribs+1] = true;
		niattribs++;
	}
	
	if ( mode & GLUT_DEPTH )
	{
		iattributes[2*niattribs  ] = WGL_DEPTH_BITS_ARB;
		iattributes[2*niattribs+1] = 1;
		niattribs++;
	}
	if ( mode & GLUT_STENCIL )
	{
		iattributes[2*niattribs  ] = WGL_STENCIL_BITS_ARB;
		iattributes[2*niattribs+1] = 1;
		niattribs++;
	}
	if ( mode & GLUT_ACCUM )
	{
		iattributes[2*niattribs  ] = WGL_ACCUM_BITS_ARB;
		iattributes[2*niattribs+1] = 1;
		niattribs++;
	}
	
	iattributes[2*niattribs  ] = WGL_SUPPORT_OPENGL_ARB;
	iattributes[2*niattribs+1] = true;
	niattribs++;

	if(rend2tex){
		iattributes[2*niattribs  ] = WGL_BIND_TO_TEXTURE_RGBA_ARB;
		iattributes[2*niattribs+1] = true;
		niattribs++;
	}

	int format;
	int pformat[MAX_PFORMATS];
	unsigned int nformats;
	if ( !wglChoosePixelFormatARB( hdc, iattributes, fattributes, MAX_PFORMATS, pformat, &nformats ) )
	{
		fprintf( stderr, "pbuffer creation error:  Couldn't find a suitable pixel format.\n" );
		exit( -1 );
	}
	format = pformat[0];
	
	
#if DEBUGGING
	fprintf( fp, "nformats = %d\n\n", nformats );
	int values[MAX_ATTRIBS];
	int iatr[MAX_ATTRIBS] = { WGL_PIXEL_TYPE_ARB, WGL_COLOR_BITS_ARB,
		WGL_RED_BITS_ARB, WGL_GREEN_BITS_ARB, WGL_BLUE_BITS_ARB,
		WGL_ALPHA_BITS_ARB, WGL_DEPTH_BITS_ARB, WGL_STENCIL_BITS_ARB, WGL_ACCUM_BITS_ARB,
		WGL_DOUBLE_BUFFER_ARB, WGL_SUPPORT_OPENGL_ARB, WGL_ACCELERATION_ARB };
	int niatr = 12;
	for ( int j = 0; j < MAX_ATTRIBS; j++ )
	{
		values[j] = false;
		iattributes[j] = iattributes[2*j];
	}
	for ( unsigned int i = 0; i < nformats; i++ )
	{
		if ( !wglGetPixelFormatAttribivARB( hdc, pformat[i], 0, niatr, iatr, values ) )
		{
			fprintf( stderr, "pbuffer creation error:  wglGetPixelFormatAttribiv() failed\n" );
			exit( -1 );
		}
		fprintf( fp, "%d. pformat = %d\n", i, pformat[i] );
		fprintf( fp, "--------------------\n" );
		for ( int k = 0; k < niatr; k++ )
		{
			if ( iatr[k] == WGL_PIXEL_TYPE_ARB )
			{
				if ( values[k] == WGL_TYPE_COLORINDEX_ARB )
					fprintf( fp, " Pixel type = WGL_TYPE_COLORINDEX_ARB\n" );
				if ( values[k] == WGL_TYPE_RGBA_ARB )
					fprintf( fp, " Pixel type = WGL_TYPE_RGBA_ARB\n" );
			}
			if ( iatr[k] == WGL_COLOR_BITS_ARB )
			{
				fprintf( fp, " Color bits = %d\n", values[k] );
			}
			if ( iatr[k] == WGL_RED_BITS_ARB )
			{
				fprintf( fp, "      red         %d\n", values[k] );
			}
			if ( iatr[k] == WGL_GREEN_BITS_ARB )
			{
				fprintf( fp, "      green       %d\n", values[k] );
			}
			if ( iatr[k] == WGL_BLUE_BITS_ARB )
			{
				fprintf( fp, "      blue        %d\n", values[k] );
			}
			if ( iatr[k] == WGL_ALPHA_BITS_ARB )
			{
				fprintf( fp, "      alpha       %d\n", values[k] );
			}
			if ( iatr[k] == WGL_DEPTH_BITS_ARB )
			{
				fprintf( fp, " Depth bits =   %d\n", values[k] );
			}
			if ( iatr[k] == WGL_STENCIL_BITS_ARB )
			{
				fprintf( fp, " Stencil bits = %d\n", values[k] );
			}
			if ( iatr[k] == WGL_ACCUM_BITS_ARB )
			{
				fprintf( fp, " Accum bits =   %d\n", values[k] );
			}
			if ( iatr[k] == WGL_DOUBLE_BUFFER_ARB )
			{
				fprintf( fp, " Double Buffer  = %d\n", values[k] );
			}
			if ( iatr[k] == WGL_SUPPORT_OPENGL_ARB )
			{
				fprintf( fp, " Support OpenGL = %d\n", values[k] );
			}
			if ( iatr[k] == WGL_ACCELERATION_ARB )
			{
				if ( values[k] == WGL_FULL_ACCELERATION_ARB )
					fprintf( fp, " Acceleration   = WGL_FULL_ACCELERATION_ARB\n" );
				if ( values[k] == WGL_GENERIC_ACCELERATION_ARB )
					fprintf( fp, " Acceleration   = WGL_GENERIC_ACCELERATION_ARB\n" );
			}
		}
		fprintf( fp, "\n" );
	}
	fprintf( fp, "selected pformat = %d\n", format );
#endif
	
	// Create the p-buffer.
	iattributes[0] = 0;
	if(rend2tex){
		iattributes[0] = WGL_TEXTURE_FORMAT_ARB;
		iattributes[1] = WGL_TEXTURE_RGBA_ARB;
		iattributes[2] = WGL_TEXTURE_TARGET_ARB;
		iattributes[3] = WGL_TEXTURE_2D_ARB;
		iattributes[4] = WGL_BIND_TO_TEXTURE_RGBA_ARB;
		iattributes[5] = TRUE;
		iattributes[6] = 0;    //the last one in an attribute array must be zero!!!
		fprintf(stderr, "Using Render To Texture P-Buffer\n");
	}

	buffer = wglCreatePbufferARB( hdc, format, width, height, iattributes );
	if ( !buffer )
	{
		DWORD err = GetLastError();
		fprintf( stderr, "pbuffer creation error:  wglCreatePbufferARB() failed\n" );
		if ( err == ERROR_INVALID_PIXEL_FORMAT )
		{
			fprintf( stderr, "error:  ERROR_INVALID_PIXEL_FORMAT\n" );
		}
		else if ( err == ERROR_NO_SYSTEM_RESOURCES )
		{
			fprintf( stderr, "error:  ERROR_NO_SYSTEM_RESOURCES\n" );
		}
		else if ( err == ERROR_INVALID_DATA )
		{
			fprintf( stderr, "error:  ERROR_INVALID_DATA\n" );
		}
		exit( -1 );
	}
	
	// Get the device context.
	myDC = wglGetPbufferDCARB( buffer );
	if ( !myDC )
	{
		fprintf( stderr, "pbuffer creation error:  wglGetPbufferDCARB() failed\n" );
		exit( -1 );
	}
	
	// Create a gl context for the p-buffer.
	myGLctx = wglCreateContext( myDC );
	if ( !myGLctx )
	{
		fprintf( stderr, "pbuffer creation error:  wglCreateContext() failed\n" );
		exit( -1 );
	}
	
	if( share )
		{
		if( !wglShareLists(hglrc, myGLctx) )
		{
			fprintf( stderr, "pbuffer: wglShareLists() failed\n" );
			exit( -1 );
		}
		}
	
	// Determine the actual width and height we were able to create.
	wglQueryPbufferARB( buffer, WGL_PBUFFER_WIDTH_ARB, &width );
	wglQueryPbufferARB( buffer, WGL_PBUFFER_HEIGHT_ARB, &height );
	
	fprintf( stderr, "Created a %d x %d pbuffer\n", width, height );
	
#if DEBUGGING
	fclose( fp );
#endif
}

void PBuffer::MakeCurrent()
{
	GlErr("NOT!Pbuffer","makecurrent");

	if ( !wglMakeCurrent( myDC, myGLctx ) )
	{
		fprintf( stderr, "PBuffer::MakeCurrent() failed.\n" );
		//exit( -1 );
	}
	GlErr("Pbuffer","makecurrent");
}


