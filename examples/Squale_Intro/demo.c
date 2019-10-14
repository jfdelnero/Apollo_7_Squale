// Apollo Squale Intro
// (c) 2016 HxC2001 / Jean-François DEL NERO

#include <stdint.h>
#include <squale_hw.h>

#include "asm_funcs.h"
#include "ym_music.h"

unsigned char cur_digits[4];
unsigned char old_digits[4];
int digit_ptr;
int erase_t;
unsigned char ledclavier;

void abort()
{
}

void waitvideochip()
{
	while(!(*((volatile unsigned char *)HW_EF9365_BASE) & 0x04));
}

void printstr(char * str,unsigned char x,unsigned char y,unsigned char csize,unsigned char color)
{
	volatile unsigned char * ptr;

	ptr = (volatile unsigned char *)HW_EF9365_BASE;

	waitvideochip();

	*((volatile unsigned char *)HW_CTRL_REG_BASE) = color | ledclavier;

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
	ptr = (volatile unsigned char *)HW_EF9365_BASE;

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
	while((*((volatile unsigned char *)HW_EF9365_BASE) & 0x02));
	while(!(*((volatile unsigned char *)HW_EF9365_BASE) & 0x02));
}

void printhex(unsigned char value,unsigned char x,unsigned char y,unsigned char csize,unsigned char color)
{
	unsigned char tmp_v;
	volatile unsigned char * ptr;

	tmp_v = value>>4;

	waitvideochip();

	*((volatile unsigned char *)HW_CTRL_REG_BASE) = color | ledclavier;

	ptr = (volatile unsigned char *)HW_EF9365_BASE;

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

	*((volatile unsigned char *)HW_CTRL_REG_BASE) = color | ledclavier;

	ptr = (volatile unsigned char *)HW_EF9365_BASE;

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

		ptr = (volatile unsigned char *)HW_EF9365_BASE;

		*((volatile unsigned char *)HW_CTRL_REG_BASE) = 0xF | ledclavier;

		ptr[0x9] = x + (unsigned char)(60*(digit_ptr&3));
		ptr[0xB] = y;

		ptr[0x3] = csize;

		if((erase_t&3) || ((digit_ptr&3)!=3))
			vid_asm_func(old_digits[digit_ptr&3]);

		erase_t=erase_t+3;
		waitvideochip();

		*((volatile unsigned char *)HW_CTRL_REG_BASE) = color | ledclavier;

		ptr[0x9] = x + (unsigned char)(60*(digit_ptr&3));
		ptr[0xB] = y;

		vid_asm_func(cur_digits[digit_ptr&3]);

		old_digits[digit_ptr&3] = cur_digits[digit_ptr&3];
	}

	digit_ptr++;
}

int main()
{
	unsigned int i;
	unsigned char j,col;
	unsigned char * ymptr;
	volatile unsigned char * reg, *ptr;

	ledclavier = 0;

	waitvideochip();
	vid_asm_func(0x04);// Clear screen

	old_digits[0] = 'Z';
	old_digits[1] = 'Z';
	old_digits[2] = 'Z';
	old_digits[3] = 'Z';

	ptr = (volatile unsigned char *)HW_EF9365_BASE;

	waitvideochip();

	ptr[0xB] = 4;
	ptr[0x3] = 0x39;

	for(i=0;i<16;i++)
	{
		waitvideochip();
		*((volatile unsigned char *)HW_CTRL_REG_BASE) = (unsigned char)i | ledclavier;

		ptr[0x9] = 0 + (unsigned char)(i*16);

		vid_asm_func(0x0B);
	}

	printstr("- The Squale Intro -",0,248-32,0x22,0x03);
	printstr("           - HxC2001 02/2016 -",0,248-42,0x11,0x00);
	printstr("- The Squale Intro -",0,248-32,0x22,0x03);

	col = 1;
	for(;;)
	{
		ymptr = (unsigned char *)&ymmusic;

		for(i=0;i<1834;i++)
		{
			progymregs_asm_func(ymptr);
			ymptr = ymptr + 14;

			vblank();

			printhex_int_fast(i,8,64,0xAF,col);

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
