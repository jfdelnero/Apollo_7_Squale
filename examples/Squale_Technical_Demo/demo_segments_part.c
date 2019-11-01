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
#include "images/squale_logo.h"

#undef BOX_CORNER_POINT3_Y
#define BOX_CORNER_POINT3_Y SCREEN_YSIZE - 60

void demo_segments_part()
{
	#define NB_MAX_POINTS 10
	int i;
	uint8_t p,t;
	uint8_t points[3 * NB_MAX_POINTS];

	uint8_t lines_buffer[(8 * 4) * 4];

	uint8_t *tmp_ptr;
	uint8_t *tmp_ptr2;
	uint8_t nbpoints;
	uint8_t xlen,ylen,cmd;

	WAIT_EF9365_READY();
	WR_BYTE( HW_CTLHRD_REG, 7 | ledclavier);
	WR_BYTE( HW_EF9365 + 0x0, 0x0C ); // Clear screen

	WAIT_EF9365_READY();

	display_vectsprite((unsigned char *) &bmp_data_footpage, 0, SCREEN_YSIZE-33 );

	WR_BYTE( HW_CTLHRD_REG, 3 | ledclavier);

	Box(BOX_CORNER_POINT1_X, BOX_CORNER_POINT1_Y, BOX_CORNER_POINT3_X, BOX_CORNER_POINT3_Y);

	display_vectsprite((unsigned char *) &bmp_data_squale_logo, 50, 0 );

	nbpoints = 1;

	tmp_ptr = (uint8_t *)&points;

	for(p=0;p<(sizeof(lines_buffer));p++)
		lines_buffer[p] = 0;

	for(p=0;p<(sizeof(points)/3);p++)
	{
		tmp_ptr[0] = (SCREEN_XSIZE/2) + p*3 + p - 32 ;
		tmp_ptr[1] = SCREEN_YSIZE/2 + ((p*2) * (-1 * (p&1)) - p/2  );
		tmp_ptr[2] = p & 3;
		tmp_ptr += 3;
	}

	WR_BYTE( HW_EF9365 + 0x8, 0 );
	WR_BYTE( HW_EF9365 + 0xA, 0 );

	i = 0;
	do
	{
		vblank();

		//WR_BYTE( HW_EF9365 + 0x1, RD_BYTE( HW_EF9365 + 0x1) | 0x04 );

		if(i&1)
			tmp_ptr2 = (uint8_t *)&lines_buffer[sizeof(lines_buffer)/2];
		else
			tmp_ptr2 = (uint8_t *)&lines_buffer;

		WAIT_EF9365_READY();

		WR_BYTE( HW_CTLHRD_REG, (7) | ledclavier);
		
		// Erase
		for(p=0;p<nbpoints;p++)
		{
			if( tmp_ptr2[0] > tmp_ptr2[2] )
			{
				cmd = 0x11 | 0x02;
				xlen = tmp_ptr2[0] - tmp_ptr2[2];
			}
			else
			{
				cmd = 0x11;
				xlen = tmp_ptr2[2] - tmp_ptr2[0];
			}

			if( tmp_ptr2[1] > tmp_ptr2[3] )
			{
				cmd |= 0x04;
				ylen = tmp_ptr2[1] - tmp_ptr2[3];
			}
			else
			{
				ylen = tmp_ptr2[3] - tmp_ptr2[1];
			}

			WAIT_EF9365_READY();

			WR_BYTE( HW_EF9365 + 0x9, tmp_ptr2[0] ); // start x
			WR_BYTE( HW_EF9365 + 0xB, tmp_ptr2[1] ); // start y
			WR_BYTE( HW_EF9365 + 0x5, xlen ); // x lenght
			WR_BYTE( HW_EF9365 + 0x7, ylen ); // y lenght
			WR_BYTE( HW_EF9365 + 0x0, cmd );

			tmp_ptr2 += 4;
		}

		if(i&1)
			tmp_ptr2 = (uint8_t *)&lines_buffer;
		else
			tmp_ptr2 = (uint8_t *)&lines_buffer[sizeof(lines_buffer)/2];

		WAIT_EF9365_READY();

		WR_BYTE( HW_CTLHRD_REG, (0) | ledclavier);
		
		// Draw
		for(p=0;p<nbpoints;p++)
		{
			if( tmp_ptr2[0] > tmp_ptr2[2] )
			{
				cmd = 0x11 | 0x02;
				xlen = tmp_ptr2[0] - tmp_ptr2[2];
			}
			else
			{
				cmd = 0x11;
				xlen = tmp_ptr2[2] - tmp_ptr2[0];
			}

			if( tmp_ptr2[1] > tmp_ptr2[3] )
			{
				cmd |= 0x04;
				ylen = tmp_ptr2[1] - tmp_ptr2[3];
			}
			else
			{
				ylen = tmp_ptr2[3] - tmp_ptr2[1];
			}

			WAIT_EF9365_READY();

			WR_BYTE( HW_EF9365 + 0x9, tmp_ptr2[0] ); // start x
			WR_BYTE( HW_EF9365 + 0xB, tmp_ptr2[1] ); // start y
			WR_BYTE( HW_EF9365 + 0x5, xlen ); // x lenght
			WR_BYTE( HW_EF9365 + 0x7, ylen ); // y lenght
			WR_BYTE( HW_EF9365 + 0x0, cmd );

			tmp_ptr2 += 4;
		}

		if(nbpoints<NB_MAX_POINTS)
			nbpoints = 1 + (i / 16);

		if(nbpoints > NB_MAX_POINTS)
			nbpoints = NB_MAX_POINTS;

		tmp_ptr = (uint8_t *)&points;
		for(p=0;p<nbpoints;p++)
		{
			if(tmp_ptr[2] & 0x1)
				tmp_ptr[0]--;
			else
				tmp_ptr[0]++;

			if(tmp_ptr[2] & 0x2)
				tmp_ptr[1]--;
			else
				tmp_ptr[1]++;

			if(tmp_ptr[0] >=  (BOX_CORNER_POINT3_X - 1) )
				tmp_ptr[2] |= 0x1;

			if(tmp_ptr[0] <=  (BOX_CORNER_POINT1_X + 1) )
				tmp_ptr[2] &= ~0x1;

			if(tmp_ptr[1] >=  (BOX_CORNER_POINT3_Y - 1) )
				tmp_ptr[2] |= 0x2;

			if(tmp_ptr[1] <=  (BOX_CORNER_POINT1_Y + 1) )
				tmp_ptr[2] &= ~0x2;

			tmp_ptr += 3;
		}

		tmp_ptr = (uint8_t *)&points;

		if(i&1)
			tmp_ptr2 = (uint8_t *)&lines_buffer[sizeof(lines_buffer)/2];
		else
			tmp_ptr2 = (uint8_t *)&lines_buffer;

		for(p=0;p<nbpoints;p++)
		{
			if(nbpoints>1)
			{
				*tmp_ptr2++ = tmp_ptr[0];
				*tmp_ptr2++ = tmp_ptr[1];
				tmp_ptr += 3;
				if( p == nbpoints - 1 )
					tmp_ptr = (uint8_t *)&points;

				*tmp_ptr2++ = tmp_ptr[0];
				*tmp_ptr2++ = tmp_ptr[1];
			}
			else
			{
				*tmp_ptr2++ = tmp_ptr[0];
				*tmp_ptr2++ = tmp_ptr[1];
				*tmp_ptr2++ = tmp_ptr[0];
				*tmp_ptr2++ = tmp_ptr[1];
				tmp_ptr += 3;
			}
		}

		i++;

	}while( new_trigger == old_trigger );
}

