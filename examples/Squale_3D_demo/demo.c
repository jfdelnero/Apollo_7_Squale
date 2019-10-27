// Apollo Squale 3D Demo
// (c) 2019 HxC2001 / Jean-François DEL NERO

#include <stdint.h>
#include <squale_hw.h>

#include "asm_funcs.h"

#include "deuxd_func.h"
#include "troisd_func.h"

#include "obj3D_Box01.h"
#include "obj3D_Sphere01.h"
#include "obj3D_Cylinder01.h"
#include "obj3D_Prism01.h"
#include "obj3D_Piramid01.h"
#include "obj3D_Hedra01.h"

#include "music/warp_5.h"

#include "images/mo5_logo.h"

unsigned char cur_digits[4];
int digit_ptr;
int erase_t;

unsigned char ledclavier;
extern lines_buffer double_lines_buffer[2];

unsigned char ymptrbuf[14];
unsigned char * cur_blockmaps[14];
volatile unsigned short blocknum;
unsigned char blockpos;

volatile unsigned char vbl_tick;

void irq_handler (void) __attribute__ ((interrupt));

#define DEMO_LOGO_STARTPOS ( YM_PAGES_NUMBERS - 23 )
#define DEMO_LOGO_ENDPOS   ( YM_PAGES_NUMBERS )


#define DEMO_SPLASH_SCREEN 0
#define DEMO_3D_OBJECT_1   1
#define DEMO_3D_OBJECT_2   2

typedef struct _demo_triggers
{
	unsigned char  effect_code;
	unsigned short music_block;
}demo_triggers;

const demo_triggers demo_program[]=
{
	{ DEMO_SPLASH_SCREEN, YM_PAGES_NUMBERS - 23 },
	{ DEMO_3D_OBJECT_1, 0 },
	{ DEMO_3D_OBJECT_2, 100 },
	{ 0x00000000, 0xFFFF }
};

volatile unsigned char current_trigger;
volatile unsigned char new_trigger;
unsigned char old_trigger;

void irq_handler()
{
	unsigned char i;

	RD_BYTE( HW_EF9365 );

	progymregs_asm_func(&ymptrbuf);

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
			cur_blockmaps[i] = ym_reg_blocks[i] + ((((unsigned short)(ym_reg_blockmaps[i][blocknum])) * YM_PAGES_SIZE));
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

const d3dtype * objectlist[]=
{
	&data3d_Hedra01,
	&data3d_Box01,
	&data3d_Piramid01,
	&data3d_Prism01,
	&data3d_Cylinder01,
	0x00000000
};

char * strings[]=
{
	"Apollo 7 Squale",
	"CPU: 6809",
	"RAM: 64KB",
	"Video: EF9365P",
	"       32KB",
	"       16 colors",
	"Video RAM: 32KB",
	"Sound: AY-3-8910A",
	"V24/V25/V54 modem",
	0
};

void waitvideochip()
{
	while(!(*((volatile unsigned char *)HW_EF9365) & 0x04));
}

void printstr(char * str,unsigned char x,unsigned char y,unsigned char csize,unsigned char color)
{
	volatile unsigned char * ptr;

	ptr = (volatile unsigned char *)HW_EF9365;

	waitvideochip();

	WR_BYTE( HW_CTLHRD_REG, color | ledclavier);

	ptr[0x9] = x;
	ptr[0xB] = y;

	ptr[0x3] = csize;

	while(*str)
	{
		waitvideochip();
		vid_asm_func(*str);
		str++;
	}
}

void line(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2)
{
	unsigned char cmd;
	volatile unsigned char * ptr;

	waitvideochip();
	ptr = (volatile unsigned char *)HW_EF9365;

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

void vblank()
{
	unsigned char old_vbl_tick;

	old_vbl_tick = vbl_tick;
	while( old_vbl_tick == vbl_tick );
}

void printhex(unsigned char value,unsigned char x,unsigned char y,unsigned char csize,unsigned char color)
{
	unsigned char tmp_v;
	volatile unsigned char * ptr;

	tmp_v = value>>4;

	waitvideochip();

	WR_BYTE( HW_CTLHRD_REG, color | ledclavier);

	ptr = (volatile unsigned char *)HW_EF9365;

	ptr[0x9] = x;
	ptr[0xB] = y;

	ptr[0x3] = csize;

	if(tmp_v<=9)
	{
		vid_asm_func('0'+tmp_v);
	}
	else
	{
		vid_asm_func('A'+(tmp_v-10));
	}

	tmp_v = value&0xF;

	if(tmp_v<=9)
	{
		waitvideochip();
		vid_asm_func('0'+tmp_v);
	}
	else
	{
		waitvideochip();
		vid_asm_func('A'+(tmp_v-10));
	}
}

void printhex_int(unsigned int value,unsigned char x,unsigned char y,unsigned char csize,unsigned char color)
{
	unsigned char tmp_v;
	volatile unsigned char * ptr;

	tmp_v = (unsigned char)(value>>(4+8))&0xF;

	waitvideochip();

	WR_BYTE( HW_CTLHRD_REG, color | ledclavier);

	ptr = (volatile unsigned char *)HW_EF9365;

	ptr[0x9] = x;
	ptr[0xB] = y;

	ptr[0x3] = csize;

	if(tmp_v<=9)
	{
		vid_asm_func('0'+tmp_v);
	}
	else
	{
		vid_asm_func('A'+(tmp_v-10));
	}

	tmp_v = (unsigned char)((value>>(8))&0xF);

	if(tmp_v<=9)
	{
		waitvideochip();
		vid_asm_func('0'+tmp_v);
	}
	else
	{
		waitvideochip();
		vid_asm_func('A'+(tmp_v-10));
	}

	tmp_v = (unsigned char)((value>>(4))&0xF);

	if(tmp_v<=9)
	{
		waitvideochip();
		vid_asm_func('0'+tmp_v);
	}
	else
	{
		waitvideochip();
		vid_asm_func('A'+(tmp_v-10));
	}

	tmp_v = (unsigned char)((value)&0xF);

	if(tmp_v<=9)
	{
		waitvideochip();
		vid_asm_func('0'+tmp_v);
	}
	else
	{
		waitvideochip();
		vid_asm_func('A'+(tmp_v-10));
	}
}

void printhex_int_fast(unsigned int value,unsigned char x,unsigned char y,unsigned char csize,unsigned char color)
{
	unsigned char tmp_v,i;
	volatile unsigned char * ptr;

	tmp_v = (unsigned char)(value>>(4+8))&0xF;

	if(tmp_v<=9)
	{
		cur_digits[0] = '0'+tmp_v;
	}
	else
	{
		cur_digits[0] = 'A'+(tmp_v-10);
	}

	tmp_v = (unsigned char)((value>>(8))&0xF);

	if(tmp_v<=9)
	{
		cur_digits[1] = '0'+tmp_v;
	}
	else
	{
		cur_digits[1] = 'A'+(tmp_v-10);
	}

	tmp_v = (unsigned char)((value>>(4))&0xF);

	if(tmp_v<=9)
	{
		cur_digits[2] = '0'+tmp_v;
	}
	else
	{
		cur_digits[2] = 'A'+(tmp_v-10);
	}

	tmp_v = (unsigned char)((value)&0xF);

	if(tmp_v<=9)
	{
		cur_digits[3] = '0'+tmp_v;
	}
	else
	{
		cur_digits[3] = 'A'+(tmp_v-10);
	}

	if(i!=4)
	{
		waitvideochip();

		ptr = (volatile unsigned char *)HW_EF9365;

		WR_BYTE( HW_CTLHRD_REG, 0xF | ledclavier);

		ptr[0x9] = x + (unsigned char)(60*(digit_ptr&3));
		ptr[0xB] = y;

		ptr[0x3] = csize;

		if((erase_t&3) || ((digit_ptr&3)!=3))
			vid_asm_func(cur_digits[digit_ptr&3]);

		erase_t=erase_t+3;
		waitvideochip();

		WR_BYTE( HW_CTLHRD_REG, color | ledclavier);

		ptr[0x9] = x + (unsigned char)(60*(digit_ptr&3));
		ptr[0xB] = y;

		vid_asm_func(cur_digits[digit_ptr&3]);

	}

	digit_ptr++;
}


void demo_logo_splash_part()
{
	vblank();

	display_vectsprite((unsigned char *) &bmp_data_mo5_logo, 0, 0 );

	while( new_trigger == old_trigger  );
}

void demo_squale_presentation_part()
{
	unsigned char i;

	i = 0;
	while ( strings[i] )
	{
		printstr(strings[i],0,255-((i+1)*8),0x11,0x04);
		i++;
	}
}

void demo_3D_oject_part(int object)
{
	int i;

	waitvideochip();
	WR_BYTE( HW_EF9365 + 0x0, 0x04 ); // Clear screen

	waitvideochip();

	i = 0;
	do
	{
		// Erase the old object
		WR_BYTE( HW_CTLHRD_REG, 7 | ledclavier);
		drawobject(&double_lines_buffer[i&1]);

		// Draw the new one
		WR_BYTE( HW_CTLHRD_REG, 0 | ledclavier);
		drawobject(&double_lines_buffer[(i&1)^1]);

		// Prepare the next one...
		calcobject(&double_lines_buffer[i&1],objectlist[object],i&0xFF,(0*3)&0xFF,(0*2)&0xFF);

		i++;
	}while( new_trigger == old_trigger );
}

int main()
{
	unsigned int i,object, objimagecnt;
	unsigned char j,col;
	volatile unsigned char * reg, *ptr;

	ledclavier = 0;
	double_lines_buffer[0].nblines = 0;
	double_lines_buffer[1].nblines = 0;

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
		cur_blockmaps[j] = ym_reg_blocks[j] + ((((unsigned short)(ym_reg_blockmaps[j][blocknum])) * YM_PAGES_SIZE));
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
			case DEMO_SPLASH_SCREEN:
				demo_logo_splash_part();
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
