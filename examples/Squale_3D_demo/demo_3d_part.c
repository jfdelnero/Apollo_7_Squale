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
#include "troisd_func.h"

#include "obj3D_Box01.h"
#include "obj3D_Piramid01.h"
#include "obj3D_Hedra01.h"

#include "music/warp_5.h"

#include "images/bart_footpage.h"

const d3dtype * objectlist[]=
{
	&data3d_Hedra01,
	&data3d_Box01,
	&data3d_Piramid01,
	0x00000000
};

extern lines_buffer double_lines_buffer[2];

void demo_3D_oject_part()
{
	int i;
	int object;

	double_lines_buffer[0].nblines = 0;
	double_lines_buffer[1].nblines = 0;

	WAIT_EF9365_READY();
	WR_BYTE( HW_EF9365 + 0x0, 0x04 ); // Clear screen

	WAIT_EF9365_READY();

	display_vectsprite((unsigned char *) &bmp_data_bart_footpage, 0, 0 );

	object = 0;
	i = 0;
	do
	{
		vblank();

		// Erase the old object
		WR_BYTE( HW_CTLHRD_REG, 7 | ledclavier);
		drawobject(&double_lines_buffer[i&1]);

		if( i > 164)
		{
			object++;
			if(object > 2)
				object = 0;
			i=1;
		}

		// Draw the new one
		WR_BYTE( HW_CTLHRD_REG, 0 | ledclavier);
		drawobject(&double_lines_buffer[(i&1)^1]);

		// Prepare the next one...
		calcobject(&double_lines_buffer[i&1],objectlist[object],i&0xFF,(0*2)&0xFF,(i*2)&0xFF);

		i++;

	}while( new_trigger == old_trigger );
}

