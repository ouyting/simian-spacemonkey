//------------------------------------------------------------------------
//
//   Joe Kniss
//     create some interesting test volumes of arbitrary size
//          uses perlin noise to modify the frequency responce of 
//          the generated data sets
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../MetaVolume.h"
#include "../VectorMath.h"
#include "perlin.h"


MetaVolume mv;
int bricksz;
int brickdims[3];
int voltype;
int volparams[128];

int use_perl; //use perlin noise
int pharm;    //perlin harmonics
double pscale;   //perterbation scale
float  pwrap[3]; 
float  palpha;
float  pbeta;
int  ntype;

int use_blur; //duhh
float blurw[4];
int blurpass; //number of blur passes

enum {
	CUBES_VOL,
	SPHERES_VOL,
	STRIPES_VOL,
	PERLIN_VOL
};

enum {
	SIGNED_NOISE,
	UNSIGNED_NOISE,
	INV_UNSIGNED_NOISE
};

void makeCubes();
void makeSpheres();
void makeStripes();
void makePerl();

void blur();

void parse(int argc, char **argv);


int main(int argc, char **argv){
	bricksz = INT_MAX;
	brickdims[0] = -1;
	brickdims[1] = -1;
	brickdims[2] = -1;
	use_perl = 0;
	pharm = 1;
	pscale = 1;
	use_blur = 0;
	blurw[0] = 1;
	blurw[1] = 1;
	blurw[2] = 1;
	blurw[3] = 1;
  blurpass = 1;
	voltype = PERLIN_VOL;
	volparams[0] = 3;
	pwrap[0] = 1;
	pwrap[1] = 1;
	pwrap[2] = 1;
	palpha = 2;
	pbeta  = 2;
	ntype  = SIGNED_NOISE;

	parse(argc, argv);
	
	mv.volumes = new Volume;
	mv.numSubVols = 1;
	mv.append = 1;

	mv.volumes->xiSize = mv.xiSize;
	mv.volumes->yiSize = mv.yiSize;
	mv.volumes->ziSize = mv.ziSize;

	int maxsz = MAX(mv.xiSize, mv.yiSize);
	maxsz = MAX(maxsz, mv.ziSize);

	mv.xfSize = mv.volumes->xfSize = mv.xiSize/(float)maxsz;
	mv.yfSize = mv.volumes->yfSize = mv.yiSize/(float)maxsz;
	mv.zfSize = mv.volumes->zfSize = mv.ziSize/(float)maxsz;

	mv.volumes->currentData = mv.volumes->dbData[0] = new unsigned char[mv.xiSize*mv.yiSize*mv.ziSize];

	if(use_perl){
		init();
	}

	switch(voltype){
	case CUBES_VOL:
    makeCubes();
		break;
	case SPHERES_VOL:
		cerr << "Making a spherical volume" << endl;
		makeSpheres();
		break;
	case STRIPES_VOL:
		makeStripes();
		break;
	case PERLIN_VOL:
		makePerl();
		break;
	}

  for(int i=0; i<blurpass; ++i){
  	blur();
  }

	if(brickdims[0] == -1)
		mv.brick(bricksz);
	else
		mv.brick(brickdims[0], brickdims[1], brickdims[2]);
	mv.writeAll(mv.dataSetName);

  cerr << "all done it any key to exit" << endl;
  char c;
  cin >> c;

	return 0;
}

//============================================================================ PERL
//==================================================================================
inline double perl(double x,double y,double z){
	
	switch(ntype){
		case SIGNED_NOISE:
			return (PerlinNoise3D(x*pwrap[0],y*pwrap[1],z*pwrap[2],palpha,pbeta,pharm)*pscale);
		case UNSIGNED_NOISE:
			return (PerlinNoise3DABS(x*pwrap[0],y*pwrap[1],z*pwrap[2],palpha,pbeta,pharm)*pscale);
		case INV_UNSIGNED_NOISE:
			return (1.0- (PerlinNoise3DABS(x*pwrap[0],y*pwrap[1],z*pwrap[2],palpha,pbeta,pharm)*pscale));
	}
  return 0;
}

//============================================================================ Cubes
//==================================================================================
void makeCubes()
{
  cerr << " Making a cube with " << volparams[0] << " nested cubes" << endl;
  int sx = mv.volumes->xiSize;
	int sy = mv.volumes->yiSize;
	int sz = mv.volumes->ziSize;
	unsigned char *d = mv.volumes->currentData;

  unsigned char dd = (unsigned char)(1.0/(volparams[0]+2) * 255); //d-datavalue
  unsigned char cv = dd; //current data value
    
	for(int i=0; i<sz; ++i){
		for(int j=0; j<sy; ++j){
			for(int k=0; k<sx; ++k){
        d[i*sx*sy + j*sx + k] = (unsigned char) cv;     
      }
    }
  }

  int size = (int)(.15 * sqrt(sx*sx + sy*sy + sz*sz));

  for(int g=0; g<volparams[0]; ++g){
    cv += dd;    
    int posx = (int)((rand()/(float)RAND_MAX *.9) * sx);
    int posy = (int)((rand()/(float)RAND_MAX *.9) * sy);
    int posz = (int)((rand()/(float)RAND_MAX *.9) * sz);
    int csx = (posx + size) > sx ? (sx - posx) : size;
    int csy = (posy + size) > sy ? (sy - posy) : size;
    int csz = (posz + size) > sz ? (sz - posz) : size;
    cerr << "sub cube pos " << posx << " " << posy << " " << posz << endl;
    for(int i=0; i<csz; ++i){
      for(int j=0; j<csy; ++j){
        for(int k=0; k<csx; ++k){
          d[(posz+i)*sx*sy + (posy+j)*sx + (posx+k)] = (unsigned char) cv;     
        }
      }
    }
  }

  cerr << " -done" << endl;
}

//========================================================================== Spheres
//==================================================================================
void makeSpheres()
{
	cerr << " Making " << volparams[0] << " concentric spheres " << endl;
	if(use_perl) cerr << "    -using perlin noise" << endl;
	
	int sx = mv.volumes->xiSize;
	int sy = mv.volumes->yiSize;
	int sz = mv.volumes->ziSize;
	unsigned char *d = mv.volumes->currentData;
	
	float dd   = 255/(float)volparams[0];
	float c[3] = {.5,.5,.5}; //center of volume

	float dx = 1/(float)sx;
	float dy = 1/(float)sy;
	float dz = 1/(float)sz;

	for(int i=0; i<sz; ++i){
		for(int j=0; j<sy; ++j){
			for(int k=0; k<sx; ++k){
				float p[3] = {k*dx, j*dy, i*dz};
				float v[3];
				subV3(v, p, c);
				float r = (float)MIN(normV3(v), .48);
				if(use_perl){
					//cerr << PerlinNoise3D(p[0],p[1],p[2],2,2,pharm) << " ";
					r += (float)perl(p[0], p[1], p[2]);
#if 0
					r += (float)(PerlinNoise3D(p[0]*pwrap[0],
						                         p[1]*pwrap[1],
																		 p[2]*pwrap[2],
																		 palpha,pbeta,pharm)*pscale);
#endif
					r = (float)CLAMP_ARB(0,r,.5);
				}

				int val = (int)(r*2 * volparams[0]);
				d[i*sx*sy + j*sx + k] = (unsigned char)((volparams[0] - val) * dd);
			}
		}
	}

	cerr << "        -done" << endl;

}

//========================================================================== Stripes
//==================================================================================
void makeStripes()
{
	cerr << " Making " << volparams[0] << " stripes " << endl;
	if(use_perl) cerr << "    -using perlin noise" << endl;
	
	int sx = mv.volumes->xiSize;
	int sy = mv.volumes->yiSize;
	int sz = mv.volumes->ziSize;
	
	unsigned char *d = mv.volumes->currentData;
	
	float dd   = 255/(float)volparams[0];

	float dx = 1/(float)sx;
	float dy = 1/(float)sy;
	float dz = 1/(float)sz;

	for(int i=0; i<sz; ++i){
		for(int j=0; j<sy; ++j){
			for(int k=0; k<sx; ++k){
				float p[3] = {k*dx, j*dy, i*dz};

				float r = p[2];
				if(use_perl){
					//cerr << PerlinNoise3D(p[0],p[1],p[2],2,2,pharm) << " ";
					r += (float)perl(p[0],p[1],p[2]);
#if 0
					r += (float)((PerlinNoise3D(p[0]*pwrap[0],
						                         p[1]*pwrap[1],
																		 p[2]*pwrap[2],
																		 palpha,pbeta,pharm)*pscale));
#endif
					r = CLAMP(r);
				}

				int val = (int)(r * volparams[0]);
				d[i*sx*sy + j*sx + k] = (unsigned char)((volparams[0] - val) * dd);
			}
		}
	}

	cerr << "        -done" << endl;
}

//=========================================================================== Perlin
//==================================================================================
void makePerl()
{
	int sx = mv.volumes->xiSize;
	int sy = mv.volumes->yiSize;
	int sz = mv.volumes->ziSize;
	unsigned char *d = mv.volumes->currentData;
	
	float dd   = 255/(float)volparams[0];

	float dx = 1/(float)sx;
	float dy = 1/(float)sy;
	float dz = 1/(float)sz;

	for(int i=0; i<sz; ++i){
		for(int j=0; j<sy; ++j){
			for(int k=0; k<sx; ++k){
				float p[3] = {k*dx, j*dy, i*dz};
					
				float r = (float)ABS(PerlinNoise3D(p[0]*pwrap[0],p[1]*pwrap[1],p[2]*pwrap[2],palpha,pbeta,pharm));
				int val = (int)(r * volparams[0]);
				d[i*sx*sy + j*sx + k] = (unsigned char)((volparams[0] - val) * dd);
			}
		}
	}


}

void blur()
{
#if 0
	if(use_blur){
		blurVARB(mv.volumes->currentData,blurw[0],blurw[1],blurw[2],blurw[3],
			mv.volumes->xiSize,mv.volumes->yiSize,mv.volumes->ziSize,1);
	}
#else

	if(!use_blur){
		cerr << " no blur" << endl;
		return;
	}
	
	int sx = mv.xiSize;
	int sy = mv.yiSize;
	int sz = mv.ziSize;

	cerr << "BLUR :   " << sx << "  " << sy  << "  " << sz <<  endl;
	cerr << " weights ; " << blurw[0] << " "<< blurw[1] << " "<< blurw[2] << " " << blurw[3] << endl;
	

	float *tmp = new float[sx*sy*sz];


	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				int stz = sx*sy;
				int sty = sx;
					tmp[i*stz + j*sty + k] = 0.0;
			}
		}
	}
	
	unsigned char *dataV = mv.volumes->currentData;

	for(i = 1; i<(sz-1); ++i){
		for(int j = 1; j<(sy-1); ++j){
			for(int k = 1; k<(sx-1); ++k){
				int stz = sx*sy;
				int sty = sx;
					int index = i*sx*sy + j*sx + k;
					float v0 = (float)(dataV[index]/255.0*blurw[0]);
					float v1 = (float)(dataV[index]/255.0*blurw[1]/6.0);
					float v2 = (float)(dataV[index]/255.0*blurw[2]/12.0);
					float v3 = (float)(dataV[index]/255.0*blurw[3]/8.0);
					
				  tmp[i*sx*sy + j*sx + k] += v0; //1
						tmp[(i+1)*sx*sy + j*sx + k] += v1; //1
							tmp[(i+1)*sx*sy + (j+1)*sx + k] += v2; //1
								tmp[(i+1)*sx*sy + (j+1)*sx + (k+1)] += v3;	//1					
								tmp[(i+1)*sx*sy + (j+1)*sx + (k-1)] += v3;	//2					
							tmp[(i+1)*sx*sy + (j-1)*sx + k] += v2; //2
								tmp[(i+1)*sx*sy + (j-1)*sx + (k+1)] += v3; //3
								tmp[(i+1)*sx*sy + (j-1)*sx + (k-1)] += v3; //4
							tmp[(i+1)*sx*sy + j*sx + (k+1)] += v2; //3
							tmp[(i+1)*sx*sy + j*sx + (k-1)] += v2; //4

						tmp[(i-1)*sx*sy + j*sx + k] += v1; //2
							tmp[(i-1)*sx*sy + (j+1)*sx + k] += v2; //5
								tmp[(i-1)*sx*sy + (j+1)*sx + (k+1)] += v3;	//5				
								tmp[(i-1)*sx*sy + (j+1)*sx + (k-1)] += v3;	//6					
							tmp[(i-1)*sx*sy + (j-1)*sx + k] += v2; //6
								tmp[(i-1)*sx*sy + (j-1)*sx + (k+1)] += v3; //7
								tmp[(i-1)*sx*sy + (j-1)*sx + (k-1)] += v3; //8
							tmp[(i-1)*sx*sy + j*sx + (k+1)] += v2; //7
							tmp[(i-1)*sx*sy + j*sx + (k-1)] += v2; //8

						tmp[i*sx*sy + (j+1)*sx + k] += v1; //3
							tmp[i*sx*sy + (j+1)*sx + (k+1)] += v2; //9
							tmp[i*sx*sy + (j+1)*sx + (k-1)] += v2; //10
						tmp[i*sx*sy + (j-1)*sx + k] += v1; //4
							tmp[i*sx*sy + (j-1)*sx + (k+1)] += v2; //11
							tmp[i*sx*sy + (j-1)*sx + (k-1)] += v2; //12
						tmp[i*sx*sy + j*sx + (k+1)] += v1; //5
						tmp[i*sx*sy + j*sx + (k-1)] += v1; //6
			}
		}
	}

	//float div = w0 + 6*w1 + 12*w2 + 8*w3;
	float div = blurw[0] + blurw[1] + blurw[2] + blurw[3];

	for(i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				int stz = sx*sy;
				int sty = sx;
					dataV[i*stz + j*sty + k] = (unsigned char)(CLAMP((tmp[i*sx*sy + j*sx + k]/div))*255);
			}
		}
	}
	
	delete[] tmp;  //why does this explode?????
#endif
}


//============================================================================ Parse
//==================================================================================
void parse(int argc, char **argv)
{
	if(argc < 3){
		cerr << argv[0] << " usage: " << endl;
		cerr << "     -o outFileName  = file name of generated volume " << endl 
		     << "          {creates outFileName.xxxx.xx & outFileName.trex}" << endl;
		cerr << "     -s sizeX sizeY sizeZ = dimensions of volume in voxels"  << endl;
		cerr << "    <-b maxBrickSize>= maximum size of a brick in voxels (bytes)" << endl;
		cerr << "    <-bd bx by bx>   = brick dimensions" << endl;
		cerr << endl;
		cerr << "    <-c numCubes>       = volume of cubes with numCubes inside" << endl; 
		cerr << "    <-spheres numShels> = volume of spheres with concentric shells" << endl;
		cerr << "    <-stripes numLines> = volume of stripes with numLines for frequency" << endl;
		cerr << endl;
		cerr << "    <-seed seedValue> = seed integer for rand() calls (int)" << endl;
		cerr << "    <-p harmonics>    = use perlin noise with some number of harmonics (int)" << endl;
		cerr << "    <-pscale scale>   = perlin turbulence scale factor (float)" << endl;
		cerr << "    <-pwrap wx wy wz> = wrap perlin domain some amount (float)" << endl;
		cerr << "    <-pa alpha>       = alpha scale for harmonic summation (float)" << endl;
		cerr << "    <-pb beta>        = beta scale for harmonic summation (float)" << endl;
		cerr << "                         sum_i(n(i*b*x,i*b*y,i*b*z)*(1/(i*alpha)))" << endl;
		cerr << "    <-pabs>           = use absolute value of noise" << endl;
		cerr << "    <-p1-abs>         = use 1 - absolute value of noise" << endl;
		cerr << endl;
		cerr << "    <-blur>  = blur volume (just a little) for smoothness" << endl;
    cerr << "    <-blurpass blurpasses> =  number of blur passes" << endl;
		cerr << "    <-bw w0 w1 w2 w3> = blur weights" << endl;
		exit(1);
	}

	int i = 1;
	while(i < argc){
		if(!strcmp(argv[i], "-o")){
			++i;
			mv.dataSetName = strdup(argv[i]);
		}
		else if(!strcmp(argv[i], "-s")){
			++i;
			mv.xiSize = atoi(argv[i]);
			++i;
			mv.yiSize = atoi(argv[i]);
			++i;
			mv.ziSize = atoi(argv[i]);
		}
		else if(!strcmp(argv[i], "-b")){
			++i;
			bricksz = atoi(argv[i]);
		}
		else if(!strcmp(argv[i], "-bd")){	cerr << "    <-bd bx by bx>   = brick dimensions" << endl;
			++i;
			brickdims[0] = atoi(argv[i]);
			++i;
			brickdims[1] = atoi(argv[i]);
			++i;
			brickdims[2] = atoi(argv[i]);
		}
		else if(!strcmp(argv[i], "-c")){
			++i;
			volparams[0] = atoi(argv[i]);
			voltype = CUBES_VOL;
		}
		else if(!strcmp(argv[i], "-spheres")){
			++i;
			volparams[0] = atoi(argv[i]);
			voltype = SPHERES_VOL;
		}
		else if(!strcmp(argv[i], "-stripes")){
			++i;
			volparams[0] = atoi(argv[i]);
			voltype = STRIPES_VOL;
		}
		else if(!strcmp(argv[i], "-seed")){
			++i;
			srand(atoi(argv[i]));
		}
		else if(!strcmp(argv[i], "-p")){
			++i;
			pharm = atoi(argv[i]);
			use_perl = 1;
		}
		else if(!strcmp(argv[i], "-pscale")){
			++i;
			pscale = atof(argv[i]);
		}
		else if(!strcmp(argv[i], "-pwrap")){
			++i;
			pwrap[0] = (float)atof(argv[i]);
			++i;
			pwrap[1] = (float)atof(argv[i]);
			++i;
			pwrap[2] = (float)atof(argv[i]);
		}
		else if(!strcmp(argv[i], "-pa")){
			++i;
			palpha = (float)atof(argv[i]);
		}
		else if(!strcmp(argv[i], "-pb")){
			++i;
			pbeta = (float)atof(argv[i]);
		}
		else if(!strcmp(argv[i], "-pabs")){
			++i;
			ntype = UNSIGNED_NOISE;
		}
		else if(!strcmp(argv[i], "-p1-abs")){
			++i;
			ntype = INV_UNSIGNED_NOISE;
		}
		else if(!strcmp(argv[i], "-blur")){
			use_blur = 1;
		}
    else if(!strcmp(argv[i], "-blurpass")){
			++i;
			blurpass = atoi(argv[i]);
		}
		else if(!strcmp(argv[i], "-bw")){
			++i;
			blurw[0] = (float)atof(argv[i]);
			++i;
			blurw[1] = (float)atof(argv[i]);
			++i;
			blurw[2] = (float)atof(argv[i]);
			++i;
			blurw[3] = (float)atof(argv[i]);
		}

		++i;
	}

}


