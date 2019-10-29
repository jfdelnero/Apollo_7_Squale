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

typedef struct dot2d_
{
	uint8_t x;
	uint8_t y;
}dot2d;

void setpixel(uint8_t x,uint8_t y, uint8_t state);
void setpixelFast(uint8_t x,uint8_t y);
void Ligne(dot2d * pointA,dot2d * pointB,uint8_t state);
void LigneFast(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void cercle(int16_t rayon,int16_t x_centre,int16_t y_centre,uint8_t state);
void display_vectsprite(unsigned char * vectorized_sprite, unsigned char x, unsigned char y);
void Box(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void printstr(char * str,unsigned char x,unsigned char y,unsigned char csize,unsigned char color);
