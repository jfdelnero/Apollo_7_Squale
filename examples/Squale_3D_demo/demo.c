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

#include "asm_funcs.h"

#include "deuxd_func.h"
#include "troisd_func.h"

#include "music/warp_5.h"

#include "demo.h"

#include "demo_segments_part.h"
#include "demo_3d_part.h"
#include "demo_startuplogo_part.h"
#include "demo_presentation_part.h"
#include "demo_squares_part.h"
#include "demo_dotschaos_part.h"
#include "demo_jump_part.h"

unsigned char ledclavier;

unsigned char ymptrbuf[14];
unsigned char * cur_blockmaps[14];
volatile unsigned short blocknum;
unsigned char blockpos;

volatile unsigned char vbl_tick;

void irq_handler (void) __attribute__ ((interrupt));

#define DEMO_MO5LOGO_SCREEN 0
#define DEMO_SQUALELOGO_SCREEN 1
#define DEMO_3D_OBJECT_1   2
#define DEMO_3D_OBJECT_2   3
#define DEMO_DOTSRACING_EFFECT 4
#define DEMO_SEGMENTS_EFFECT 5
#define DEMO_ZOOMBOX_EFFECT 6
#define DEMO_JUMP_EFFECT 7

volatile unsigned char current_trigger;
volatile unsigned char new_trigger;
unsigned char old_trigger;

typedef struct _demo_triggers
{
	unsigned char  effect_code;
	unsigned short music_block;
}demo_triggers;

const demo_triggers demo_program[]=
{
	{ DEMO_MO5LOGO_SCREEN, YM_PAGES_NUMBERS - 23 },
	{ DEMO_SQUALELOGO_SCREEN, 0 },
	{ DEMO_SEGMENTS_EFFECT, 30  },
	{ DEMO_ZOOMBOX_EFFECT, 90  },
	{ DEMO_JUMP_EFFECT, 120  },
	{ DEMO_3D_OBJECT_1, 170 },
	{ DEMO_3D_OBJECT_2, 200 },
	{ DEMO_DOTSRACING_EFFECT, 220  },
	{ 0x00000000, 0xFFFF }
};

void irq_handler()
{
	unsigned char i;

	RD_BYTE( HW_EF9365 );

	progymregs_asm_func((unsigned char *)&ymptrbuf);

	for(i=0;i<14;i++)
	{
		ymptrbuf[i] = *cur_blockmaps[i];
		cur_blockmaps[i]++;
	}

	blockpos += 1;

	if( blockpos >= YM_PAGES_SIZE)
	{
		blockpos = 0;

		blocknum++;

		if( blocknum >= YM_PAGES_NUMBERS )
			blocknum = 0;

		for(i=0;i<14;i++)
		{
			cur_blockmaps[i] = (unsigned char *)ym_reg_blocks[i] + ((((unsigned short)(ym_reg_blockmaps[i][blocknum])) * YM_PAGES_SIZE));
		}

		if( demo_program[current_trigger].music_block == blocknum )
		{
			new_trigger = current_trigger;
			current_trigger++;
			if( demo_program[current_trigger].music_block == 0xFFFF )
			{
				current_trigger = 0;
			}
		}
	}

	vbl_tick++;
}

void abort()
{
}

void waitvideochip()
{
	while(!(*((volatile unsigned char *)HW_EF9365) & 0x04));
}

void vblank()
{
	unsigned char old_vbl_tick;

	old_vbl_tick = vbl_tick;
	while( old_vbl_tick == vbl_tick );
}

int main()
{
	unsigned int i,object, objimagecnt;
	unsigned char j,col;
	volatile unsigned char * reg, *ptr;

	ledclavier = 0;

	WR_BYTE( HW_CTLHRD_REG, 0 | ledclavier);

	waitvideochip();
	WR_BYTE( HW_EF9365 + 0x0, 0x0C ); // Clear screen

	ptr = (volatile unsigned char *)HW_EF9365;

	waitvideochip();

	ptr[0xB] = 4;
	ptr[0x3] = 0x39;

	WR_WORD( IRQ_VECTOR, &irq_handler);

	blocknum = YM_PAGES_NUMBERS - 24;
	blockpos = 0;

	for(j=0;j<14;j++)
	{
		cur_blockmaps[j] = (unsigned char *)ym_reg_blocks[j] + ((((unsigned short)(ym_reg_blockmaps[j][blocknum])) * YM_PAGES_SIZE));
	}

	current_trigger = 0;
	new_trigger = 0;
	old_trigger = 0;

	vbl_tick = 0;
	WR_BYTE( HW_EF9365 + 0x1, 0x23 );

	for(;;)
	{
		vblank();
		switch(demo_program[new_trigger].effect_code)
		{
			case DEMO_MO5LOGO_SCREEN:
				demo_logo_splash_part();
			break;
			case DEMO_SQUALELOGO_SCREEN:
				demo_squale_presentation_part();
			break;
			case DEMO_DOTSRACING_EFFECT:
				demo_dotschaos_part();
			break;
			case DEMO_SEGMENTS_EFFECT:
				demo_segments_part();
			break;
			case DEMO_ZOOMBOX_EFFECT:
				demo_zoombox_part();
			break;
			case DEMO_JUMP_EFFECT:
				demo_jump_part();
			break;
			case DEMO_3D_OBJECT_1:
				demo_3D_oject_part( 0 );
			break;
			case DEMO_3D_OBJECT_2:
				demo_3D_oject_part( 1 );
			break;
		}
		old_trigger = new_trigger;
	}

	return 0;
}
