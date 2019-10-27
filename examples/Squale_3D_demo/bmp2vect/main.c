/*
//
// Copyright (C) 2017-2019 Jean-François DEL NERO / HxC2001
//
// This file is part of the Squale-bmp2vect software
//
// Squale-bmp2vect may be used and distributed without restriction provided
// that this copyright statement is not removed from the file and that any
// derivative work contains the original copyright notice and the associated
// disclaimer.
//
// Squale-bmp2vect is free software; you can redistribute it
// and/or modify  it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// Squale-bmp2vect is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Squale-bmp2vect; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>

#include "bmp_file.h"

// default settings

#define MAX_PARAM_ARG_SIZE 512

void generate_source_files(char * outfile, unsigned char * buf, int size)
{
	int i;
	FILE * f;
	char filename[MAX_PARAM_ARG_SIZE];

	f = 0;

	if(strlen(outfile))
	{
		// Create header file
		strcpy(filename,outfile);
		strcat(filename,".h");
		f = fopen(filename , "w");
		if(f)
		{
			fprintf(f,"/////////////////////////////////\n");
			fprintf(f,"//  Vector generated BMP data  //\n");
			fprintf(f,"/////////////////////////////////\n");
			fprintf(f,"\n");
			fprintf(f,"extern const unsigned char bmp_data_%s[];\n",outfile);
			fclose(f);
		}

		strcpy(filename,outfile);
		strcat(filename,".c");

		f = fopen(filename , "w");
	}

	if(!f)
		f = stdout;

	fprintf(f,"/////////////////////////////////\n");
	fprintf(f,"//  Vector generated BMP data  //\n");
	fprintf(f,"/////////////////////////////////\n");
	fprintf(f,"\n");

	fprintf(f,"#include \x22%s.h\x22\n",outfile);

	fprintf(f,"\n");

	fprintf(f,"const unsigned char bmp_data_%s[] =\n",outfile);

	for(i=0;i<size;i++)
	{
		if(!(i&0xF))
		{
			if(i)
			{
				fprintf(f,"\n\t");
			}
			else
			{
				fprintf(f,"\x7B\n\t");
			}
		}

		fprintf(f,"0x%.2X",buf[i]);

		if( i < size - 1 )
		{
			fprintf(f,",");
		}
		else
		{
			fprintf(f,"\n\x7D\x3B\n");
		}
	}

	if( f != stdout )
	{
		fclose(f);
	}
}

unsigned long getpixstate(bitmap_data * bdata, int xpos, int ypos)
{
	if( (xpos < bdata->xsize) && (ypos < bdata->ysize) )
	{
		return bdata->data[( bdata->xsize * ypos ) + xpos];
	}
	else
	{
		printf("getpixstate : outside bmp ! : %d x %d\n", xpos, ypos );
	}
	return 0;
}

int isOption(int argc, char* argv[],char * paramtosearch,char * argtoparam)
{
	int param=1;
	int i,j;

	char option[512];

	memset(option,0,sizeof(option));
	while(param<=argc)
	{
		if(argv[param])
		{
			if(argv[param][0]=='-')
			{
				memset(option,0,sizeof(option));

				j=0;
				i=1;
				while( argv[param][i] && argv[param][i]!=':')
				{
					option[j]=argv[param][i];
					i++;
					j++;
				}

				if( !strcmp(option,paramtosearch) )
				{
					if(argtoparam)
					{
						if(argv[param][i]==':')
						{
							i++;
							j=0;
							while( argv[param][i] && j < ( MAX_PARAM_ARG_SIZE - 1 ) )
							{
								argtoparam[j]=argv[param][i];
								i++;
								j++;
							}
							argtoparam[j]=0;
							return 1;
						}
						else
						{
							return -1;
						}
					}
					else
					{
						return 1;
					}
				}
			}
		}
		param++;
	}

	return 0;
}

void printhelp(char* argv[])
{
	printf("Options:\n");
	printf("  -help \t\t\t\t: This help\n");
	printf("  -file:[filename]\t\t\t: Input bmp file\n");
	printf("  -fontname:[name]\t\t\t: Output font name\n");
	printf("\n");
}

unsigned long col2index[16]=
{
	0xFFFFFF,
	0x00FFFF,
	0xFF00FF,
	0x0000FF,
	0xFFFF00,
	0x00FF00,
	0xFF0000,
	0x000000,
	0x7F7F7F,
	0x007F7F,
	0x7F007F,
	0x00007F,
	0x7F7F00,
	0x007F00,
	0x7F0000,
	0x000000
};

unsigned char getcolorindex(unsigned long rgb)
{
	int i;

	for(i=0;i<16;i++)
	{
		if(col2index[i] == rgb )
		{
			return i;
		}
	}

	return 0;
}

unsigned char * pack_bmp( bitmap_data * bdata, int * size )
{
	int x,y,cntentries;
	unsigned char lastcolor,newcolor;
	unsigned int rlecnt;
	unsigned char * outbuffer;
	unsigned char * start_ptr;
	unsigned char * cnt_ptr;

	outbuffer = malloc( bdata->xsize * bdata->ysize );

	start_ptr = outbuffer;

	*outbuffer++ = bdata->ysize - 1;

	for(y = 0; y < bdata->ysize ;y++)
	{
		rlecnt = 0;
		lastcolor = getcolorindex(getpixstate( bdata, 0, y));

		cnt_ptr = outbuffer;

		*outbuffer++ = 0x00;
		cntentries = 0;

		for(x = 0; x < bdata->xsize ;x++)
		{
			newcolor = getcolorindex(getpixstate( bdata, x, y));

			if( newcolor == lastcolor )
			{
				rlecnt++;
				if( rlecnt > 255 )
				{
					*outbuffer++ = 	( (0x00 << 4) | (lastcolor & 0xF) );
					*outbuffer++ = 255;
					rlecnt -= 255;
					cntentries++;
				}
			}
			else
			{
				if(rlecnt <= 15)
				{
					*outbuffer++ = 	( (rlecnt << 4) | (lastcolor & 0xF) );
					cntentries++;
				}
				else
				{
					*outbuffer++ = 	( (0x00 << 4) | (lastcolor & 0xF) );
					*outbuffer++ = rlecnt;
					cntentries++;
				}

				rlecnt = 1;
			}

			lastcolor = newcolor;
		}

		if(rlecnt)
		{
			if(rlecnt <= 15)
			{
				*outbuffer++ = 	( (rlecnt << 4) | (lastcolor & 0xF) );
				cntentries++;
			}
			else
			{
				*outbuffer++ = 	( (0x00 << 4) | (lastcolor & 0xF) );
				*outbuffer++ = rlecnt;
				cntentries++;
			}
		}

		*cnt_ptr = cntentries;
	}

	*size = outbuffer - start_ptr;

	return start_ptr;
}

int main(int argc, char *argv[])
{
	int bufsize;
	bitmap_data bdata;
	unsigned char * packedimage;
	char filename[MAX_PARAM_ARG_SIZE];
	char fontname[MAX_PARAM_ARG_SIZE];

	printf("Squale bmp2vect v0.1\n");
	printf("(c) Jean-François DEL NERO / HxC2001 2019\n\n");

	// help option...
	if(isOption(argc,argv,"help",0)>0)
	{
		printhelp(argv);
	}

	memset(filename,0,sizeof(filename));
	if(isOption(argc,argv,"file",(char*)&filename)>0)
	{
		printf("Input file : %s\n",filename);
	}

	memset(fontname,0,sizeof(fontname));
	if(isOption(argc,argv,"imagename",(char*)&fontname)>0)
	{
		printf("Image name : %s\n",fontname);
	}

	if(strlen(filename))
	{
		if(!bmp_load(filename,&bdata))
		{
			printf("\nBmp Loaded... Xsize: %d, Ysize: %d\n",bdata.xsize,bdata.ysize);

			packedimage = pack_bmp( &bdata, &bufsize );

			// Get the sprite size.
			generate_source_files(fontname, (unsigned char*)packedimage, bufsize);

			free(packedimage);
		}
	}
	else
	{
		printhelp(argv);
	}
}
