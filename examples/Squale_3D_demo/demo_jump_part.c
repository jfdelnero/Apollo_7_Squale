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

void demo_jump_part()
{
	#define NB_MAX_POINTS 1
	int i;
	uint8_t p,t;
	// x pos, y pos, x speed/way, y speed/way

	uint8_t points[4 * NB_MAX_POINTS];
	uint8_t *tmp_ptr;
	uint8_t nbpoints;

	waitvideochip();
	WR_BYTE( HW_CTLHRD_REG, 7 | ledclavier);
	WR_BYTE( HW_EF9365 + 0x0, 0x0C ); // Clear screen

	waitvideochip();

	display_vectsprite((unsigned char *) &bmp_data_footpage, 0, SCREEN_YSIZE-33 );

	WR_BYTE( HW_CTLHRD_REG, 3 | ledclavier);

	Box(BOX_CORNER_POINT1_X, BOX_CORNER_POINT1_Y, BOX_CORNER_POINT3_X, BOX_CORNER_POINT3_Y);

	nbpoints = 1;

	tmp_ptr = (uint8_t *)&points;

	for(p=0;p<(sizeof(points)/3);p++)
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
		vblank();

		tmp_ptr = (uint8_t *)&points;

		waitvideochip();
		WR_BYTE( HW_CTLHRD_REG, (7) | ledclavier);

		//WR_BYTE( HW_EF9365 + 0x1, RD_BYTE( HW_EF9365 + 0x1) | 0x04 );

		for(p=0;p<nbpoints;p++)
		{
			printstr("Squale",tmp_ptr[0],tmp_ptr[1],0x44,7);

			tmp_ptr += 4;
		}

		tmp_ptr = (uint8_t *)&points;
		for(p=0;p<nbpoints;p++)
		{
			tmp_ptr[0] += tmp_ptr[2];
			tmp_ptr[1] += ((int8_t)tmp_ptr[3])/1;

			if( (tmp_ptr[0] >=  (BOX_CORNER_POINT3_X - 1)) && ((int8_t)tmp_ptr[2]) >= 0 )
			{
				tmp_ptr[0] -= (tmp_ptr[0] - (BOX_CORNER_POINT3_X - 1));
				tmp_ptr[2] = -((int8_t)tmp_ptr[2]);
			}

			if( (tmp_ptr[0] <=  (BOX_CORNER_POINT1_X + 1)) && ((int8_t)tmp_ptr[2]) < 0  )
			{
				tmp_ptr[0] += ((BOX_CORNER_POINT1_X + 1) - tmp_ptr[0] );
				tmp_ptr[2] = -((int8_t)tmp_ptr[2]);
			}

			if( (tmp_ptr[1] >=  (BOX_CORNER_POINT3_Y - 1)) && ((int8_t)tmp_ptr[3]) >= 0 )
			{
				tmp_ptr[1] -= (tmp_ptr[1] - (BOX_CORNER_POINT3_Y - 1));
				tmp_ptr[3] = -((int8_t)tmp_ptr[3]);
			}

			if( (tmp_ptr[1] <=  (BOX_CORNER_POINT1_Y + 1)) && ((int8_t)tmp_ptr[3]) < 0  )
			{
				tmp_ptr[1] += ((BOX_CORNER_POINT1_Y + 1) - tmp_ptr[1]);
				tmp_ptr[3] = -((int8_t)tmp_ptr[3]);
			}

			if( ((int8_t)tmp_ptr[3]) >= 0 )
				tmp_ptr[3]--;

			if( ((int8_t)tmp_ptr[3]) < 0 )
				tmp_ptr[3]--;

			tmp_ptr += 4;
		}

		waitvideochip();

		//WR_BYTE( HW_EF9365 + 0x1, RD_BYTE( HW_EF9365 + 0x1) & ~0x04 );

		tmp_ptr = (uint8_t *)&points;
		WR_BYTE( HW_CTLHRD_REG, (0) | ledclavier);

		for(p=0;p<nbpoints;p++)
		{
			printstr("Squale",tmp_ptr[0],tmp_ptr[1],0x44,0);

			tmp_ptr += 4;
		}

		i++;

		if(nbpoints<NB_MAX_POINTS)
			nbpoints = 1 + (i / 16);

		if(nbpoints > NB_MAX_POINTS)
			nbpoints = NB_MAX_POINTS;
	}while( new_trigger == old_trigger );

}

