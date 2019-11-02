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

#include "demo.h"

#include "asm_funcs.h"

#include "deuxd_func.h"

#include "images/footpage.h"

extern volatile unsigned char blockpos;
extern volatile unsigned char ymptrbuf[14];

uint8_t jump_color[]={4,5,1,0,2,3,5,6,8,9,10,11,12,13,14,15};

void demo_jump_part(char * string)
{
	#define NB_MAX_POINTS 16
	uint8_t points[4 * NB_MAX_POINTS]; // x pos, y pos, x speed/way, y speed/way
	uint8_t *tmp_ptr;
	uint8_t nbpoints,size_code,stringlen;

	int16_t i;
	uint8_t p;

	uint8_t charsizexpix,charsizeypix;

	stringlen = 0;
	while(string[stringlen])
		stringlen++;

	switch(stringlen)
	{
		case 1:
			size_code = 0x77;
			charsizexpix = 5*7;
			charsizeypix = 8*7;
		break;
		case 2:
			size_code = 0x44;
			charsizexpix = 5*4;
			charsizeypix = 8*4;
		break;
		case 3:
			size_code = 0x44;
			charsizexpix = 5*4;
			charsizeypix = 8*4;
		break;
		case 4:
			size_code = 0x33;
			charsizexpix = 5*3;
			charsizeypix = 8*3;
		break;
		case 5:
		case 6:
			size_code = 0x22;
			charsizexpix = 5*2;
			charsizeypix = 8*2;
		break;
		default:
			size_code = 0x11;
			charsizexpix = 5;
			charsizeypix = 8;
		break;
	}

	WAIT_EF9365_READY();

	WR_BYTE( HW_EF9365 + 0x8, 0 );
	WR_BYTE( HW_EF9365 + 0xA, 0 );

	WR_BYTE( HW_CTLHRD_REG, 7 | ledclavier);
	WR_BYTE( HW_EF9365 + 0x0, 0x0C ); // Clear screen

	WAIT_EF9365_READY();

	display_vectsprite((unsigned char *) &bmp_data_footpage, 0, SCREEN_YSIZE-33 );

	WR_BYTE( HW_CTLHRD_REG, 3 | ledclavier);

	Box(BOX_CORNER_POINT1_X, BOX_CORNER_POINT1_Y, BOX_CORNER_POINT3_X, BOX_CORNER_POINT3_Y);

	nbpoints = 1;

	tmp_ptr = (uint8_t *)&points;

	for(p=0;p<(sizeof(points)/4);p++)
	{
		tmp_ptr[0] = (SCREEN_XSIZE/2);
		tmp_ptr[1] = BOX_CORNER_POINT1_Y + ((BOX_CORNER_POINT3_Y - BOX_CORNER_POINT1_Y)/2) +40;
		tmp_ptr[2] = 3;
		tmp_ptr[3] = 1;
		tmp_ptr += 4;
	}

	i = 0;
	do
	{
		vblank();

		tmp_ptr = (uint8_t *)&points;

		WAIT_EF9365_READY();

		WR_BYTE( HW_CTLHRD_REG, (7) | ledclavier);
		WR_BYTE( HW_EF9365 + 0x3, size_code );

		for(p=0;p<nbpoints;p++)
		{
			WAIT_EF9365_READY();

			WR_BYTE( HW_EF9365 + 0x8, 0 );
			WR_BYTE( HW_EF9365 + 0x9, tmp_ptr[0] );
			WR_BYTE( HW_EF9365 + 0xA, 0 );
			WR_BYTE( HW_EF9365 + 0xB, tmp_ptr[1] );
			WR_BYTE( HW_EF9365 + 0x0, string[p] );

			tmp_ptr += 4;
		}

		tmp_ptr = (uint8_t *)&points;
		for(p=0;p<nbpoints;p++)
		{
			if( ((int8_t)tmp_ptr[2]) < 0 && tmp_ptr[0] < -((int8_t)tmp_ptr[2]))
				*((int8_t*)&tmp_ptr[2]) = -((int8_t)tmp_ptr[2]);

			tmp_ptr[0] += tmp_ptr[2];
			tmp_ptr[1] += ((int8_t)tmp_ptr[3]);

			// X right border
			if( (tmp_ptr[0] >=  (BOX_CORNER_POINT3_X - (charsizexpix))) && (((int8_t)tmp_ptr[2]) >= 0 ) )
			{
				tmp_ptr[0] -= (tmp_ptr[0] - ( BOX_CORNER_POINT3_X- (charsizexpix+1)));
				tmp_ptr[2] = -((int8_t)tmp_ptr[2]);
				if(tmp_ptr[2])
				{
					tmp_ptr[2]++;
					if(!tmp_ptr[2])
						tmp_ptr[2] = -(((blockpos&0x3) + ymptrbuf[1]&0x7) + 4); // relaunch (random)
				}
				else
					tmp_ptr[2] = -(((blockpos&0x3) + ymptrbuf[1]&0x7) + 4); // relaunch (random)
			}

			// X left border
			if( (tmp_ptr[0] <=  (BOX_CORNER_POINT1_X + 1)) && ((int8_t)tmp_ptr[2]) < 0  )
			{
				tmp_ptr[0] += ((BOX_CORNER_POINT1_X + 1) - tmp_ptr[0] );
				tmp_ptr[2] = -((int8_t)tmp_ptr[2]);
				if(tmp_ptr[2])
				{
					tmp_ptr[2]--;
					if(!tmp_ptr[2])
						tmp_ptr[2] = (((blockpos&0x3) + ymptrbuf[1]&0x7) + 4); // relaunch (random)
				}
				else
					tmp_ptr[2] =  (((blockpos&0x3) + ymptrbuf[1]&0x7) + 4); // relaunch (random)
			}

			// Y floor
			if( (tmp_ptr[1] <=  (BOX_CORNER_POINT1_Y + 1)) )
			{
				if(((int8_t)tmp_ptr[3]) < 0)
				{
					tmp_ptr[1] += ((BOX_CORNER_POINT1_Y + 1) - tmp_ptr[1]);

					// Invert
					tmp_ptr[3] = -((int8_t)tmp_ptr[3]);
				}
				else
				{
					tmp_ptr[3] = +((blockpos&0x7) + ymptrbuf[0]&0xF + 4);
				}
			}

			tmp_ptr[3]--;

			tmp_ptr += 4;
		}

		WAIT_EF9365_READY();

		tmp_ptr = (uint8_t *)&points;

		WAIT_EF9365_READY();

		WR_BYTE( HW_EF9365 + 0x3, size_code );

		for(p=0;p<nbpoints;p++)
		{
			WAIT_EF9365_READY();

			WR_BYTE( HW_CTLHRD_REG, jump_color[p] | ledclavier);

			WR_BYTE( HW_EF9365 + 0x8, 0 );
			WR_BYTE( HW_EF9365 + 0x9, tmp_ptr[0] );
			WR_BYTE( HW_EF9365 + 0xA, 0 );
			WR_BYTE( HW_EF9365 + 0xB, tmp_ptr[1] );
			WR_BYTE( HW_EF9365 + 0x0, string[p] );

			tmp_ptr += 4;
		}

		i++;

		if(nbpoints<NB_MAX_POINTS)
			nbpoints = 1 + (i / 16);

		if(nbpoints > NB_MAX_POINTS || nbpoints > stringlen)
			nbpoints = stringlen;

	}while( new_trigger == old_trigger );

}

