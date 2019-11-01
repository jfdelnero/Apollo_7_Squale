///////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//
//-----------H----H--X----X-----CCCCC----22222----0000-----0000------11----------//
//----------H----H----X-X-----C--------------2---0----0---0----0--1--1-----------//
//---------HHHHHH-----X------C----------22222---0----0---0----0-----1------------//
//--------H----H----X--X----C----------2-------0----0---0----0-----1-------------//
//-------H----H---X-----X---CCCCC-----222222----0000-----0000----1111------------//
//-------------------------------------------------------------------------------//
//----------------------------------------------------- http://hxc2001.free.fr --//
///////////////////////////////////////////////////////////////////////////////////
// File : rawym_to_rleym.c
//       Convert and pack ym files.
//
// Written by: Jean François DEL NERO
//
///////////////////////////////////////////////////////////////////////////////////

#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hxcym.h"


ymcontext modloaded;
unsigned char * modfile;

int loadym(char * file)
{
	FILE * f;
	int filesize;

	printf("Loading %s ...\n",file);

	f = fopen(file,"rb");
	if(f)
	{
		fseek(f,0,SEEK_END);
		filesize = ftell(f);
		fseek(f,0,SEEK_SET);
		if(filesize && filesize < 32*1024*1024)
		{
			modfile = malloc(filesize);
			if(modfile)
			{
				memset(modfile,0,filesize);
				fread(modfile,filesize,1,f);

				hxcym_load(&modloaded,(void*)modfile,filesize);
			}
		}

		fclose(f);
	}
	else
	{
		printf("Can't open %s !\n",file);
	}

	return 0;
}


int main(int argc, char *argv[])
{
	printf("YM to RLE\n");

	if(argc > 1)
	{
		loadym(argv[1]);
	}
}

