// Apollo Squale 3D Demo
// (c) 2019 HxC2001 / Jean-François DEL NERO

#include <stdint.h>
#include <squale_hw.h>

#include "asm_funcs.h"
#include "ym_music.h"

#include "deuxd_func.h"
#include "troisd_func.h"

#include "obj3D_Box01.h"
#include "obj3D_Sphere01.h"
#include "obj3D_Cylinder01.h"
#include "obj3D_Prism01.h"
#include "obj3D_Piramid01.h"
#include "obj3D_Hedra01.h"

unsigned char cur_digits[4];
unsigned char old_digits[4];
int digit_ptr;
int erase_t;

unsigned char ledclavier;
extern lines_buffer double_lines_buffer[2];
unsigned char * ymptr;


void irq_handler (void) __attribute__ ((interrupt));

void irq_handler()
{
	RD_BYTE( HW_EF9365 );

	progymregs_asm_func(ymptr);

	ymptr = ymptr + 14;

	if(ymptr >= (&ymmusic + sizeof(ymmusic) ) )
		ymptr = (unsigned char *)&ymmusic;
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
	while( ( RD_BYTE( HW_EF9365 ) & 0x02 ) );
	while(!( RD_BYTE( HW_EF9365 ) & 0x02 ) );
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

	i = 0;
	while( ( cur_digits[digit_ptr&3] == old_digits[digit_ptr&3] ) )//&& ( i < 4 ) )
	{
		digit_ptr++;
		i++;
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
			vid_asm_func(old_digits[digit_ptr&3]);

		erase_t=erase_t+3;
		waitvideochip();

		WR_BYTE( HW_CTLHRD_REG, color | ledclavier);

		ptr[0x9] = x + (unsigned char)(60*(digit_ptr&3));
		ptr[0xB] = y;

		vid_asm_func(cur_digits[digit_ptr&3]);

		old_digits[digit_ptr&3] = cur_digits[digit_ptr&3];
	}

	digit_ptr++;
}

int main()
{
	unsigned int i,object, objimagecnt;
	unsigned char j,col;
	volatile unsigned char * reg, *ptr;

	ledclavier = 0;
	double_lines_buffer[0].nblines = 0;
	double_lines_buffer[1].nblines = 0;

	waitvideochip();
	vid_asm_func(0x04);// Clear screen

	old_digits[0] = 'Z';
	old_digits[1] = 'Z';
	old_digits[2] = 'Z';
	old_digits[3] = 'Z';

	ptr = (volatile unsigned char *)HW_EF9365;

	waitvideochip();

	ptr[0xB] = 4;
	ptr[0x3] = 0x39;

	WR_WORD( IRQ_VECTOR, &irq_handler);

	ymptr = (unsigned char *)&ymmusic;

	WR_BYTE( HW_EF9365 + 0x1, 0x23 );

	for(i=0;i<16;i++)
	{
		waitvideochip();

		WR_BYTE( HW_CTLHRD_REG, i | ledclavier);
		ptr[0x9] = 0 + (unsigned char)(i*16);

		vid_asm_func(0x0B);
	}

	i = 0;
	while ( strings[i] )
	{
		printstr(strings[i],0,256-((i+1)*8),0x11,0x04);
		i++;
	}

	object = 0;
	objimagecnt = 0;

	col = 1;
	for(;;)
	{

		for(i=0;i<1834;i++)
		{
			vblank();

			// Erase the old object
			WR_BYTE( HW_CTLHRD_REG, 7 | ledclavier);
			drawobject(&double_lines_buffer[i&1]);

			// Draw the new one
			WR_BYTE( HW_CTLHRD_REG, 0 | ledclavier);
			drawobject(&double_lines_buffer[(i&1)^1]);

			// Prepare the next one...
			calcobject(&double_lines_buffer[i&1],objectlist[object],i&0xFF,(0*3)&0xFF,(0*2)&0xFF);

			objimagecnt++;
			if(objimagecnt > 90)
			{
				object++;
				if(!objectlist[object])
					object = 0;
				objimagecnt = 0;
			}

			//printhex_int_fast(i,8,64,0xAF,col);

			if(i&0x8)
				ledclavier = 0x00;
			else
				ledclavier = 0x80;
		}

		col++;

		if(col==0xF)
			col = 0;

		if(col==0x7)
			col = 0x8;

		old_digits[0] = 'Z';
	}

	return 0;
}
