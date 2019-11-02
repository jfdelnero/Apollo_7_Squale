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

#include "images/mo5_logo.h"
#include "images/squale_logo.h"
#include "images/apollo_7_logo.h"
#include "images/footpage.h"

char * strings[]=
{
	"CPU: 6809 @ 3.5 MHz",
	"RAM: 64KB",
	"Video: EF9365P",
	"       32KB",
	"       16 colors",
	"Video RAM: 32KB",
	"Sound: AY-3-8910A",
	"Embedded modem",
	"Tape and Cartridge support",
	0
};

void demo_squale_presentation_part()
{
	unsigned char i;

	WAIT_EF9365_READY();
	WR_BYTE( HW_CTLHRD_REG, 7 | ledclavier);
	WR_BYTE( HW_EF9365 + 0x0, 0x0C ); // Clear screen

	WAIT_EF9365_READY();

	display_vectsprite((unsigned char *) &bmp_data_footpage, 0, SCREEN_YSIZE-33 );
	display_vectsprite((unsigned char *) &bmp_data_apollo_7_logo, 26, 10 );

	i = 0;
	while ( strings[i] )
	{
		printstr(strings[i],40,100-((i+1)*8),0x11,0x04);
		i++;
	}

	do
	{
		for(i=0;i<10 && ( new_trigger == old_trigger );i++)
		{
			vblank();
			display_vectsprite((unsigned char *) &bmp_data_squale_logo, 40, i + 64 );
		}

		for(i=0;i<10 && ( new_trigger == old_trigger );i++)
		{
			vblank();
			display_vectsprite((unsigned char *) &bmp_data_squale_logo, 40, 64 + 10 - (i) );
		}
	}while( new_trigger == old_trigger );
}

