//------------------------------------------------------------------------
//  -genVGH main.cpp
//    -generates VGH (Value Grad Hessian) volumes from various data types
//
//
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

#include <iostream.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "../MetaVolume.h"

MetaVolume *mv;
char *outName;


int readData();
int writeData();

#if 0
template<class T> 
void makeVGH(Volume *v);
#endif

int parse(int argc, char **argv);

#ifndef MAX
#define MAX(x,y) (((x)>(y)) ? (x) : (y))
#endif

#ifndef MIN
#define MIN(x,y) (((x)<(y)) ? (x) : (y))
#endif

inline double affine(const double i, const double x, const double I, 
		     const double o, const double O)
{
   return  ( ((O)-(o))*((x)-(i)) / ((I)-(i)) + (o) );
}


//============================================================== make VGH
//=======================================================================
template<class T> 
void makeVGH(Volume *v)
{
	cerr << "making VGH volume" << endl;

	T *d = (T*)v->nativeData;
	int sx = v->xiSize;
	int sy = v->yiSize;
	int sz = v->ziSize;

	float *gradV3 = new float[sx*sy*sz*3];
	float *gmag = new float[sx*sy*sz];
	float gmmax = -100000000;
	float gmmin = 100000000;
	float dmax = -100000000;
	float dmin = 100000000;


	//compute the gradient
	cerr << "   computing 1st derivative";
	for(int i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				if((k<1)||(k>sx-2)||(j<1)||(j>sy-2)||(i<1)||(i>sz-2)){
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = 0;
					gmag[i*sx*sy + j*sx + k] = 0;
				}
				else {
					float dx = (float)(d[i*sx*sy     + j*sx     +(k+1)] - d[i*sx*sy     + j*sx     + (k-1)]);
					float dy = (float)(d[i*sx*sy     + (j+1)*sx + k]    - d[i*sx*sy     + (j-1)*sx + k]);
					float dz = (float)(d[(i+1)*sx*sy + j*sx     + k]    - d[(i-1)*sx*sy + j*sx     + k]);
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = dx;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = dy;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = dz;
					gmag[i*sx*sy + j*sx + k] = (float)sqrt(dx*dx+dy*dy+dz*dz);
					gmmax = MAX(gmag[i*sx*sy + j*sx + k], gmmax);
					gmmin = MIN(gmag[i*sx*sy + j*sx + k], gmmin);
					dmax = MAX(d[i*sx*sy + j*sx +k], dmax);
					dmin = MIN(d[i*sx*sy + j*sx +k], dmin);
				}
			}
		}
	}
	cerr << " - done" << endl;

	float *hess = new float[sx*sy*sz];
	float hmax = -100000000;
	float hmin = 100000000;

	//compute the 2nd derivative in the gradient direction
	//  a mask would probably help out here??
	cerr << "   computing 2nd derivative in the gradient direction";
	for(i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				if((k<1)||(k>sx-2)||(j<1)||(j>sy-2)||(i<1)||(i>sz-2)){
					hess[i*sx*sy + j*sx + k] = 0;
				}
				else {
					float h[9];
					h[0] = gradV3[i*sx*sy*3 + j*sx*3 + (k+1)*3 + 0] - gradV3[i*sx*sy*3 + j*sx*3 + (k-1)*3 + 0];
					h[1] = gradV3[i*sx*sy*3 + (j+1)*sx*3 + k*3 + 0] - gradV3[i*sx*sy*3 + (j-1)*sx*3 + k*3 + 0];
					h[2] = gradV3[(i+1)*sx*sy*3 + j*sx*3 + k*3 + 0] - gradV3[(i-1)*sx*sy*3 + j*sx*3 + k*3 + 0];
					h[3] = gradV3[i*sx*sy*3 + j*sx*3 + (k+1)*3 + 1] - gradV3[i*sx*sy*3 + j*sx*3 + (k-1)*3 + 1];
					h[4] = gradV3[i*sx*sy*3 + (j+1)*sx*3 + k*3 + 1] - gradV3[i*sx*sy*3 + (j-1)*sx*3 + k*3 + 1];
					h[5] = gradV3[(i+1)*sx*sy*3 + j*sx*3 + k*3 + 1] - gradV3[(i-1)*sx*sy*3 + j*sx*3 + k*3 + 1];
					h[6] = gradV3[i*sx*sy*3 + j*sx*3 + (k+1)*3 + 2] - gradV3[i*sx*sy*3 + j*sx*3 + (k-1)*3 + 2];
					h[7] = gradV3[i*sx*sy*3 + (j+1)*sx*3 + k*3 + 2] - gradV3[i*sx*sy*3 + (j-1)*sx*3 + k*3 + 2];
					h[8] = gradV3[(i+1)*sx*sy*3 + j*sx*3 + k*3 + 2] - gradV3[(i-1)*sx*sy*3 + j*sx*3 + k*3 + 2];
					float tv[3];
					float tg[3] = {gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0]/gmag[i*sx*sy + j*sx + k], 
						             gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1]/gmag[i*sx*sy + j*sx + k],
												 gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2]/gmag[i*sx*sy + j*sx + k]};

					tv[0] = tg[0] * h[0] + 
						      tg[1] * h[1] +
									tg[2] * h[2];
					tv[1] = tg[0] * h[3] + 
						      tg[1] +
									tg[2] * h[5];
					tv[2] = tg[0] * h[6] + 
						      tg[1] * h[7] +
									tg[2] * h[8];
					hess[i*sx*sy + j*sx + k] = tg[0] * tv[0] + 
						                         tg[1] * tv[1] +
									                   tg[2] * tv[2];
					hmax = MAX(hess[i*sx*sy + j*sx + k], hmax);
					hmin = MIN(hess[i*sx*sy + j*sx + k], hmin);

				}
			}
		}
	}
	cerr << " -done" << endl;

	cerr << "   quantizing";
	v->currentData = new unsigned char[sx*sy*sz*3];
	unsigned char *o = v->currentData;
	for(i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				if((k<1)||(k>sx-2)||(j<1)||(j>sy-2)||(i<1)||(i>sz-2)){
					o[i*sx*sy*3 + j*sx*3 + k*3 + 0] = 0;
					o[i*sx*sy*3 + j*sx*3 + k*3 + 1] = 0;
					o[i*sx*sy*3 + j*sx*3 + k*3 + 2] = 0;
				}
				else {
					o[i*sx*sy*3 + j*sx*3 + k*3 + 0] = (unsigned char)affine(dmin, d[i*sx*sy + j*sx + k], dmax, 0, 255);
					o[i*sx*sy*3 + j*sx*3 + k*3 + 1] = (unsigned char)affine(gmmin, gmag[i*sx*sy + j*sx + k], gmmax, 0, 255);
					if(hess[i*sx*sy + j*sx + k] < 0){
						float th = (float)affine(hmin, hess[i*sx*sy + j*sx + k], 0, 0, 1);
						//th = (float)(1.0-((1.0-th)*(1.0-th)*(1.0-th)));
						o[i*sx*sy*3 + j*sx*3 + k*3 + 2] = (unsigned char)affine(0,th,1, 0, 255/3); 
					} else {
						float th = (float)affine(0, hess[i*sx*sy + j*sx + k], hmax, 0, 1);
						//th = (float)(1.0-((1.0-th)*(1.0-th)*(1.0-th)));
						o[i*sx*sy*3 + j*sx*3 + k*3 + 2] = (unsigned char)affine(0,th,1,255/3,255/3*2);
					}
				}
			}
		}
	}
	cerr << " - done" << endl;
	cerr << "done making VGH volume" << endl;
}


//=========================================================================
//================================================================  MAIN
//=========================================================================
int main(int argc, char **argv){

	outName = 0;
	mv = 0;
	if(parse(argc, argv)) return 1;

	if(mv->numSubVols > 1){
		cerr << "ERROR support for VGH from bricked volumes not supported" << endl;
		return 1;
	}

	if(!readData()){
		cerr << "Error reading file" << endl;
		exit(1);
	}

	cerr << " data read succesful" << endl;

	switch(mv->dataType){
	case MetaVolume::_INT:
		makeVGH<int>(&mv->volumes[0]);
		break;
	case MetaVolume::_UINT:
		makeVGH<unsigned int>(&mv->volumes[0]);
		break;
	case MetaVolume::_FLOAT:
		makeVGH<float>(&mv->volumes[0]);
		break;
	case MetaVolume::_SHORT:
		makeVGH<short>(&mv->volumes[0]);
		break;
	case MetaVolume::_USHORT:
		makeVGH<unsigned short>(&mv->volumes[0]);
		break;
	case MetaVolume::_UCHAR:
		cerr << " generating VGH from unsigned char" << endl;
		makeVGH<unsigned char>(&mv->volumes[0]);
		break;
	}

	if(outName == 0){
		outName = new char[512];
		sprintf(outName, "%s-vgh.nrrd", mv->path);
	}
	writeData();


	return 0;
}

//============================================================= Read data
//=======================================================================

//----------------------------------------------------
int read2B(char *subfile, Volume *volume){
	FILE *f;
	if(!(f = fopen(subfile, "rb"))){
	   cerr << "Error: Reader::readVolume, failed to open " 
		<< subfile << " for reading" << endl;
	   return 0;
	}
	
	cerr << "** Reading " << subfile << " of size " << volume->xiSize << " x " 
		<< volume->yiSize <<  " x " << volume->ziSize << " = " 
	     << volume->xiSize * volume->yiSize * volume->ziSize 
			 << " x 2 bytes " << endl;
	size_t n;
	//read it
	n = fread((void *)volume->nativeData, 
		        sizeof(unsigned short), 
		        volume->xiSize * volume->yiSize * volume->ziSize, f);
	   //sanity check
	if(n != (unsigned int)(volume->xiSize * volume->yiSize * volume->ziSize)){
	      cerr << "Error: Reader::readVolume, UC read failed\n";
	      cerr << "  n = " << n << endl;
	      return 0; //error
	}
	//close it to force a flush
	fclose(f);
	cerr << "read : " << n*2 << " bytes " << endl;
	return n;
}

//----------------------------------------------------
int read4B(char *subfile, Volume *volume){

	return 0;
}

//----------------------------------------------------
int read1B(char *subfile, Volume *volume){
	FILE *f;
	if(!(f = fopen(subfile, "rb"))){
	   cerr << "Error: Reader::readVolume, failed to open " 
		<< subfile << " for reading" << endl;
	   return 0;
	}
	
	cerr << "** Reading " << subfile << " of size " << volume->xiSize << " x " 
		<< volume->yiSize <<  " x " << volume->ziSize << " = " 
	     << volume->xiSize * volume->yiSize * volume->ziSize 
			 << " x 1 byte " << endl;
	size_t n;
	//read it
	n = fread((void *)volume->nativeData, 
		        sizeof(unsigned char), 
		        volume->xiSize * volume->yiSize * volume->ziSize, f);
	   //sanity check
	if(n != (unsigned int)(volume->xiSize * volume->yiSize * volume->ziSize)){
	      cerr << "Error: Reader::readVolume, UC read failed\n";
	      cerr << "  n = " << n << endl;
	      return 0; //error
	}
	//close it to force a flush
	fclose(f);
	cerr << "read : " << n << " bytes " << endl;
	return n;
}

//======================================== change the endian of some data

int flip2B(void *data, int sx, int sy, int sz)
{
	unsigned short *d = (unsigned short*)data;
	for(int i=0; i<sz; ++i){
		for(int j=0; j<sy; ++j){
			for(int k=0; k<sx; ++k){
				unsigned short tmp0 = (d[i*sx*sy*2 + j*sx*2 + k]>>8)*0x00FF;
				unsigned short tmp1 = (d[i*sx*sy*2 + j*sx*2 + k])*0x00FF;
				d[i*sx*sy*2 + j*sx*2 + k] = (tmp1<<8) + tmp0;
			}
		}
	}
	return 0;
}

int flip4B(void *data, int sx, int sy, int sz)
{
	unsigned char *d = (unsigned char*)data;
	for(int i=0; i<sz; ++i){
		for(int j=0; j<sy; ++j){
			for(int k=0; k<sx; ++k){
				unsigned char tmp  = d[i*sx*sy*2 + j*sx*2 + k*2 + 0];
				unsigned char tmp2 = d[i*sx*sy*2 + j*sx*2 + k*2 + 1];
				d[i*sx*sy*2 + j*sx*2 + k*2 + 0] = d[i*sx*sy*2 + j*sx*2 + k*2 + 3];
				d[i*sx*sy*2 + j*sx*2 + k*2 + 1] = d[i*sx*sy*2 + j*sx*2 + k*2 + 2];
				d[i*sx*sy*2 + j*sx*2 + k*2 + 3] = tmp;
				d[i*sx*sy*2 + j*sx*2 + k*2 + 2] = tmp2;
			}
		}
	}
	return 0;
}

//----------------------------------------------------
int readData()
{
	Volume *volume = &mv->volumes[0];
	
	//see if we need to append the subvolume number
	char    subfile[512];
	if(mv->append)  
		sprintf(subfile, "%s.%04d.%02d", mv->path, 0, 0);
	else            
		sprintf(subfile, "%s", mv->path);
	cerr << "Reading: " << subfile << endl;

	if((mv->dataType == MetaVolume::_USHORT)){
		volume->nativeData = (void*)new unsigned short[(volume->xiSize*
																										volume->yiSize*
																										volume->ziSize)];		
		if(!read2B(subfile, volume)) return 0;
	}
	else if((mv->dataType == MetaVolume::_SHORT)){
		volume->nativeData = (void*)new short[(volume->xiSize*
			                                     volume->yiSize*
			                                     volume->ziSize)];
		if(!read2B(subfile, volume)) return 0;
	} 
	else if((mv->dataType == MetaVolume::_INT)){
		volume->nativeData = (void*)new int[(volume->xiSize*
			                                   volume->yiSize*
			                                   volume->ziSize)];
		if(!read4B(subfile, volume)) return 0;
	}
	else if((mv->dataType == MetaVolume::_UINT)){
		volume->nativeData = (void*)new unsigned int[(volume->xiSize*
			                                            volume->yiSize*
			                                            volume->ziSize)];
		if(!read4B(subfile, volume)) return 0;
	}
	else if((mv->dataType == MetaVolume::_FLOAT)){
		volume->nativeData = (void*)new float[(volume->xiSize*
			                                     volume->yiSize*
			                                     volume->ziSize)];
		if(!read4B(subfile, volume)) return 0;
	}
	else if((mv->dataType == MetaVolume::_UCHAR)){
		volume->nativeData = (void*)new unsigned char[(volume->xiSize*
			                                             volume->yiSize*
			                                             volume->ziSize)];
		if(!read1B(subfile, volume)){
			cerr << "ERROR: file not read" << endl;
			exit(1);
		}
	}

	if(mv->dataEndian != MetaVolume::_LITTLE_ENDIAN){
		cerr << " Changing endian" << endl;
		switch(mv->dataType){
		case MetaVolume::_SHORT:
		case MetaVolume::_USHORT:
			flip2B(volume->nativeData, volume->xiSize, volume->yiSize, volume->ziSize);
			mv->dataEndian = MetaVolume::_LITTLE_ENDIAN;
			break;
		case MetaVolume::_INT:
		case MetaVolume::_UINT:
		case MetaVolume::_FLOAT:
			flip2B(volume->nativeData, volume->xiSize, volume->yiSize, volume->ziSize);
			mv->dataEndian = MetaVolume::_LITTLE_ENDIAN;
			break;
		}
		cerr << "done" << endl;
	}

	return 1;
}

//============================================================ write Data
//=======================================================================
int writeData()
{
	FILE *f;
	if(!(f = fopen(outName, "wb"))){
	   cerr << "Error: writeData, failed to open " 
			<< outName << " for writing" << endl;
	   return 0;
	}

	int sx = mv->volumes[0].xiSize;
	int sy = mv->volumes[0].yiSize;
	int sz = mv->volumes[0].ziSize;

	fprintf(f,"NRRD00.01\n");
	fprintf(f,"number: %d\n", sx*sy*sz*3);
	fprintf(f,"type: unsigned char\n");
	fprintf(f,"dimension: 4\n");
	fprintf(f,"encoding: raw\n");
	fprintf(f,"endian: big\n");
	fprintf(f,"sizes: 3 %d %d %d\n", sx, sy, sz);
	fprintf(f,"spacings: nan0x7fffffff %f %f %f\n", mv->volumes[0].xfSize/sx, 
		                                              mv->volumes[0].yfSize/sy, 
															  									mv->volumes[0].zfSize/sz);
	fprintf(f,"labels: \"vgh\" \"x\" \"y\" \"z\"\n");
	fprintf(f,"\n");

	unsigned int n;
	n= fwrite((void *) mv->volumes[0].currentData, sizeof(unsigned char), sx*sy*sz*3, f);
	if( n != (unsigned int)(sx*sy*sz*3)){
		cerr << "Error: MetaVolume:writeVol, write failed " << n << " of " << sx*sy*sz << endl;
		return n;
	}

	cerr << "   " << outName << " successfully written" << endl;

	fclose(f);

	return n;
}

//================================================================= parse
//=======================================================================

void usage(){
	cerr << "usage: genVGH -t fileName.trex" << endl;
	cerr << "              -o outFileName" << endl;
}

int parse(int argc, char **argv)
{
	if(argc < 3){
		usage();
		return 1;
	}

	for(int i=1; i<argc; ++i){
		if(!strcmp(argv[i], "-t")){
			++i;
			mv = new MetaVolume(argv[i]);
		}
		if(!strcmp(argv[i], "-o")){
			++i;
			outName = strdup(argv[i]);
		}
	}

	if(!mv){
		cerr << "ERROR: input file undefined" << endl;
		usage();
		return 1;
	}

	return 0;
}
