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
// <<The Squale 3D Demo>> A demo for the French Apollo Squale computer.
//
// Written by: Jean François DEL NERO
//
// You are free to do what you want with this code.
// A credit is always appreciated if you include it into your prod :)
//
// Change History (most recent first):
///////////////////////////////////////////////////////////////////////////////////

typedef struct dot_
{
	int16_t x;
	int16_t y;
	int16_t z;
}dot;

typedef struct polyg_
{
	dot point[3];
	uint8_t vectricemask;
}polyg;

typedef  struct _data3dtype
{
   uint8_t nbfaces;
   uint8_t nbvertex;
   int8_t * vertex;
   int8_t * faces;
}data3dtype;

typedef  struct _d3dtype
{
   uint8_t nbfaces;
   const int8_t * vertex;
}d3dtype;

typedef struct _lines_buffer
{
	uint8_t nblines;
	uint8_t lines[4*256];
}lines_buffer;

void Show3DPoint(dot * point,dot2d * point2D);
void rotateX(dot * point,uint8_t xang);
void rotateY(dot * point,uint8_t yang);
void rotateZ(dot * point,uint8_t zang);
//void rotateXYZ(int8_t * polygone,uint8_t xang,uint8_t yang,uint8_t zang);
void calcobject(lines_buffer * ln_buffer, const d3dtype * obj,uint8_t xrotate,uint8_t yrotate,uint8_t zrotate);
void drawobject(lines_buffer * ln_buffer);
