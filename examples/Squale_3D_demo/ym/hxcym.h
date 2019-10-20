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
// File : hxcym.h
// Contains: a tiny ym player
//
// Written by: Jean François DEL NERO
//
// Change History (most recent first):
///////////////////////////////////////////////////////////////////////////////////

#ifndef YMPLAY_DEF
#define YMPLAY_DEF

// Basic type
typedef unsigned char	muchar;
typedef unsigned short	muint;
typedef          short	mint;
typedef unsigned int    mulong;

#if IS_LITTLE_ENDIAN
	#define ENDIAN_32BIT(value)	value
	#define ENDIAN_16BIT(value)	value
	#define GET_32BIT_WORD(buffer, location)	( ((mulong)buffer[location+3]<<24) + ((mulong)buffer[location+2]<<16) + ((mulong)buffer[location+1]<<8) + (mulong)buffer[location+0] )
	#define GET_16BIT_WORD(buffer, location)	( ((muint)buffer[location+1]<<8) + (muint)buffer[location+0] )

	#define SET_32BIT_WORD(buffer, location, value)	{ buffer[location+0] = (muchar)((value)&0xFF); \
													  buffer[location+1] = (muchar)((value>>8)&0xFF); \
													  buffer[location+2] = (muchar)((value>>16)&0xFF); \
													  buffer[location+3] = (muchar)((value>>24)&0xFF); }

	#define SET_16BIT_WORD(buffer, location, value)	{ buffer[location+0] = (muchar)((value)&0xFF); \
													  buffer[location+1] = (muchar)((value>>8)&0xFF); }
// Big Endian
#else
	#define ENDIAN_32BIT(value)	( ((mulong)(value&0x000000FF)<<24) + ((mulong)(value&0x0000FF00)<<8) + ((mulong)(value&0x00FF0000)>>8) + ((mulong)(value&0xFF000000)>>24) )
	#define ENDIAN_16BIT(value)	( ((muint)(value&0x00FF)<<8) + ((muint)(value&0xFF00)>>8)  )
	#define GET_32BIT_WORD(buffer, location)	( ((mulong)buffer[location+3]<<24) + ((mulong)buffer[location+2]<<16) + ((mulong)buffer[location+1]<<8) + (mulong)buffer[location+0] )
	#define GET_16BIT_WORD(buffer, location)	( ((muint)buffer[location+1]<<8) + (muint)buffer[location+0] )

	#define SET_32BIT_WORD(buffer, location, value)	{ buffer[location+0] = (muchar)((value)&0xFF); \
													  buffer[location+1] = (muchar)((value>>8)&0xFF); \
													  buffer[location+2] = (muchar)((value>>16)&0xFF); \
													  buffer[location+3] = (muchar)((value>>24)&0xFF); }

	#define SET_16BIT_WORD(buffer, location, value)	{ buffer[location+0] = (muchar)((value)&0xFF); \
													  buffer[location+1] = (muchar)((value>>8)&0xFF); }
#endif


#define NUMMAXCHANNELS 32
#define MAXNOTES 12*12
#define SAMPLE_RATE 44100
//
// YM file structures
//


#pragma pack(1)

typedef struct
{
	muchar	size;
	muchar	sum;
	muchar	id[5];
	mulong	packed;
	mulong	original;
	muchar	reserved[5];
	muchar	level;
	muchar	name_lenght;
} lzhHeader;

typedef struct {
	muchar  file_id[4];
	muchar  signature[8];
	mulong  nb_of_frame;
	mulong  attributs;
	muint   nb_of_digidrum;
	mulong  master_clock;
	muint   frame_freq;
	mulong  loop_frame;
	muint   extension_size;
} ym_file;

typedef struct {
	mulong  sample_size;
	muchar  sample_data;
} ym_digidrum;

#pragma pack()

//
// HxCYm Internal structures
//

typedef struct {
	char *  sampledata[31];

	unsigned char * unpacked_ym;

	mulong  playrate;
	muint   tablepos;
	muint   patternpos;
	muint   patterndelay;
	muint   jump_loop_effect;
	muchar  bpm;
	mulong  patternticks;
	mulong  patterntickse;
	mulong  patternticksaim;
	mulong  sampleticksconst;

	mulong  samplenb;

	muint   number_of_channels;

	muint   fullperiod[MAXNOTES * 8];

	muint   mod_loaded;

	mint    last_r_sample;
	mint    last_l_sample;

	mint    stereo;
	mint    stereo_separation;
	mint    bits;
	mint    filter;

	mulong  main_clock;
	mulong  frame_freq;
	mulong  nb_of_frames;
	mulong  nb_of_digidrums;

	mulong  registers_dump_base;
	mulong  interleave;

} ymcontext;

///////////////////////////////////////////////////////////////////////////////////
// HxCYM Core API:
// -------------------------------------------
// int  hxcym_init(ymcontext * ymctx)
//
// - Initialize the ymcontext buffer. Must be called before doing anything else.
//   Return 1 if success. 0 in case of error.
// -------------------------------------------
// int  hxcym_load( ymcontext * ymctx, void * mod_data, int mod_data_size )
//
// - "Load" a YM from memory (from "mod_data" with size "mod_data_size").
//   Return 1 if success. 0 in case of error.
// -------------------------------------------
// void hxcym_unload( ymcontext * ymctx )
// - "Unload" / clear the player status.
// -------------------------------------------
///////////////////////////////////////////////////////////////////////////////////

int  hxcym_init( ymcontext * ymctx );
int  hxcym_setcfg( ymcontext * ymctx, int samplerate, int bits, int stereo, int stereo_separation, int filter);
int  hxcym_load( ymcontext * ymctx, void * ym_data, int ym_data_size );
void hxcym_unload( ymcontext * ymctx );

#endif

