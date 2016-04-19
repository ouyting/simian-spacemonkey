//<html><a href="MetaVolume.cpp">source</a> <plaintext>
//--------------------------------------------------------------------
// MetaVolume.h
//  also defines class Volume
//  stores information about a volume, includes a reader/parser for .trex files
//   jmk 6-25-00
//--------------------------------------------------------------------


#ifndef __MetaVolume_H
#define __MetaVolume_H

#include <stdio.h>


class NrrdC;

class Volume{
 public:
  Volume();
  Volume(Volume &v); //copy constructor does not copy data!
  ~Volume();

	unsigned int writeVol(FILE *trex, char *filename);

  int           subVolNum;
  
  int           xiSize, yiSize, ziSize;
	int           xiVSize, yiVSize, ziVSize; // size of the original volume (before padding etc)
  float         xfSize, yfSize, zfSize;
  float         xfVSize, yfVSize, zfVSize; // size of the original volume (before padding etc)

  int           xiPos, yiPos, ziPos;
  float         xfPos, yfPos, zfPos;
  
  unsigned char *dbData[2];   //points to two blocks of data
	//unsigned char *dbData1[2];     //two blocks for gradient magnitude (secondary data)
	//unsigned char *dbData2[2];     //two blocks for gradient magnitude (secondary data)
	//unsigned char *dbData3[2];     //two blocks for gradient magnitude (secondary data)
	unsigned char *dbGrad[2];   // two blocks for gradients (Vectors)
  unsigned char *currentData; //points to the right block of data
	//unsigned char *currentData1; //points to the right gradient
	//unsigned char *currentData2; //points to the right gradient
	//unsigned char *currentData3; //points to the right gradient
	unsigned char *currentGrad;

	unsigned char **oldData;       //ring buffer for cached data
	unsigned char **oldGrad;       //ring buffer for cached gradients
	int            *oldTimeStep;
	int             tstepCache;
	int             oldTSpos;

  int           extradivs;

	void *nativeData;
 
	int timestep;

 private:
	
};

class MetaVolume{
 public:
  MetaVolume(const char *filename, int isnrrd=0);  //filename = trex/nrrd file
	MetaVolume();
  ~MetaVolume();

	unsigned int  readVol(int volNum, int timestep = 0); //read a sub-volume
	unsigned int  readAll(int timestep = 0);             //read all sub-volumes
	unsigned int  readNrrd(int debug);                   //read a nrrd
  
	unsigned int  swapTStep(int timestep);               //try to swap a cached timestep (0 = need to read)
	unsigned int  cacheTStep();                          //try to cache the current timestep (0=didn't happen)

	unsigned int  writeAll(char *filename);

	int           mergeMV(int blurnorms,                 //blur normals???
		                    int addG, int addH,            //add grad and hessian??
		                    MetaVolume *mv1 = 0,           //combine metavolumes
												MetaVolume *mv2 = 0, 
												MetaVolume *mv3 = 0);

	void          normalsVGH(int blurrnorms=0,  //blur normals???
		                       int only1st = 0);  //compute normals, only use first element??
	void          padOut();                  //padd to nearest pow2 larger
	void          brick(int maxsz);          //brick if larger than maxsz (in voxels)
	void          brick(int bx, int by, int bz); 

	int           hist2D(unsigned char *hist);  //be sure you already created the data

	enum{
     _DOUBLE,//8byte
     _FLOAT, //4byte
     _INT,   //4byte
		 _UINT,  //4byte
     _SHORT, //2byte
		 _USHORT,//2byte
     _UCHAR, //1byte
  };

	enum{
		_BIG_ENDIAN,
		_LITTLE_ENDIAN
	};

  char         *pipes[32];  //which pipes are we useing??
  int           npipes;     //how many pipes are available??

  int           isValid(void) { return valid; };

  int           output(char *outfile);  //write out the current metavolume
  void          printInfo();            //print metavolume info to std err

  int           valid;                  //is this metavolume ready to go??
  
  int           dataType;               //uses enum above
	int           dataEndian;
  char         *dataSetName;            //not sure if this is used?
  char         *nativeDataSetName;

  int          append;                  //append subvolume & timestep extensions
  char         *path;                   //the path to the actual data
  char         *TLUTfile;               //is there a saved tlut??

  char         *baneFile;               //is there a semi-automatic tf for this
  char         *nrrdFile;  //you can test this to see if it came from a nrrd

  int           tsteps;                 //number of time steps
  int           tstart;                 //starting time step
  int           tstop;                  //ending timestep
	int           currentTStep;           //what time step are we on?
	int           tstepCache;             //number of timesteps to cache

  int           xiSize, yiSize, ziSize; //size of the whole volume in voxels
	int           xiVSize, yiVSize, ziVSize; // size of the original volume (before padding etc)
  float         xfSize, yfSize, zfSize; //size of the whole volume in volume space
  float         xfVSize, yfVSize, zfVSize; //size of the whole volume in volume space (before ...)
	float         xSpc, ySpc, zSpc;       //spacings between voxels
  int           numSubVols;             //number of sub-volumes (#of *volumes[]*)
  int           nelts; //number of elements in each data buffer (usualy 1)
	int           nVelts; //number of elements in original volume
	int           dims;                   //????


  Volume       *volumes;                //all sub volumes are kept here
	Volume       *wholeVol;               //the whole volume - unbricked
	
private:

	//there must be a better way!!!!
	void           pow2Celing(unsigned char *&data0,  //these are created for you   
		                        //unsigned char *&data1,  //if d1 == 0
														//unsigned char *&data2,  //  ..then data1 = 0 ...
														//unsigned char *&data3,
														unsigned char *&grad,
														const unsigned char *d0,  //data comes in from these
														//const unsigned char *d1,
														//const unsigned char *d2,
														//const unsigned char *d3,
														const unsigned char *g,
														int &sx, int &sy, int &sz);

	int            isPow2(int sz);   //is sz a perfect power of 2
	int            getPow2(int sz);  //get the power of 2 greater than or equal to sz
	int            parse(const char *filename);
	int            parseNrrd(char *line);
  char          *wtspc(char *str);
  void           spcwt(char *str);
};

#endif
