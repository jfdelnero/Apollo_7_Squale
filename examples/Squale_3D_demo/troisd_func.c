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
//
// Apollo Squale Technical demo
// A demo for the French Apollo Squale computer.
//
// (c) 2019 HxC2001 / Jean-François DEL NERO
// (c) 2019 MO5.COM / Association MO5.COM
//
// Written by: Jean François DEL NERO
//
// You are free to do what you want with this code.
// But a credit is always appreciated :)
//
// Change History (most recent first):
///////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <squale_hw.h>

#include "deuxd_func.h"
#include "troisd_func.h"
#include "cossintab.h"

lines_buffer double_lines_buffer[2];

#define ZOFF 95
#define XOFF 70
#define YOFF 70

void rotateX(dot * point,uint8_t xang)
{
// Rotate around x-axis
// YT = Y * COS(xang) - Z * SIN(xang) / 256
// ZT = Y * SIN(xang) + Z * COS(xang) / 256
// Y = YT
// Z = ZT
	int16_t yt;
	int16_t zt;
	int8_t cosv;
	int8_t sinv;

	cosv = SinCos[xang];
	xang += 64;
	sinv = SinCos[xang];

	yt = ( ( point->y * cosv ) - ( point->z * sinv ) ) / 128;
	zt = ( ( point->y * sinv ) + ( point->z * cosv ) ) / 128;

	point->y = yt;
	point->z = zt;
}

void rotateY(dot * point,uint8_t yang)
{
// Rotate around y-axis
// XT = X * COS(yang) - Z * SIN(yang) / 256
// ZT = X * SIN(yang) + Z * COS(yang) / 256
// X = XT
// Z = ZT
	int16_t xt;
	int16_t zt;
	int8_t cosv;
	int8_t sinv;

	cosv = SinCos[yang];
	yang += 64;
	sinv = SinCos[yang];

	xt = ( ( point->x * cosv ) - ( point->z * sinv ) ) / 128;
	zt = ( ( point->x * sinv ) + ( point->z * cosv ) ) / 128;

	point->x = xt;
	point->z = zt;
}

void rotateZ(dot * point,uint8_t zang)
{
// Rotate around z-axis
// XT = X * COS(zang) - Y * SIN(zang) / 256
// YT = X * SIN(zang) + Y * COS(zang) / 256
// X = XT
// Y = YT
	int16_t xt;
	int16_t yt;
	int8_t cosv;
	int8_t sinv;

	cosv = SinCos[zang];
	zang += 64;
	sinv = SinCos[zang];

	xt = ( ( point->x * cosv ) - ( point->y * sinv ) ) / 128;
	yt = ( ( point->x * sinv ) + ( point->y * cosv ) ) / 128;

	point->x = xt;
	point->y = yt;
}

void calcpolygone(lines_buffer * ln_buffer, const int8_t * polygone,uint8_t xrotate,uint8_t yrotate,uint8_t zrotate)
{
	uint8_t i,c;
	uint8_t * lines_ptr;
	int8_t * polygone_ptr;

	dot temppoint[3];
	dot2d points2d[3];

	lines_ptr = &ln_buffer->lines[ln_buffer->nblines*4];

	polygone_ptr = (int8_t *)polygone;
	for(i=0;i<3;i++)
	{
		c=0;
		switch(i)
		{
			case 0:
				if( polygone[3] || polygone[11])
				{
					c=1;
				}
			break;

			case 1:
				if( polygone[3] || polygone[7])
				{
					c=1;
				}
			break;

			case 2:
				if( polygone[7] || polygone[11])
				{
					c=1;
				}
			break;
		}

		if(c)
		{
			temppoint[i].x = *polygone_ptr++;
			temppoint[i].y = *polygone_ptr++;
			temppoint[i].z = *polygone_ptr++;

			//rotat
			if(xrotate)
				rotateX(&temppoint[i],xrotate + 64);
			if(yrotate)
				rotateY(&temppoint[i],yrotate + 64);
			if(zrotate)
				rotateZ(&temppoint[i],zrotate + 64);

			points2d[i].x= ((((temppoint[i].x<<6) / (temppoint[i].z+ZOFF)))) + ((256/2) + XOFF) ;
			points2d[i].y= ((((temppoint[i].y<<6) / (temppoint[i].z+ZOFF)))) + ((256/2) + YOFF);

			polygone_ptr++;
		}
		else
			polygone_ptr +=4;
	}

	if(polygone[3])
	{
		*lines_ptr++ = points2d[0].x;
		*lines_ptr++ = points2d[0].y;
		*lines_ptr++ = points2d[1].x;
		*lines_ptr++ = points2d[1].y;

		ln_buffer->nblines++;
	}

	if(polygone[7])
	{
		*lines_ptr++ = points2d[1].x;
		*lines_ptr++ = points2d[1].y;
		*lines_ptr++ = points2d[2].x;
		*lines_ptr++ = points2d[2].y;

		ln_buffer->nblines++;
	}

	if(polygone[11])
	{
		*lines_ptr++ = points2d[0].x;
		*lines_ptr++ = points2d[0].y;
		*lines_ptr++ = points2d[2].x;
		*lines_ptr++ = points2d[2].y;

		ln_buffer->nblines++;
	}
}

void calcobject(lines_buffer * ln_buffer, const d3dtype * obj,uint8_t xrotate,uint8_t yrotate,uint8_t zrotate)
{
	uint8_t i;

	ln_buffer->nblines = 0;

	for(i=0;i<obj->nbfaces;i++)
	{
		calcpolygone(ln_buffer,&obj->vertex[i<<4],xrotate,yrotate,zrotate);
	}
}

void drawobject(lines_buffer * ln_buffer)
{
	uint8_t i;
	unsigned char cmd;
	uint8_t * lines_ptr;
	uint8_t xlen,ylen;
	lines_ptr = &ln_buffer->lines[0];

	WR_BYTE(HW_EF9365 + 0x8,0);
	WR_BYTE(HW_EF9365 + 0xA,0);

	for(i=0; i<ln_buffer->nblines; i++)
	{
		if( lines_ptr[0] > lines_ptr[2] )
		{
			cmd = 0x13;
			xlen = lines_ptr[0] - lines_ptr[2];
		}
		else
		{
			cmd = 0x11;
			xlen = lines_ptr[2] - lines_ptr[0];
		}

		if( lines_ptr[1] > lines_ptr[3] ) // y1 > y2 ?
		{
			cmd = cmd | 0x04;
			ylen = lines_ptr[1] - lines_ptr[3];
		}
		else
		{
			ylen = lines_ptr[3] - lines_ptr[1];
		}

		WAIT_EF9365_READY();

		WR_BYTE(HW_EF9365 + 0x9,lines_ptr[0]); // x start
		WR_BYTE(HW_EF9365 + 0xB,lines_ptr[1]); // y start
		WR_BYTE(HW_EF9365 + 0x5,xlen);
		WR_BYTE(HW_EF9365 + 0x7,ylen);
		WR_BYTE(HW_EF9365 + 0x0,cmd);

		lines_ptr+=4;

	}

	// Wait to finish the last line...
	WAIT_EF9365_READY();
}
