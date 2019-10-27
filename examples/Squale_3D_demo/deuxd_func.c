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
// <<The Squale 3D Demo>> A demo for the French Apollo Squale computer.
//
// Written by: Jean François DEL NERO
//
// You are free to do what you want with this code.
// A credit is always appreciated if you include it into your prod :)
//
// Change History (most recent first):
///////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <squale_hw.h>

#include "deuxd_func.h"

extern volatile uint8_t buffernb;


const uint8_t div10tab[]=
{
	0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,
	3,3,3,3,3,3,3,3,3,3,
	4,4,4,4,4,4,4,4,4,4,
	5,5,5,5,5,5,5,5,5,5,
	6,6,6,6,6,6,6,6,6,6,
	7,7,7,7,7,7,7,7,7,7,
	8,8,8,8,8,8,8,8,8,8,
	9,9,9,9,9,9,9,9,9,9,
	10,10,10,10,10,10,10,10,10,10,
	11,11,11,11,11,11,11,11,11,11,
	12,12,12,12,12,12,12,12,12,12,
	13,13,13,13,13,13,13,13,13,13,
	14,14,14,14,14,14,14,14,14,14,
	15,15,15,15,15,15,15,15,15,15,
	16,16,16,16,16,16,16,16,16,16,
	17,17,17,17,17,17,17,17,17,17,
	18,18,18,18,18,18,18,18,18,18,
	19,19,19,19,19,19,19,19,19,19,
	20,20,20,20,20,20,20,20,20,20,
	21,21,21,21,21,21,21,21,21,21,
	22,22,22,22,22,22,22,22,22,22,
	23,23,23,23,23,23,23,23,23,23,
	24,24,24,24,24,24,24,24,24,24,
	25,25,25,25,25,25,25,25,25,25
};

const uint8_t multentab[]=
{
	0,10,20,30,40,50,60,70,80,90,100,110,120
};


void setpixel(uint8_t x,uint8_t y, uint8_t state)
{
	uint8_t i,j,c,k,d;

	if(x<80 && y<100)
	{
		i=div10tab[y];//y/10;
		k=y-multentab[i];
	}
}


void setpixelFast(uint8_t x,uint8_t y)
{

}


dot2d points2d[3];

void Ligne(dot2d * pointA,dot2d * pointB,uint8_t state)
{
	LigneFast(pointA,pointB);
}

void LigneFast(dot2d * pointA,dot2d * pointB)
{
	uint8_t x1,x2,y1,y2;
	unsigned char cmd;
	volatile unsigned char * ptr;

	x1=  pointA->x;
	x2=  pointB->x;
	y1=  pointA->y;
	y2=  pointB->y;

	ptr = (volatile unsigned char *)0xF000;

	ptr[0x9] = x1;
	ptr[0xB] = y1;
	cmd = 0x11;

	if( x1 > x2 )
	{
		cmd = cmd | 0x02;
		ptr[0x5] = x1 - x2;
	}
	else
	{
		ptr[0x5] = x2 - x1;
	}

	if( y1 > y2 )
	{
		cmd = cmd | 0x04;
		ptr[0x7] = y1 - y2;
	}
	else
	{
		ptr[0x7] = y2 - y1;
	}

	*ptr = cmd;

}

void cercle(int16_t rayon,int16_t x_centre,int16_t y_centre,uint8_t state)
{
	int16_t x,y,d;

	x=0;
	y=rayon;
	d=rayon;
	while(y>=x)
	{
		setpixel(x+x_centre , y+y_centre,state);
		setpixel(y+x_centre , x+y_centre,state);
		setpixel(-x+x_centre , y+y_centre,state);
		setpixel(-y+x_centre , x+y_centre,state);
		setpixel(x+x_centre , -y+y_centre,state);
		setpixel(y+x_centre , -x+y_centre,state);
		setpixel(-x+x_centre , -y+y_centre,state);
		setpixel(-y+x_centre , -x+y_centre,state);

		if(d >= 2*x)
		{
			d=d-2*x-1;
			x++;
		}
		else
		{
			if( d < 2*(rayon-y) )
			{
				d=d+2*y-1;
				y--;

			}
			else
			{
				d=d+2*(y-x-1);
				y--;
				x++;
			}
		}
	}
	return;
}

extern unsigned char ledclavier;

void display_vectsprite(unsigned char * vectorized_sprite, unsigned char x, unsigned char y)
{
	unsigned char i,j;

	unsigned char nb_lines;
	unsigned char nb_vects;
	unsigned char cnt,color;

	// [NUMBER_OF_LINES] [NUMBER_OF_VECTORS] [ _CNT|_COL] [ _CNT|_COL] [ _CNT|_COL] [ COUNT__ (if CNT==0)] ... [NUMBER_OF_VECTORS]....

	nb_lines = *vectorized_sprite++;

	waitvideochip();

	WR_BYTE(HW_EF9365 + 0x7, 0x00);

	for(i=0;i<nb_lines;i++)
	{
		waitvideochip();

		WR_BYTE(HW_EF9365 + 0x8,0);
		WR_BYTE(HW_EF9365 + 0x9,x);

		WR_BYTE(HW_EF9365 + 0xA,0);
		WR_BYTE(HW_EF9365 + 0xB,(255 - (y + i)));

		nb_vects = *vectorized_sprite++;
		for(j=0;j<nb_vects;j++)
		{
			waitvideochip();

			color = (*vectorized_sprite & 0xF);
			cnt = (*vectorized_sprite++ >> 4);
			if(!cnt)
				cnt = (*vectorized_sprite++);

			WR_BYTE(HW_EF9365 + 0x5, cnt);

			WR_BYTE( HW_CTLHRD_REG, color | ledclavier);

			WR_BYTE(HW_EF9365 + 0x0, 0x11);
		}
	}
}
