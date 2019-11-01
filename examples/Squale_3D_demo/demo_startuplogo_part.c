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

#include "images/mo5_logo.h"

void demo_logo_splash_part()
{
	WAIT_EF9365_READY();

	WR_BYTE( HW_CTLHRD_REG, 0 | ledclavier);
	WR_BYTE( HW_EF9365 + 0x0, 0x0C ); // Clear screen

	vblank();

	display_vectsprite((unsigned char *) &bmp_data_mo5_logo, 0, 0 );

	while( new_trigger == old_trigger  );
}
