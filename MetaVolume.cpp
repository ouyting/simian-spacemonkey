//                   ________    ____   ___ 
//                  |        \  /    | /  /
//                  +---+     \/     |/  /
//                  +--+|  |\    /|     < 
//                  |  ||  | \  / |  |\  \ 
//                  |      |  \/  |  | \  \ 
//                   \_____|      |__|  \__\
//                       Copyright  2001 
//                      Joe Michael Kniss
//                    << jmk@cs.utah.edu >>
//               "All Your Base are Belong to Us"
//-------------------------------------------------------------------------



#include "MetaVolume.h"
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "VectorMath.h"
//#include <NrrdC.h>


Volume::Volume()
{
   xiSize = INT_MAX;
   yiSize = INT_MAX;
   ziSize = INT_MAX;
	 xiVSize = INT_MAX;
   yiVSize = INT_MAX;
   ziVSize = INT_MAX;
   xfSize = 0.0;
   yfSize = 0.0;
   zfSize = 0.0;
	 xfVSize = 0.0;
   yfVSize = 0.0;
   zfVSize = 0.0;
   xiPos = 0;
   yiPos = 0;
   ziPos = 0;
   xfPos = 0.0;
   yfPos = 0.0;
   zfPos = 0.0;
   subVolNum = 0;
   dbData[0] = 0;
   dbData[1] = 0;
	 dbGrad[0] = 0;
	 dbGrad[1] = 0;
   currentData = 0;
	 currentGrad = 0;
   extradivs = 0;
	 oldData = 0;
	 oldTimeStep = 0;
	 tstepCache = 0;
}

Volume::Volume(Volume &v)
{
   xiSize = v.xiSize;
   yiSize = v.yiSize;
   ziSize = v.ziSize;
	 xiVSize = v.xiVSize;
   yiVSize = v.yiVSize;
   ziVSize = v.ziVSize;
   xfSize = v.xfSize;
   yfSize = v.yfSize;
   zfSize = v.zfSize;
	 xfVSize = v.xfVSize;
   yfVSize = v.yfVSize;
   zfVSize = v.zfVSize;
   xiPos = v.xiPos;
   yiPos = v.yiPos;
   ziPos = v.ziPos;
   xfPos = v.xfPos;
   yfPos = v.yfPos;
   zfPos = v.zfPos;
   subVolNum = v.subVolNum;
   dbData[0] = 0;
   dbData[1] = 0;
	 dbGrad[0] = 0;
	 dbGrad[1] = 0;
   currentData = 0;
	 currentGrad = 0;
   extradivs = 0;
	 oldData = 0;
	 oldTimeStep = 0;
	 tstepCache = v.tstepCache;
}

Volume::~Volume()
{
   if(dbData[0]) delete dbData[0];
   if(dbData[1]) delete dbData[1];
}

unsigned int Volume::writeVol(FILE *trex, char *filename)
{
	fprintf(trex, "SubVolume {\n");
	fprintf(trex, "\tSize int:        %d, %d, %d\n", xiSize, yiSize, ziSize);
	fprintf(trex, "\tSize float:      %f, %f, %f\n", xfSize, yfSize, zfSize);
	fprintf(trex, "\tPos int:         %d, %d, %d\n", xiPos, yiPos, ziPos);
	fprintf(trex, "\tPos float:       %f, %f, %f\n", xfPos, yfPos, zfPos);
	fprintf(trex, "}\n");

	FILE *f;
	if(!(f = fopen(filename, "wb"))){
	   cerr << "Error: MetaVolume::writeAll, failed to open " 
			<< filename << " for writing" << endl;
	   return 0;
	}

	unsigned int n;
	n= fwrite((void *) currentData, sizeof(unsigned char), xiSize*yiSize*ziSize, f);
	if( n != (unsigned int)(xiSize*yiSize*ziSize)){
		cerr << "Error: MetaVolume:writeVol, write failed " << n << " of " << xiSize*yiSize*ziSize << endl;
		return n;
	}

	cerr << "   " << filename << " successfully written" << endl;

	fclose(f);

	return n;
}


//========================================================================
//============================= Meta Volume ==============================
//========================================================================

MetaVolume::MetaVolume(const char *filename, int isnrrd)
{
   dataSetName = NULL;
   nativeDataSetName = NULL;
   path = NULL;
   TLUTfile = NULL;
   baneFile = NULL;
   nrrdFile = NULL;
   tsteps = 0;
	 tstart = 0;
	 tstop  = 0;
	 currentTStep = 0;
	 tstepCache = 0;
   xiSize = INT_MAX;
   yiSize = INT_MAX;
   ziSize = INT_MAX;
	 xiVSize = INT_MAX;
   yiVSize = INT_MAX;
   ziVSize = INT_MAX;
   xfSize = 0.0;
   yfSize = 0.0;
   zfSize = 0.0;
	 xfVSize = 0.0;
   yfVSize = 0.0;
   zfVSize = 0.0;
	 xSpc   = 1.0;
	 ySpc   = 1.0;
	 zSpc   = 1.0;
   npipes = 0;
   numSubVols = 0;
	 nelts = 1;
	 nVelts = 1;
	 dims  = 3;

	 wholeVol = 0;

	 if(!isnrrd){
		 if (parse(filename) < 0)
			 valid = 0;
		 else
			 valid = 1;
	 } else {
		 nrrdFile = strdup(filename);	
	 }
  
	 //if(nrrdFile){  don't do this here, takes too long
   //  nrrdc = new NrrdC(nrrdFile);
   //} 
}

MetaVolume::MetaVolume()
{
   dataSetName = NULL;
   nativeDataSetName = NULL;
   path = NULL;
   TLUTfile = NULL;
   baneFile = NULL;
   nrrdFile = NULL;
	 tsteps = 0;
	 tstart = 0;
	 tstop  = 0;
	 currentTStep = 0;
	 tstepCache = 0;
   xiSize = 0;
   yiSize = 0;
   ziSize = 0;
	 xiVSize = 0;
   yiVSize = 0;
   ziVSize = 0;
   xfSize = 0.0;
   yfSize = 0.0;
   zfSize = 0.0;
	 xfVSize = 0.0;
   yfVSize = 0.0;
   zfVSize = 0.0;
	 xSpc   = 1.0;
	 ySpc   = 1.0;
	 zSpc   = 1.0;
   npipes = 0;
   numSubVols = 0;
	 nelts = 1;
	 nVelts = 1;
	 dims  = 3;

	 wholeVol = 0;
	
}

MetaVolume::~MetaVolume()
{
  free((void *)dataSetName);
  free((void *)nativeDataSetName);
  free((void *)path);
  free((void *)TLUTfile);
}

//=============================================================== PARSE
//======================================================================

int MetaVolume::parse(const char *filename)
{
	FILE *f;
	if(!(f = fopen(filename, "r"))){
		cerr << "Error :: MetaVolume() : Could not open '" 
			<< filename << "' for reading.\n";
		return -1;
	}
	
	int strlen = 128;
	char str[128];
	char *tmp = 0;
	int subv = 0;
	
	//default data type.
	dataType = _UCHAR;  
	append   = 1;
	
	//init pipe info
	npipes = 0;
	
	while(fgets(str, strlen, f)){
		tmp = strtok(str, ":{\n");
		tmp = wtspc(tmp);
		
		if(tmp){
      if(!strcmp(tmp, "Data Type")){
				tmp = strtok(NULL, "\n\0");
				tmp = wtspc(tmp);
				if(tmp == NULL) cerr << "Data Type not read, syntax error\n";
				else{
					if((!strcmp(tmp, "float")) || (!strcmp(tmp, "FLOAT")) || 
						(!strcmp(tmp, "Float"))){
						dataType = _FLOAT;
					}
					else if((!strcmp(tmp, "double")) || (!strcmp(tmp, "DOUBLE")) || 
						(!strcmp(tmp, "Double"))){
						dataType = _DOUBLE;
					}
					else if((!strcmp(tmp, "int")) || (!strcmp(tmp, "INT")) || 
						(!strcmp(tmp, "Int"))){
						dataType = _INT;
					}
					else if((!strcmp(tmp, "uint")) || (!strcmp(tmp, "UINT")) || 
						(!strcmp(tmp, "UInt"))){
						dataType = _UINT;
					}
					else if((!strcmp(tmp, "short")) || (!strcmp(tmp, "SHORT")) || 
						(!strcmp(tmp, "Short"))){
						dataType = _SHORT;
					}
					else if((!strcmp(tmp, "ushort")) || (!strcmp(tmp, "USHORT")) || 
						(!strcmp(tmp, "UShort"))){
						dataType = _USHORT;
					}
					else if((!strcmp(tmp, "uchar")) || (!strcmp(tmp, "UCHAR")) || 
						(!strcmp(tmp, "Uchar"))){
						dataType = _UCHAR;
					}
				}
      }
      else if((!strcmp(tmp, "Time Step Cache")) || (!strcmp(tmp,"Time step cache")) || 
				      (!strcmp(tmp, "time step cache"))){
				tmp = strtok(NULL, "\n\0");
				tmp = wtspc(tmp);
				if(tmp == NULL) cerr << "Time Step Cache not read, syntax error\n";
				else{
					tstepCache = atoi(tmp);
					cerr << "I will Cache " << tstepCache << " time steps" << endl;
				}
			}
      else if((!strcmp(tmp, "ENDIAN")) || (!strcmp(tmp,"Endian")) || (!strcmp(tmp, "endian"))){
				tmp = strtok(NULL, "\n\0");
				tmp = wtspc(tmp);
				if(tmp == NULL) cerr << "Endian not read, syntax error\n";
				else{
					if((!strcmp(tmp,"BIG")) || (!strcmp(tmp, "big")) || (!strcmp(tmp, "Big"))){
						dataEndian = _BIG_ENDIAN;
					}
					if((!strcmp(tmp,"LITTLE")) || (!strcmp(tmp, "little")) || (!strcmp(tmp, "Little"))){
						dataEndian = _LITTLE_ENDIAN;
					}
				}
			}
      else if(!strcmp(tmp, "Displays")){
				while(tmp = strtok(NULL, ",\n\0")){
					tmp = wtspc(tmp);
					cout << " Display requested: " << tmp << endl;
					pipes[npipes++] = strdup(tmp);
				}
      }
			
      else if(!strcmp(tmp, "Don't append numbers")){
				append = 0;
				cout << "No knowledge of pre-bricking or time seriese" << endl;
      }
			
      else if(!strcmp(tmp, "Data Set Name")){
				tmp = strtok(NULL, "\n\0");
				tmp = wtspc(tmp);
				if(tmp == NULL) cerr << "Data Set Name not read, syntax error\n";
				else{
					dataSetName = strdup(tmp);
				}
      } //data set name
      
      else if(!strcmp(tmp, "Native Data Set Name")){
				tmp = strtok(NULL, "\n\0");
				tmp = wtspc(tmp);
				if(tmp == NULL) cerr << "Native Data Set Name not read, syntax error\n";
				else{
					nativeDataSetName = strdup(tmp);
				}
      } //native data set files (with original float vals)
			
      else if(!strcmp(tmp, "Data Set Files")){
				tmp = strtok(NULL, "\n\0");
				tmp = wtspc(tmp);
				if(tmp == NULL) cerr << "Data Set Files not read, syntax error\n";
				else{
					path = strdup(tmp);
				}
      } //data set files
			
      else if(!strcmp(tmp, "Number of Time Steps")){
				tmp = strtok(NULL, ",\n\0");
				tmp = wtspc(tmp);
				if(tmp == NULL) 
					cerr << "Number of Time Steps not read, syntax error\n";
				else{
					tsteps = atoi(tmp);
				}
				tmp = strtok(NULL, ",\n\0");
				tmp = wtspc(tmp);
				if(tmp == NULL) 
					cerr << "Time Step Start not read, syntax error\n";
				else{
					tstart = atoi(tmp);
				}
				tmp = strtok(NULL, ",\n\0");
				tmp = wtspc(tmp);
				if(tmp == NULL) 
					cerr << "Time Step Stop not read, syntax error\n";
				else{
					tstop = atoi(tmp);
				}
				
      } //number of time steps
			
      else if(!strcmp(tmp, "TLUT File")){
				tmp = strtok(NULL, "\n\0");
				tmp = wtspc(tmp);
				if(tmp == NULL) cerr << "TLUT file not read, syntax error\n";
				else{
					TLUTfile = strdup(tmp);
				}
      } //tlut file
			
      else if(!strcmp(tmp, "Bane File")){
				tmp = strtok(NULL, "\n\0");
				tmp = wtspc(tmp);
				if(tmp == NULL) cerr << "Bane file not read, syntax error\n";
				else{
					baneFile = strdup(tmp);
				}
      } //tlut file
			
      else if(!strcmp(tmp, "Nrrd File")){
				tmp = strtok(NULL, "\n\0");
				tmp = wtspc(tmp);
				if(tmp == NULL) cerr << "Nrrd file not read, syntax error\n";
				else{
					nrrdFile = strdup(tmp);
				}
      } //tlut file
      
      else if(!strcmp(tmp, "Volume Size int")){
				tmp = strtok(NULL, ",");
				tmp = wtspc(tmp);
				if(tmp == NULL) 
					cerr << "Volume size int (x) not read, syntax error\n";
				else{
					xiVSize = xiSize = atoi(tmp);
				}
				tmp = strtok(NULL, ",");
				tmp = wtspc(tmp);
				if(tmp == NULL) 
					cerr << "Volume size int (y) not read, syntax error\n";
				else{
					yiVSize = yiSize = atoi(tmp);
				}
				tmp = strtok(NULL, "\n\0");
				tmp = wtspc(tmp);
				if(tmp == NULL) 
					cerr << "Volume size int (z) not read, syntax error\n";
				else{
					ziVSize = ziSize = atoi(tmp);
				}
      } //volume size int
      
      else if(!strcmp(tmp, "Volume Size float")){
				tmp = strtok(NULL, ",");
				tmp = wtspc(tmp);
				if(tmp == NULL) 
					cerr << "Volume size float (x) not read, syntax error\n";
				else{
					xfVSize = xfSize = (float)atof(tmp);
				}
				tmp = strtok(NULL, ",");
				tmp = wtspc(tmp);
				if(tmp == NULL) 
					cerr << "Volume size float (y) not read, syntax error\n";
				else{
					yfVSize = yfSize = (float)atof(tmp);
				}
				tmp = strtok(NULL, "\n\0");
				tmp = wtspc(tmp);
				if(tmp == NULL) 
					cerr << "Volume size float (z) not read, syntax error\n";
				else{
					zfVSize = zfSize = (float)atof(tmp);
				}
      } // volume size float
      
      else if(!strcmp(tmp, "Number of Sub Volumes")){
				tmp = strtok(NULL, "\n\0");
				tmp = wtspc(tmp);
				if(tmp == NULL) 
					cerr << "Number of Sub Volumes not read, syntax error\n";
				else{
					numSubVols = atoi(tmp);
					volumes = new Volume[numSubVols];
				}
      } //number of Sub Volumes
      
      else if(!strcmp(tmp, "SubVolume")){
				if(!fgets(str, strlen, f)){
					cerr << "Error parsing " << filename 
						<< ": SubVolume{" << endl;
					return 0;
				}
				tmp = str;
				tmp = wtspc(tmp);
				spcwt(tmp);
				if(volumes == NULL){
					cerr << "Error: Number of subvolumes not known" << endl;
					return 0;
				}
				while(strcmp(str, "}")){
					tmp = strtok(str, ":");
					tmp = wtspc(tmp);
					if(!strcmp(tmp, "Size int")){
						tmp = strtok(NULL, ",");
						tmp = wtspc(tmp);
						if(tmp == NULL) 
							cerr << "Size int (x) not read, syntax error\n";
						else{
							volumes[subv].xiVSize = volumes[subv].xiSize = atoi(tmp);
						}
						tmp = strtok(NULL, ",");
						tmp = wtspc(tmp);
						if(tmp == NULL) 
							cerr << "Size int (y) not read, syntax error\n";
						else{
							volumes[subv].yiVSize = volumes[subv].yiSize = atoi(tmp);
						}
						tmp = strtok(NULL, "\n\0");
						tmp = wtspc(tmp);
						if(tmp == NULL) 
							cerr << "Size int (z) not read, syntax error\n";
						else{
							volumes[subv].ziVSize = volumes[subv].ziSize = atoi(tmp);
						}
					} //size int
					
					else if(!strcmp(tmp, "Size float")){
						tmp = strtok(NULL, ",");
						tmp = wtspc(tmp);
						if(tmp == NULL) 
							cerr << "Size float (x) not read, syntax error\n";
						else{
							volumes[subv].xfVSize = volumes[subv].xfSize = (float)atof(tmp);
						}
						tmp = strtok(NULL, ",");
						tmp = wtspc(tmp);
						if(tmp == NULL) 
							cerr << "Size float (y) not read, syntax error\n";
						else{
							volumes[subv].yfVSize = volumes[subv].yfSize = (float)atof(tmp);
						}
						tmp = strtok(NULL, "\n\0");
						tmp = wtspc(tmp);
						if(tmp == NULL) 
							cerr << "Size float (z) not read, syntax error\n";
						else{
							volumes[subv].zfVSize = volumes[subv].zfSize = (float)atof(tmp);
						}
					} //size float
					
					else if(!strcmp(tmp, "Pos int")){
						tmp = strtok(NULL, ",");
						tmp = wtspc(tmp);
						if(tmp == NULL) 
							cerr << "Pos int (x) not read, syntax error\n";
						else{
							volumes[subv].xiPos = atoi(tmp);
						}
						tmp = strtok(NULL, ",");
						tmp = wtspc(tmp);
						if(tmp == NULL) 
							cerr << "Pos int (y) not read, syntax error\n";
						else{
							volumes[subv].yiPos = atoi(tmp);
						}
						tmp = strtok(NULL, "\n\0");
						tmp = wtspc(tmp);
						if(tmp == NULL) 
							cerr << "Pos int (z) not read, syntax error\n";
						else{
							volumes[subv].ziPos = atoi(tmp);
						}
					} //pos int
					
					else if(!strcmp(tmp, "Pos float")){
						tmp = strtok(NULL, ",");
						tmp = wtspc(tmp);
						if(tmp == NULL) 
							cerr << "Pos int (x) not read, syntax error\n";
						else{
							volumes[subv].xfPos = (float)atof(tmp);
						}
						tmp = strtok(NULL, ",");
						tmp = wtspc(tmp);
						if(tmp == NULL) 
							cerr << "Pos int (y) not read, syntax error\n";
						else{
							volumes[subv].yfPos = (float)atof(tmp);
						}
						tmp = strtok(NULL, "\n\0");
						tmp = wtspc(tmp);
						if(tmp == NULL) 
							cerr << "Pos int (z) not read, syntax error\n";
						else{
							volumes[subv].zfPos = (float)atof(tmp);
						}
					} //pos float
					
					else {
						if(!((str[0] == '#') || (str[0] == '\0') || (str[0] == '\n')))
							cerr << "Error: MetaVolume::parse() : unknown argument : '" 
							<< tmp << "'" << endl;
					}
					
					if(NULL == fgets(str, strlen, f)){	       
						cerr << "Error parsing " << filename 
							<< ": SubVolume{" << endl;
						return 0;
					}
					tmp = str;
					tmp = wtspc(tmp);
					spcwt(tmp);
	 } // while !'}'
	 ++subv;
      }//SubVolume
      
      else {
				if(!((str[0] == '#') || (str[0] == '\0') || (str[0] == '\n')))
					cerr << "Error: MetaVolume::parse() : unknown argument : '" 
					<< tmp << "'" << endl;
      }
			
     }
   }//while !eof
	 
   fclose(f);

   if(npipes){

   }
   
	//if there is anything that needs a full parse, do it here:
	for(int v=0; v<numSubVols; ++v){
		volumes[v].oldData = new unsigned char*[tstepCache];
		volumes[v].oldGrad = new unsigned char*[tstepCache];
		volumes[v].oldTimeStep = new int[tstepCache];
		volumes[v].tstepCache = tstepCache;
		volumes[v].oldTSpos = 0;
		for(int t=0; t<tstepCache; ++t){
			volumes[v].oldData[t] = 0;
			volumes[v].oldTimeStep[t] = -1;
		}
	}

   return 1;
}

//=============================================================== output
//======================================================================

int MetaVolume::output(char *outfile)
{
   FILE *f;
   if(!(f=fopen(outfile, "w"))){
      cerr << "Error :: MetaVolume() : Could not open " 
	   << outfile << " for reading.\n";
      return 1;
   }

   fprintf(f,"Data Set Name:\t\t\t%s\n", dataSetName);
   fprintf(f,"Data Set Files:\t\t\t%s\n", path);
   fprintf(f,"Number of Time Steps:\t\t%d, %d, %d\n", tsteps, tstart, tstop);
   fprintf(f,"TLUT File:\t\t\t%s\n", TLUTfile);
   fprintf(f,"Volume Size int:\t\t%d, %d, %d\n", xiSize, yiSize, ziSize);
   fprintf(f,"Volume Size float:\t\t%f, %f, %f\n\n", xfSize, yfSize, zfSize);
   
   fprintf(f,"Number of Sub Volumes:\t%d\n", numSubVols);

   for(int i=0; i< numSubVols; ++i){
      Volume *v = &volumes[i];
      fprintf(f, "\nSubVolume{\n");
      fprintf(f, "\tSize int:\t%d, %d, %d\n", v->xiSize, v->yiSize, v->ziSize);
      fprintf(f, "\tSize float:\t%f, %f, %f\n", v->xfSize, v->yfSize, v->zfSize);
      fprintf(f, "\tPos int:\t%d, %d, %d\n", v->xiPos, v->yiPos, v->ziPos);
      fprintf(f, "\tPos float:\t%f, %f, %f\n", v->xfPos, v->yfPos, v->zfPos);
      fprintf(f, "}\n");
   }
   
   fprintf(f, "\n\n#file generated automaticly by MetaVolume.cpp\n");
   return 0;
}

//=========================================================== print info
//======================================================================

void MetaVolume::printInfo()
{
   cout << endl;
   cout << "******** Volume Information ********\n";
	 if(dataSetName)
	   cout << "  Data Set Name: " << dataSetName << endl;
	 if(path)
	   cout << "  Path to data : " << path << endl;
	 if(TLUTfile)
		 cout << "  TLUT file    : " << TLUTfile << endl;
	 if(nrrdFile)
		 cout << "  NRRD file    : " << nrrdFile << endl;
	 cout << "  Time Steps   : " << tsteps << endl;
   cout << "       Start   : " << tstart << endl;
   cout << "       Stop    : " << tstop  << endl;
   cout << "  Voxel Dim    : " << xiSize << ", " << yiSize << ", " 
	<< ziSize << endl;
   cout << "  Volume Size  : " << xfSize << ", " << yfSize << ", " 
	<< zfSize << endl; 
	 cout << "  Number of Elements : " << nelts << endl;
   cout << "  Sub Volumes  : " << numSubVols << endl;
   cout << endl;
   for(int i=0 ; i< numSubVols ; ++i){
      Volume *v = &volumes[i];
      cout << "    Sub Volume : " << i << endl;
      cout << "         Voxel Dim    : " << v->xiSize << ", " << v->yiSize 
	   << ", " << v->ziSize << endl;
      cout << "         Volume Size  : " << v->xfSize << ", " << v->yfSize 
	   << ", " << v->zfSize << endl;
      cout << "         Voxel Pos    : " << v->xiPos << ", " << v->yiPos 
	   << ", " << v->ziPos << endl;
      cout << "         Volume Pos   : " << v->xfPos << ", " << v->yfPos 
	   << ", " << v->zfPos << endl;
      cout << endl;
   }
   cout << "************************************\n";
   cout << endl;
}

//============================================================ read Vol
//======================================================================

unsigned int MetaVolume::readVol(int volNum, int timestep)
{

	Volume *volume = &volumes[volNum];
	if(!volume->dbData[0]){ //create the volume if we haven't already

		volume->dbData[0] = new unsigned char[(volume->xiVSize*
				                                      volume->yiVSize*
				                                      volume->ziVSize)];
		//cache this new volume	

	} else {  //start over if we have
		//delete[] volume->dbData[0];
		
		volume->dbData[0] = new unsigned char[(volume->xiVSize*
				                                      volume->yiVSize*
				                                      volume->ziVSize)];
		xiSize = xiVSize;
		yiSize = yiVSize;
		ziSize = ziVSize;
		volume->xiSize = volume->xiVSize;
		volume->yiSize = volume->yiVSize;
		volume->ziSize = volume->ziVSize;
		xfSize = xfVSize;
		yfSize = yfVSize;
		zfSize = zfVSize;
		volume->xfSize = volume->xfVSize;
		volume->yfSize = volume->yfVSize;
		volume->zfSize = volume->zfVSize;
		nelts = nVelts;
	}


	currentTStep = timestep;

	//see if we need to append the subvolume number
	char    subfile[512];
	if(append)  
		sprintf(subfile, "%s.%04d.%02d", path, timestep, volNum);
	else            
		sprintf(subfile, "%s", path);
	cerr << "Reading: " << subfile << endl;
	//open the file, no special direct i/o yet
#if 1//WIN32
	unsigned int size = volume->xiVSize * volume->yiVSize * volume->ziVSize;
	
	ifstream  fin(subfile, ios:: in | ios::binary);
	if (!fin.is_open()){
		cerr << "Error: Reader::readVolume, failed to open " 
			<< subfile << " for reading" << endl;
		return 0;
	}

	//read for a specific data type
	if(dataType == _UCHAR){
		cerr << "--Reading single byte volume (";
		cerr << size << " bytes)" << endl;

		fin.read((char *)volume->dbData[0], size);
		unsigned int n = fin.gcount();  //verify our read
		fin.close(); 
		if(n != size){
			cerr << "Error: Reader::readVolume, UC read failed\n";
			cerr << "  n = " << n << endl;
			return 0; //error
		}
		cerr << subfile << " was succesfully read (" << n << " bytes)" << endl;
		volume->currentData = volume->dbData[0];
		return n;
	}
	else if((dataType == _USHORT) || (dataType == _SHORT)){
		cerr << "--Reading double byte volume (";
		size *= 2;
		cerr << size << " bytes)" << endl;
		volume->nativeData = (void *) new unsigned char[size];
		
		fin.read((char *)volume->nativeData, size);
		unsigned int n = fin.gcount();  //verify our read
		fin.close(); 
		if(n != size){
			cerr << "Error: Reader::readVolume, 2B read failed\n";
			cerr << "  n = " << n << endl;
			return 0; //error
		}
		cerr << subfile << " was succesfully read (" << n << " bytes)" << endl;

		if(dataEndian == _BIG_ENDIAN){
			cerr << "Changing Endian ...";
			_nrrdSwapShortEndian(volume->nativeData, size/2);
			cerr << "Done" << endl;
		}

		if(dataType == _USHORT)
			quantize(volume->dbData[0], volume->xiVSize, volume->yiVSize, volume->ziVSize, (unsigned short *)volume->nativeData);
		else if(dataType == _SHORT)
			quantize(volume->dbData[0], volume->xiVSize, volume->yiVSize, volume->ziVSize, (short *)volume->nativeData);

		volume->currentData = volume->dbData[0];
		return n;
	}
	else if((dataType == _UINT)||(dataType == _INT)||(dataType == _FLOAT)){
		cerr << "--Reading quad byte volume (";
		size *=4;
		cerr << size << " bytes)" << endl;
		volume->nativeData = (void *) new unsigned char[size];

		fin.read((char *)volume->nativeData, size);
		unsigned int n = fin.gcount();  //verify our read
		fin.close(); 
		if(n != size){
			cerr << "Error: Reader::readVolume, 4B read failed\n";
			cerr << "  n = " << n << endl;
			return 0; //error
		}
		cerr << subfile << " was succesfully read (" << n << " bytes)" << endl;

		if(dataEndian == _BIG_ENDIAN){
			cerr << "Changing Endian ...";
			_nrrdSwapWordEndian(volume->nativeData, size/4);
			cerr << "Done" << endl;
		}

		if(dataType == _FLOAT)
			quantize(volume->dbData[0], volume->xiVSize, volume->yiVSize, volume->ziVSize, (float *)volume->nativeData);
		
		volume->currentData = volume->dbData[0];
		return n;
	}
	else if(dataType == _DOUBLE){
		cerr << "--Reading 8 byte volume (";
		size *= 8;
		cerr << size << " bytes)" << endl;
		volume->nativeData = (void *) new unsigned char[size];

		fin.read((char *)volume->dbData[0], size);
		unsigned int n = fin.gcount();  //verify our read
		fin.close(); 
		if(n != size){
			cerr << "Error: Reader::readVolume, 8B read failed\n";
			cerr << "  n = " << n << endl;
			return 0; //error
		}
		cerr << subfile << " was succesfully read (" << n << " bytes)" << endl;
		volume->currentData = volume->dbData[0];
		return n;
	}


#else	//oldschool c style reads dont work in Microsoft land! - this needs to be fixex (xiVSize)
	FILE *f;
	if(!(f = fopen(subfile, "rb"))){
	   cerr << "Error: Reader::readVolume, failed to open " 
		<< subfile << " for reading" << endl;
	   return 0;
	}
	
	cerr << "** Reading " << subfile << " of size " << volume->xiSize << " x " 
		<< volume->yiSize <<  " x " << volume->xiSize << " = " 
	     << volume->xiSize * volume->yiSize * volume->ziSize << endl;
	size_t n;
	//check to see if we have unsigned chars
	if(dataType == MetaVolume::_UCHAR){
	   //read it
	   n = fread((void *)volume->dbData[0], 
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
	   volume->currentData = volume->dbData[0];
	   return n;
	}
#endif
	return 0;
}

//============================================================= read All
//======================================================================

unsigned int MetaVolume::readAll(int timestep)
{
	unsigned int ret = 0;
	for(int i=0; i< numSubVols; ++i){
		ret += readVol(i,timestep);
		if(!ret) return 0;
	}
	return ret;
}

//============================================================ swapTStep
//======================================================================
unsigned int MetaVolume::swapTStep(int timestep)//try to swap a cached timestep (0 = need to read)
{
	for(int i=0; i< numSubVols; ++i){
		int gotone = -1;
		for(int t=0; t<tstepCache; ++t){  //search for the timestep
			if(volumes[i].oldTimeStep[t] == timestep)
				gotone = t;
		}
		if(gotone == -1){
			cerr << "Time step not cached" << endl;
			return 0;
		} else {
			if(!volumes[i].oldData[gotone]){
				cerr << "BANG! should have been cached but it's not!!!" << endl;
				return 0;
			}
			cerr << "Time step cached - swaping data :" << (int)gotone << endl;
			volumes[i].currentData = volumes[i].oldData[gotone];
			volumes[i].currentGrad = volumes[i].oldGrad[gotone];
			volumes[i].dbData[0] = volumes[i].oldData[gotone];
			currentTStep = timestep;
		}
	}
	return 1;
}

//=========================================================== cacheTStep
//======================================================================
unsigned int MetaVolume::cacheTStep()//try to cache the current timestep (0=didn't happen)
{
	if(tstepCache > 0){
	
		for(int i=0; i<numSubVols; ++i){
			int gotone = -1;
			for(int t=0; t<tstepCache; ++t){  //search for the timestep
				cerr << " t= " << t << " oldTimeStepVal = " << volumes[i].oldTimeStep[t] << endl;
				if(volumes[i].oldTimeStep[t] == currentTStep){
					gotone = t;
				}
			}
			if(gotone == -1){
				cerr << "Caching a time step " << volumes[i].oldTSpos << endl;
				volumes[i].oldTimeStep[volumes[i].oldTSpos] = currentTStep;
				volumes[i].oldData[volumes[i].oldTSpos] = volumes[i].currentData;	
				volumes[i].oldGrad[volumes[i].oldTSpos] = volumes[i].currentGrad;	
				volumes[i].oldTSpos = (volumes[i].oldTSpos + 1) % tstepCache;
			} else {
				cerr << "Time step is already cached in " << gotone << endl;
			}
		}
	}
	return 1;
}

//============================================================= writeAll
//======================================================================

unsigned int MetaVolume::writeAll(char *filename)
{
	char    subfile[512];
	sprintf(subfile, "%s.trex", filename);
	FILE *f;
	if(!(f = fopen(subfile, "w"))){
	   cerr << "Error: MetaVolume::writeAll, failed to open " 
			<< subfile << " for writing" << endl;
	   return 0;
	}
	fprintf(f, "#  Meta Volume  #\n");
	fprintf(f, "\n\n#    Global info\n");
  fprintf(f, "Data Set Name:         %s\n", filename);
	fprintf(f, "#  NOTE: Data Set Files uses an implicit extension: filename.timestep.subvol\n");
	fprintf(f, "Data Set Files:        %s\n", filename);
	//change this for time varying
	fprintf(f, "Number of Time Steps:  1, 0, 0\n");
	fprintf(f, "Volume Size int:       %d, %d, %d\n", xiSize, yiSize, ziSize);
	fprintf(f, "Volume Size float:     %f, %f, %f\n", xfSize, yfSize, zfSize);
	fprintf(f, "\n\n#    Subvolume info\n");
	fprintf(f, "Number of Sub Volumes: %d\n", numSubVols);
	
	unsigned int n = 0;

	for(int i =0; i<numSubVols; ++i){
			char subvolfile[512];
			if(append)  	//change this for time varying
				sprintf(subvolfile, "%s.%04d.%02d", filename, 0, i);
			else            
				sprintf(subvolfile, "%s", filename);
			cerr << "writing " << filename << " sub-volume " << i << "  as " << subvolfile << endl;
			n += volumes[i].writeVol(f, subvolfile);
	}

	fclose(f);

	return n;
}


//============================================================ read Nrrd
//======================================================================

unsigned int MetaVolume::readNrrd(int debug)
{
	FILE *f;
	if(!(f = fopen(nrrdFile, "rb"))){
	   cerr << "Error: Reader::readNrrd, failed to open " 
		<< nrrdFile << " for reading" << endl;
	   return 0;
	}
	char line[256];
	if(debug)
		cerr << "Nrrd info:" << endl;

	int ret = 0;
	while(strlen(fgets(line,256,f)) > 1){
		if(debug)
			cerr << line;
		ret += parseNrrd(line);
	}

	if(ret != 0) return -1;

	size_t n;
	//check to see if we have unsigned chars
	if(dataType == MetaVolume::_USHORT){
		cerr << "WARNING: unsigned short quantization only supports scalar data" << endl;
		unsigned short *data = new unsigned short[xiSize*yiSize*ziSize*nelts];
	   n = fread((void *)data, 
		     sizeof(unsigned short), 
		     xiSize * yiSize * ziSize * nelts, f);
	   //sanity check
	   if(n != (unsigned int)(xiSize * yiSize * ziSize * nelts)){
	      cerr << "Error: Reader::readNrrd, UC read failed\n";
	      cerr << "  n = " << n << endl;
	      return 0; //error
	   }
	   //close it to force a flush
	   fclose(f);
	   cerr << "read : " << n << " bytes " << endl;
		 unsigned char *ucdata = new unsigned char[xiSize*yiSize*ziSize*nelts];
		 quantize(ucdata,xiSize, yiSize, ziSize, data);
		 
		 xfSize = xSpc * xiSize;
		 yfSize = ySpc * yiSize;
		 zfSize = zSpc * ziSize;
		 float max = MAX(xfSize, yfSize);
		 max = MAX(max, zfSize);
		 xfSize /=max;
		 yfSize /=max;
		 zfSize /=max;

		 numSubVols = 1;
		 volumes = new Volume[1];
	   volumes[0].currentData = volumes[0].dbData[0] = ucdata;
		 volumes[0].xiSize = xiSize;
		 volumes[0].yiSize = yiSize;
		 volumes[0].ziSize = ziSize;
		 volumes[0].xfSize = xfSize;
		 volumes[0].yfSize = yfSize;
		 volumes[0].zfSize = zfSize;
	}
	if(dataType == MetaVolume::_UCHAR){
	   //read it
		 unsigned char *data = new unsigned char[xiSize*yiSize*ziSize*nelts];
	   n = fread((void *)data, 
		     sizeof(unsigned char), 
		     xiSize * yiSize * ziSize * nelts, f);
	   //sanity check
	   if(n != (unsigned int)(xiSize * yiSize * ziSize * nelts)){
	      cerr << "Error: Reader::readNrrd, UC read failed\n";
	      cerr << "  n = " << n << endl;
	      return 0; //error
	   }
	   //close it to force a flush
	   fclose(f);
	   cerr << "read : " << n << " bytes " << endl;

		 xfSize = xSpc * xiSize;
		 yfSize = ySpc * yiSize;
		 zfSize = zSpc * ziSize;
		 float max = MAX(xfSize, yfSize);
		 max = MAX(max, zfSize);
		 xfSize /=max;
		 yfSize /=max;
		 zfSize /=max;

		 numSubVols = 1;
		 volumes = new Volume[1];
	   volumes[0].currentData = volumes[0].dbData[0] = data;
		 volumes[0].xiSize = xiSize;
		 volumes[0].yiSize = yiSize;
		 volumes[0].ziSize = ziSize;
		 volumes[0].xfSize = xfSize;
		 volumes[0].yfSize = yfSize;
		 volumes[0].zfSize = zfSize;

		 return n;
	}
	
	return 0;
}

//============================================================ Merge MVs
//======================================================================
int MetaVolume::mergeMV(int blurnorms,
												int addG, int addH,            //add grad and hessian??
		                    MetaVolume *mv1,           //combine metavolumes
											  MetaVolume *mv2, 
												MetaVolume *mv3)
{
	int ne = nelts;
	if(addG) ++ne;
	if(addH){
		if(!addG){
			cerr << "ERROR MetaVolume::mergeMV() adding hessian without grad not supported" << endl;
			return 0;
		}
		++ne;
	}
	if(mv1){
		if(numSubVols != mv1->numSubVols){
			cerr << "ERROR MetaVolume::mergeMV() number of Sub Volumes do not match" << endl;
			return 0;
		}
		ne += mv1->nelts;
	}
	if(mv2){
		if(numSubVols != mv2->numSubVols){
			cerr << "ERROR MetaVolume::mergeMV() number of Sub Volumes do not match" << endl;
			return 0;
		}
		ne += mv2->nelts;
	}
	if(mv3){ 
		if(numSubVols != mv3->numSubVols){
			cerr << "ERROR MetaVolume::mergeMV() number of Sub Volumes do not match" << endl;
			return 0;
		}
		ne += mv3->nelts;
	}

	cerr << "number of elements in new volume : " << ne << endl; 

	//copy the data from the metavolumes into new data
	for(int v=0; v<numSubVols; ++v){
		Volume *tv = &volumes[v];
		int sx = tv->xiSize;
		int sy = tv->yiSize;
		int sz = tv->ziSize;
		unsigned char *d0 = tv->currentData;
		unsigned char *d1 = 0;
		unsigned char *d2 = 0;
		unsigned char *d3 = 0;
		if(mv1){
			d1 = mv1->volumes[v].currentData;
		}
		if(mv2){
			d2 = mv2->volumes[v].currentData;
		}
		if(mv3){
			d3 = mv3->volumes[v].currentData;
		}
		unsigned char *d = tv->currentData = new unsigned char[tv->xiSize*tv->yiSize*tv->ziSize*ne];

		// copy data
		for(int i=0; i<sz; ++i){
			for(int j=0; j<sy; ++j){
				for(int k=0; k<sx; ++k){
					int offs = 0;  //current element offset into new data
					for(int e=0; e<nelts; ++e){ //copy our data
						d[i*sx*sy*ne + j*sx*ne + k*ne + e] = d0[i*sx*sy*nelts + j*sx*nelts + k*nelts + e];
					}
					offs += nelts; //end our data
					if(mv1){ //copy mv1's data
						int nelts1 = mv1->nelts;
						for(int e=0; e<mv1->nelts; ++e){
							d[i*sx*sy*ne + j*sx*ne + k*ne + offs + e] = d1[i*sx*sy*nelts1 + j*sx*nelts1 + k*nelts1 + e];
						}
						offs += mv1->nelts;
					} //end mv1
					if(mv2){ //copy mv2's data
						int nelts2 = mv2->nelts;
						for(int e=0; e<mv2->nelts; ++e){
							d[i*sx*sy*ne + j*sx*ne + k*ne + offs + e] = d2[i*sx*sy*nelts2 + j*sx*nelts2 + k*nelts2 + e];
						}
						offs += mv2->nelts;
					} //end mv2
					if(mv3){ //copy mv3's data
						int nelts3 = mv3->nelts;
						for(int e=0; e<mv3->nelts; ++e){
							d[i*sx*sy*ne + j*sx*ne + k*ne + offs + e] = d3[i*sx*sy*nelts3 + j*sx*nelts3 + k*nelts3 + e];
						}
						offs += mv3->nelts;
					} //end mv3
				}//end for x
			}//end for y
		}//end for z
	}//end for v


	float *grad = 0;
	if(addG){
		if(numSubVols != 1){
			cerr << "Warning MetaVolume::mergeMV() un-bricking required, " << endl
				   << "       gradients will be wrong at brick boundaries" << endl;
		}
		for(int v=0; v< numSubVols; ++v){
			Volume *tv = &volumes[v];
			int sx = tv->xiSize;
			int sy = tv->yiSize;
			int sz = tv->ziSize;
			grad = new float[sx*sy*sz*3];
			if(addH){ //gradient magnitude and hessian
				AGradArb<unsigned char>(grad, sx, sy, sz, ne-2, ne, tv->currentData);
				unsigned char *gmag = new unsigned char[sx*sy*sz];
				unsigned char *hess = new unsigned char[sx*sy*sz];
				GMagHess(gmag, hess, sx, sy, sz, grad);
				for(int i=0; i<sz; ++i){
					for(int j=0; j<sy; ++j){
						for(int k=0; k<sx; ++k){
							tv->currentData[i*sx*sy*ne + j*sx*ne + k*ne + ne-2 + 0] =
								  gmag[i*sx*sy + j*sx + k];
							tv->currentData[i*sx*sy*ne + j*sx*ne + k*ne + ne-2 + 1] =
								  hess[i*sx*sy + j*sx + k];
						}
					}
				}
				delete[] gmag;
				delete[] hess;
			}
			else{ //just gradient magnitude
				AGradArb<unsigned char>(grad, sx, sy, sz, ne-1, ne, tv->currentData);
				unsigned char *gmag = new unsigned char[sx*sy*sz];
				GMag(gmag, sx, sy, sz, grad);
				for(int i=0; i<sz; ++i){
					for(int j=0; j<sy; ++j){
						for(int k=0; k<sx; ++k){
							tv->currentData[i*sx*sy*ne + j*sx*ne + k*ne + ne-1 + 0] =
								  gmag[i*sx*sy + j*sx + k];
						}
					}
				}
				delete[] gmag;				
			}
			//now get the normals for shading
		  tv->currentGrad = tv->dbGrad[0] = new unsigned char[sx*sy*sz*3];
			if(blurnorms){
				cerr << "  bluring normals";
				blurV3D(grad,(float)1,(float).3,(float).2,(float).1,sx,sy,sz);
				cerr << " -done" << endl;
			}
			scalebiasN(tv->currentGrad, grad, sx, sy, sz);
			delete[] grad;
		}

	}
	if(numSubVols == 1){
		wholeVol = &volumes[0];
	}

	nelts = ne;

	return 0;
}


//============================================================== normals
//======================================================================

void MetaVolume::normalsVGH(int blurnorms, int only1st)
{
#if 0
	if(nelts == 1){
		for(int v =0; v< numSubVols; ++v){
			int sx = volumes[v].xiSize;
			int sy = volumes[v].yiSize;
			int sz = volumes[v].ziSize;
			if(volumes[v].currentData1){ //extra data
				float *fgrad = new float[sx*sy*sz*3];      //temp float grad
				volumes[v].currentGrad  = new unsigned char[sx*sy*sz*3];
				volumes[v].currentData2 = new unsigned char[sx*sy*sz];
				//compute derivative
				addDer(volumes[v].currentData2,fgrad,sx,sy,sz,volumes[v].currentData,volumes[v].currentData1);	
				scalebiasN(volumes[v].currentGrad, fgrad,sx,sy,sz);          //normalize+scale&bias
				delete[] fgrad;

			} else {  //scalar volume
				volumes[v].currentGrad  = new unsigned char[sx*sy*sz*3];	  //need to get a grad
				volumes[v].currentData1 = new unsigned char[sx*sy*sz];      //need to get a grad mag
				float *fgrad = new float[sx*sy*sz*3];      //temp float grad
				derivative3D(volumes[v].currentData1, fgrad, sx, sy, sz, volumes[v].currentData); //compute derivative
				scalebiasN(volumes[v].currentGrad, fgrad,sx,sy,sz);         //normalize+scale&bias
				delete[] fgrad;                           //done with temp
			}
		}
	} else {
#endif

		if(only1st){ //VGH volume???
			for(int v=0; v< numSubVols; ++v){
				int sx = volumes[v].xiSize;
				int sy = volumes[v].yiSize;
				int sz = volumes[v].ziSize;
				volumes[v].currentGrad = new unsigned char[sx*sy*sz*3];
				float *fgrad = new float[sx*sy*sz*3];      //temp float grad
				derivative3DVGH(fgrad,sx,sy,sz,volumes[v].currentData);
				if(blurnorms){
					cerr << "  bluring normals";
					blurV3D(fgrad,(float)1,(float).3,(float).2,(float).1,sx,sy,sz);
					cerr << " -done" << endl;
				}
				scalebiasN(volumes[v].currentGrad, fgrad,sx,sy,sz);
				delete[] fgrad;
			}
		} else {

		}
	//}

}

//=============================================================== padOut
//======================================================================

void MetaVolume::padOut()
{
	for(int i =0; i<numSubVols; ++i){
		int sx = volumes[i].xiSize; 
		int sy = volumes[i].yiSize; 
		int sz = volumes[i].ziSize;
		if(!isPow2(sx) || !isPow2(sx) || !isPow2(sx)){
			cerr << "warning!!!! dataset is not a power of two" << endl;
			cerr << " padding out..." << endl;

			unsigned char *newdata0 = 0;
			unsigned char *newgrad  = 0;

			pow2Celing(newdata0,
				newgrad,
				volumes[i].currentData, 
				volumes[i].currentGrad,
				sx,sy,sz);
			
			xfSize *= (float)sx/(float)xiSize;
			yfSize *= (float)sy/(float)yiSize;
			zfSize *= (float)sz/(float)ziSize;
			xiSize = sx;
			yiSize = sy;
			ziSize = sz;
			volumes[i].currentData  = volumes[i].dbData[i] = newdata0;
			volumes[i].currentGrad  = volumes[i].dbGrad[i] = newgrad;
			volumes[i].xiSize = xiSize;
			volumes[i].yiSize = yiSize;
			volumes[i].ziSize = ziSize;
			volumes[i].xfSize = xfSize;
			volumes[i].yfSize = yfSize;
			volumes[i].zfSize = zfSize;
		}
	}
}

//================================================================ Brick
//======================================================================

void MetaVolume::brick(int maxsz)
{
	if(numSubVols == 1){
		if((xiSize*yiSize*ziSize) > maxsz){ //we have to rebrick
			int xd = 1;
			int yd = 1;
			int zd = 1;
			int sx = xiSize;
			int sy = yiSize;
			int sz = ziSize;
			while(((sx/(float)xd)*(sy/(float)yd)*(sz/(float)zd)) > maxsz){
				if(zd > yd){
					if(yd > xd)
						xd*=2;
					else
						yd*=2;
				} else
					zd*=2;
			}
			cerr << "NEW brick dims " << sx/(float)xd << " " << sy/(float)yd << " " << sz/(float)zd << endl;
			Volume *tempv = &volumes[0];
			wholeVol = tempv;
			unsigned char *od = tempv->currentData; //old data
			volumes = new Volume[xd*yd*zd];   //create new volumes
			numSubVols = xd*yd*zd;
			int nsx = (int)(sx/(float)xd);    //new sizes
			int nsy = (int)(sy/(float)yd);
			int nsz = (int)(sz/(float)zd);
			cerr << "  Creating sub-volume : ";
			for(int i=0; i<zd; ++i){
				for(int j=0; j<yd; ++j){
					for(int k=0; k<xd; ++k){
						int cv = i*yd*xd + j*xd + k; //current (sub)volume
						cerr << cv << " ";
						volumes[cv].currentData = volumes[cv].dbData[0] = new unsigned char[nsx*nsy*nsz*nelts];
						if(tempv->currentGrad) 
							volumes[cv].currentGrad = volumes[cv].dbGrad[0] = new unsigned char[nsx*nsy*nsz*3];
						volumes[cv].xiSize = nsx;
						volumes[cv].yiSize = nsy;
						volumes[cv].ziSize = nsz;
						volumes[cv].xfSize = tempv->xfSize*(nsx/(float)sx);
						volumes[cv].yfSize = tempv->yfSize*(nsy/(float)sy);
						volumes[cv].zfSize = tempv->zfSize*(nsz/(float)sz);
						volumes[cv].xfPos  = volumes[cv].xfSize*k;
						volumes[cv].yfPos  = volumes[cv].yfSize*j;
						volumes[cv].zfPos  = volumes[cv].zfSize*i;
						volumes[cv].xiPos  = volumes[cv].xiSize*k;
						volumes[cv].yiPos  = volumes[cv].yiSize*j;
						volumes[cv].ziPos  = volumes[cv].ziSize*i;

						for(int z=0; z<nsz; ++z){
							for(int y=0; y<nsy; ++y){
								for(int x=0; x<nsx; ++x){
									for(int e=0; e<nelts; ++e){
										volumes[cv].currentData[z*nsy*nsx*nelts + y*nsx*nelts + x*nelts + e] = 
										  tempv->currentData[(i*nsz + z)*sx*sy*nelts + (j*nsy + y)*sx*nelts + (k*nsx + x)*nelts + e];
									}//elts
								}//x
							}//y
						}//z----------------------

						if(tempv->currentGrad){
							for(int z=0; z<nsz; ++z){
								for(int y=0; y<nsy; ++y){
									for(int x=0; x<nsx; ++x){
										for(int e=0; e<3; ++e){
											volumes[cv].currentGrad[z*nsy*nsx*3 + y*nsx*3 + x*3 + e] = 
												tempv->currentGrad[(i*nsz + z)*sx*sy*3 + (j*nsy + y)*sx*3 + (k*nsx + x)*3 + e];
										}//elts
									}//x
								}//y
							}//z----------------------
						}


					}//xd
				}//yd
			}//zd--------------------
			cerr << endl;
		} //end if we had to rebrick
	} else {
		cerr << "Warning::Re-bricking is needed!!!, auto-re-bricking is not supported...YET" << endl; 
	}
}

void MetaVolume::brick(int bx, int by, int bz)
{
	//this doesn't copy auxilary data
	if(numSubVols == 1){
		int xd = (int)(xiSize/(float)bx);
		int yd = (int)(yiSize/(float)by);
		int zd = (int)(ziSize/(float)bz);
		int sx = xiSize;
		int sy = yiSize;
		int sz = ziSize;
		Volume *tempv = &volumes[0];
		wholeVol = tempv;
		unsigned char *od = tempv->currentData; //old data
		volumes = new Volume[xd*yd*zd];   //create new volumes
		numSubVols = xd*yd*zd;
		int nsx = bx;    //new sizes
		int nsy = by;
		int nsz = bz;
		cerr << "  Creating sub-volume : ";
		for(int i=0; i<zd; ++i){
			for(int j=0; j<yd; ++j){
				for(int k=0; k<xd; ++k){
					int cv = i*yd*xd + j*xd + k; //current (sub)volume
					cerr << cv << " ";
					volumes[cv].currentData = volumes[cv].dbData[0] = new unsigned char[nsx*nsy*nsz*nelts];

					if(tempv->currentGrad) 
						volumes[cv].currentGrad = volumes[cv].dbGrad[0] = new unsigned char[nsx*nsy*nsz*3];
					volumes[cv].xiSize = nsx;
					volumes[cv].yiSize = nsy;
					volumes[cv].ziSize = nsz;
					volumes[cv].xfSize = tempv->xfSize*(nsx/(float)sx);
					volumes[cv].yfSize = tempv->yfSize*(nsy/(float)sy);
					volumes[cv].zfSize = tempv->zfSize*(nsz/(float)sz);
					volumes[cv].xfPos  = volumes[cv].xfSize*k;
					volumes[cv].yfPos  = volumes[cv].yfSize*j;
					volumes[cv].zfPos  = volumes[cv].zfSize*i;
					volumes[cv].xiPos  = volumes[cv].xiSize*k;
					volumes[cv].yiPos  = volumes[cv].yiSize*j;
					volumes[cv].ziPos  = volumes[cv].ziSize*i;
					
					for(int z=0; z<nsz; ++z){
						for(int y=0; y<nsy; ++y){
							for(int x=0; x<nsx; ++x){
								for(int e=0; e<nelts; ++e){
									volumes[cv].currentData[z*nsy*nsx*nelts + y*nsx*nelts + x*nelts + e] = 
										tempv->currentData[(i*nsz + z)*sx*sy*nelts + (j*nsy + y)*sx*nelts + (k*nsx + x)*nelts + e];
								}//elts
							}//x
						}//y
					}//z----------------------

				} //xd
			}//yd
		}//zd
	} else {
		cerr << "Warning::Re-bricking is needed!!!, auto-re-bricking is not supported...YET" << endl; 	
	}

}

//=========================================================== parse NRRD
//======================================================================

int MetaVolume::parseNrrd(char *line)
{
	char *tmp;
	tmp = strtok(line, ":");
	if(!strcmp(tmp, "dimension")){
		tmp = strtok(NULL, "\n");
		dims = atoi(tmp);
	}
	else if(!strcmp(tmp, "sizes")){
		tmp = strtok(NULL, "\n");
		if(dims == 3){
			sscanf(tmp,"%d %d %d", &xiSize, &yiSize, &ziSize);
		}
		else if(dims == 4){
			sscanf(tmp, "%d %d %d %d", &nelts, &xiSize, &yiSize, &ziSize);
		} else {
			cerr << "Error parsing nrrd file: incorrect dimension: " << dims << endl;
		  return 1;
		}
	}
	else if(!strcmp(tmp, "spacings")){
		tmp = strtok(NULL, "\n");
		if(dims == 3){
			sscanf(tmp,"%f %f %f", &xSpc, &ySpc, &zSpc);
		}
		else if(dims == 4){
			tmp = strtok(tmp, " ");
			tmp = strtok(NULL, "\n");
			sscanf(tmp, "%f %f %f", &xSpc, &ySpc, &zSpc);
		} else {
			cerr << "Error parsing nrrd file: incorrect dimension: " << dims << endl;
			return 1;
		}
	}
	else if(!strcmp(tmp, "type")){
		tmp = strtok(NULL, "\n");
		if(!strcmp(tmp, "unsigned char") || !strcmp(tmp, " unsigned char")){
			dataType = MetaVolume::_UCHAR;
		}
		else if(!strcmp(tmp, "unsigned short") || !strcmp(tmp, " unsigned short")){
			dataType = MetaVolume::_USHORT;
		}
		else{
			cerr << "Error parsing nrrd file: only unsigned char/short data type supported" << endl;
			return 1;
		}
	}
	return 0;
}

//======================================================== is Power of 2
//======================================================================


int MetaVolume::isPow2(int sz)
{
	double tmp = sz/2.0;
	while(tmp > 1.0){
		tmp = tmp/2.0;
	}
	if(tmp != 1.0) return 0;
	else return 1;
}

int MetaVolume::getPow2(int sz)
{
	double tmp = sz/2.0;
	int i = 2;
	while(tmp > 1.0){
		i *= 2;
		tmp = tmp/2.0;
	}
	return i;
}

//==================================================== make pow 2 celing
//======================================================================

void MetaVolume::pow2Celing(unsigned char *&data0,   //there must be a better way!!!!
														unsigned char *&grad,
														const unsigned char *d0,
														const unsigned char *g,
														int &sx, int &sy, int &sz)
{
	int nx = getPow2(sx);
	int ny = getPow2(sy);
	int nz = getPow2(sz);
	cerr << "new sizes = " << nx << " " << ny << " " << nz <<endl;
	cerr << "old sizes = " << sx << " " << sy << " " << sz << endl;
	
	data0 = new unsigned char[nx*ny*nz*nelts];
	if(g)  grad  = new unsigned char[nx*ny*nz*3];

	for(int i=0; i<nz; ++i){
		for(int j=0; j<ny; ++j)
			for(int k=0; k<nx; ++k)
				for(int l=0; l<nelts; ++l)
					data0[i*ny*nx*nelts + j*nx*nelts + k*nelts + l] = 0;
	}

	if(g){
		for(int i=0; i<nz; ++i)
			for(int j=0; j<ny; ++j)
				for(int k=0; k<nx; ++k)
					for(int l=0; l<3; ++l)
						grad[i*ny*nx*3 + j*nx*3 + k*3 + l] = 0;
	}

	for(i=0; i<sz; ++i){
		for(int j=0; j<sy; ++j)
			for(int k=0; k<sx; ++k)
				for(int l=0; l<nelts; ++l)
					data0[i*ny*nx*nelts + j*nx*nelts + k*nelts + l] = 
						d0[i*sy*sx*nelts + j*sx*nelts + k*nelts + l];
	}

	if(g){
		for(i=0; i<sz; ++i)
			for(int j=0; j<sy; ++j)
				for(int k=0; k<sx; ++k)
					for(int l=0; l<3; ++l)
						grad[i*ny*nx*3 + j*nx*3 + k*3 + l] = 
							g[i*sy*sx*3 + j*sx*3 + k*3 + l];	
	}


	sx = nx;
	sy = ny;
	sz = nz;

}

int MetaVolume::hist2D(unsigned char *hist)
{
	float *ih = new float[256*256];
	
	for(int i=0; i<256*256; ++i){
		ih[i] = 0;
	}

	if((nelts >= 2) && volumes[0].currentData){
		for(int v=0; v< numSubVols; ++v){
			for(int i=0; i<volumes[v].ziSize; ++i){
				for(int j=0; j<volumes[v].yiSize; ++j){
					for(int k=0; k<volumes[v].xiSize; ++k){
						unsigned char *d = volumes[v].currentData;
						int vox = i*volumes[v].xiSize*volumes[v].yiSize*nelts + j*volumes[v].xiSize*nelts + k*nelts;
						ih[d[vox] + d[vox+1]*256] += 1;
					}
				}
			}
		}
	} 

	else {
		cerr << "MetaVolume::hist2D, sorry this type of histogram is not implemented" << endl;
		delete[] ih;
		return 0;
	}

	float max = 0;
	for(i = 0; i< 256*256; ++i){
		ih[i] = (float)log(ih[i]);
		max = MAX(ih[i], max);
	}
	for(i=0; i< 256*256; ++i){
		hist[i] = (unsigned char)(ih[i]/(float)max*255);
	}
	delete[] ih;
	return 1;
}

//-- remove leading & trailing white space -----------------------

char *MetaVolume::wtspc(char *str)
{
  if (str == NULL)
    return NULL;
  
  // first trailing (walk to the end of the string)
  int i = 0;
  while(str[i] != '\0')  // Why not just use strlen... 
    ++i;
  
  --i;
  
  while((str[i] == ' ') || (str[i] == '\t') || (str[i] == '\n')){
    str[i] = '\0';
    --i;
    if (i < 0)
      break;
  }

  // then leading
  i = 0;
  while((str[i] == ' ') || (str[i] == '\t'))
    ++i;
  return &str[i];
}

//-- remove trailing white spc ----------------------------------

void MetaVolume::spcwt(char *str)
{
  if(str == NULL)
    return;
}



