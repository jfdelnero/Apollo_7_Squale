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
// File : hxcym.c
// Contains: a tiny ym player
//
// Written by: Jean François DEL NERO
//
// You are free to do what you want with this code.
// A credit is always appreciated if you include it into your prod :)
//
// This file include some parts of the Noisetracker/Soundtracker/Protracker
// Module Format documentation written by Andrew Scott (Adrenalin Software)
// (modformat.txt)
//
// The core (hxcmod.c/hxcmod.h) is designed to have the least external dependency.
// So it should be usable on almost all OS and systems.
// Please also note that no dynamic allocation is done into the HxCMOD core.
//
// Change History (most recent first):
///////////////////////////////////////////////////////////////////////////////////

//#define TEST 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hxcym.h"

#include "packer/lzh.h"

#include "packer/lzh.h"


#include "packed_ym.h"

#ifdef TEST

unsigned char ymptrbuf[14];
unsigned char * cur_blockmaps[14];
unsigned short blocknum;
unsigned char blockpos;

void irq_handler()
{
	unsigned char i;
	unsigned char * tmp;

	for(i=0;i<14;i++)
	{
		tmp = cur_blockmaps[i];
		ymptrbuf[i] = *tmp;
		cur_blockmaps[i] = tmp + 1;
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
			cur_blockmaps[i] = (ym_reg_blocks[i] + (((unsigned short)(ym_reg_blockmaps[i][blocknum])) * YM_PAGES_SIZE));
		}
	}

}

#endif

///////////////////////////////////////////////////////////////////////////////////

static void memcopy( void * dest, void *source, unsigned long size )
{
	unsigned long i;
	unsigned char * d,*s;

	d=(unsigned char*)dest;
	s=(unsigned char*)source;
	for(i=0;i<size;i++)
	{
		d[i]=s[i];
	}
}

static void memclear( void * dest, unsigned char value, unsigned long size )
{
	unsigned long i;
	unsigned char * d;

	d=(unsigned char*)dest;
	for(i=0;i<size;i++)
	{
		d[i]=value;
	}
}

static int memcompare( unsigned char * buf1, unsigned char * buf2, unsigned int size )
{
	unsigned int i;

	i = 0;

	while(i<size)
	{
		if(buf1[i] != buf2[i])
		{
			return 0;
		}
		i++;
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////
int hxcym_init(ymcontext * ymctx)
{
	muint i,j;

	if( ymctx )
	{
		memclear(ymctx,0,sizeof(ymcontext));
		ymctx->playrate = 44100;
		ymctx->stereo = 1;
		ymctx->stereo_separation = 1;
		ymctx->bits = 16;
		ymctx->filter = 1;

		return 1;
	}

	return 0;
}

int hxcym_setcfg(ymcontext * ymctx, int samplerate, int bits, int stereo, int stereo_separation, int filter)
{
	if( ymctx )
	{
		ymctx->playrate = samplerate;

		if( stereo )
			ymctx->stereo = 1;
		else
			ymctx->stereo = 0;

		if(stereo_separation < 4)
		{
			ymctx->stereo_separation = stereo_separation;
		}

		if( bits == 8 || bits == 16 )
		{
			ymctx->bits = bits;
		}

		if( filter )
			ymctx->filter = 1;
		else
			ymctx->filter = 0;

		return 1;
	}

	return 0;
}

unsigned char get_reg( ymcontext * ymctx, int frame, int reg )
{

	if( ymctx->interleave )
	{
		return ymctx->unpacked_ym[ ymctx->registers_dump_base + ( ( reg * ymctx->nb_of_frames ) + frame ) ];
	}
	else
	{
		return ymctx->unpacked_ym[ ymctx->registers_dump_base + ( ( frame * 16 ) + reg ) ];
	}
}

void push_hex(FILE * f, int * col, unsigned char data)
{
	fprintf(f,"0x%.2X", data );

	*col = *col + 1;
	if(*col<16)
	{
		fprintf(f,",");
	}
	else
	{
		*col=0;
		fprintf(f,",\r\n");
	}
}

void push_bin(FILE * f, int * col, unsigned char data)
{
	fputc(data,f);
}

void write_binary_file(char * name,int index,unsigned char * buffer,int size)
{
	FILE * f;
	char tmp[512];

	sprintf(tmp,name,index);

	f= fopen( tmp,"wb");
	if(f)
	{
		fwrite(buffer,size,1,f);
		fclose(f);
	}
}

int hxcym_load( ymcontext * ymctx, void * ym_data, int ym_data_size )
{
	muint i, j, max,rle_len,k,l,n,o;
	unsigned short t;
	lzhHeader * lzhH;
	ym_file * ym_f;
	unsigned char * ymmemory,* ymmodmemory;
	FILE * f,*f2;
	unsigned char curchar;
	int col,count,total_count;
	unsigned char * regs_dump;
	unsigned int * block_map[14],* block_map2[14];
	int blocksize;

	unsigned char * packed_regs_dump;
	unsigned char * packed_block_map;

	unsigned char * unpacked_regs_dump;

	ymmemory = (unsigned char *)ym_data;
	ymmodmemory = ymmemory + ym_data_size;

	if(ymmemory)
	{
		if( ymctx )
		{
			lzhH = (lzhHeader *)ym_data;
			if( !strncmp(lzhH->id,"-lh5-",5) )
			{
				printf("Unpacking... (packed : %d bytes, unpacked : %d bytes\n", lzhH->packed, lzhH->original);

				ymctx->unpacked_ym = malloc(lzhH->original);
				if(!ymctx->unpacked_ym)
				{
					printf("Memory allocation error...\n");
					return -1;
				}
				memset(ymctx->unpacked_ym,0,lzhH->original);
				unlzh(ymmemory + (lzhH->size + 2),lzhH->packed, ymctx->unpacked_ym,lzhH->original);

				ym_f = (ym_file *)ymctx->unpacked_ym;

				ymctx->interleave = 1;

				ymctx->frame_freq = ENDIAN_16BIT(ym_f->frame_freq);
				ymctx->main_clock = ENDIAN_32BIT(ym_f->master_clock);
				ymctx->nb_of_frames = ENDIAN_32BIT(ym_f->nb_of_frame);
				ymctx->nb_of_digidrums = ENDIAN_16BIT(ym_f->nb_of_digidrum);

				ymctx->registers_dump_base = sizeof(ym_file) + ym_f->extension_size;

				i = 0;
				while( i < ymctx->nb_of_digidrums )
				{
					ymctx->registers_dump_base += ( ENDIAN_32BIT( *(&ymctx->unpacked_ym[ymctx->registers_dump_base]) ) + 4);
					i++;
				}

				printf("Frame frequency : %d Hz, Clock : %d Hz, Nb of frames : %d, Nb of digidrums : %d\n",ymctx->frame_freq,ymctx->main_clock,ymctx->nb_of_frames,ymctx->nb_of_digidrums);

				// Song name
				printf("Name : ");
				while( ymctx->unpacked_ym[ymctx->registers_dump_base] )
				{
					printf("%c",ymctx->unpacked_ym[ymctx->registers_dump_base]);
					ymctx->registers_dump_base++;
				}
				ymctx->registers_dump_base++;
				printf("\n");

				// Author name
				printf("Author : ");
				while( ymctx->unpacked_ym[ymctx->registers_dump_base] )
				{
					printf("%c",ymctx->unpacked_ym[ymctx->registers_dump_base]);
					ymctx->registers_dump_base++;
				}
				printf("\n");
				ymctx->registers_dump_base++;

				// Song comment
				printf("Comment : ");
				while( ymctx->unpacked_ym[ymctx->registers_dump_base] )
				{
					printf("%c",ymctx->unpacked_ym[ymctx->registers_dump_base]);
					ymctx->registers_dump_base++;
				}
				printf("\n");
				ymctx->registers_dump_base++;

				/*
				f = fopen("raw_ym.txt","w+b");

				for(i=0;i<ymctx->nb_of_frames;i++)
				{
					fprintf(f," ");
					for(j=0;j<14;j++)
					{
						fprintf(f,"0x%.2X", get_reg( ymctx, i, j ) );
						//if(j!=13)
							fprintf(f,",");

					}
					fprintf(f,"\r\n");
				}
				fclose(f);*/

////
				f = fopen("packed_ym.c","w+b");
				f2 = fopen("packed_ym.h","w+b");

				total_count = 0;
				regs_dump = malloc(ymctx->nb_of_frames);
				if(regs_dump)
				{
					memset(regs_dump,0,ymctx->nb_of_frames);

					blocksize = 16;
					//for(blocksize = 8;blocksize < 256; blocksize += 8)
					{
						total_count = 0;

						for(j=0;j<14;j++)
						{
							for(i=0;i<ymctx->nb_of_frames;i++)
							{
								regs_dump[i] = get_reg( ymctx, i, j );
							}

							count = 0;

							block_map[j] = malloc(((ymctx->nb_of_frames/blocksize)*sizeof(unsigned int)));
							memset(block_map[j],0x00,(ymctx->nb_of_frames/blocksize)*sizeof(unsigned int));

							block_map2[j] = malloc((ymctx->nb_of_frames/blocksize)*sizeof(unsigned int));
							memset(block_map2[j],0x00,(ymctx->nb_of_frames/blocksize)*sizeof(unsigned int));

							for(l=0;l<ymctx->nb_of_frames/blocksize;l++)
							{
								if( !block_map2[j][l] )
								{
									for(k=0;k<ymctx->nb_of_frames/blocksize;k++)
									{
										if(!block_map2[j][k] && (l != k))
										{
											if(!memcmp(&regs_dump[k * blocksize],&regs_dump[l * blocksize],blocksize))
											{
												block_map[j][k] = l;
												block_map2[j][k] = 0xFF;
												count++;
											}
										}
										else
										{
											if(l == k)
											{
												block_map[j][k] = l;
												block_map2[j][k] = 0xFF;
											}
										}
									}
								}
							}

							count = 0;
							for(l=0;l<ymctx->nb_of_frames/blocksize;l++)
							{
								if(block_map[j][l] != l)
								{
									count++;
								}
							}

							// Pack regs...
							packed_regs_dump = malloc(blocksize * count);
							memset(packed_regs_dump,0,blocksize * count);

							packed_block_map = malloc(ymctx->nb_of_frames/blocksize);
							memset(packed_block_map,0, ymctx->nb_of_frames/blocksize);

							o = 0;
							for(l=0;l<ymctx->nb_of_frames/blocksize;l++)
							{
								// Pack source blocksize
								if( block_map[j][l] == l)
								{
									memcpy(&packed_regs_dump[o * blocksize], &regs_dump[l * blocksize],blocksize);
									packed_block_map[l] = o;
								}

								for(n=0;n<ymctx->nb_of_frames/blocksize;n++)
								{
									if(block_map[j][n] == l)
									{
										packed_block_map[n] = o;
									}
								}

								if( block_map[j][l] == l)
									o++;
							}

							/*unpacked_regs_dump = malloc( ymctx->nb_of_frames * blocksize );
							memset(unpacked_regs_dump,0,ymctx->nb_of_frames * blocksize);

							for(l=0;l<ymctx->nb_of_frames/blocksize;l++)
							{
								memcpy(&unpacked_regs_dump[l * blocksize], &packed_regs_dump[packed_block_map[l] * blocksize],blocksize);
							}*/

							//fprintf(f2,"\nconst unsigned char ym_reg%.2d[%d];\n",j,o*blocksize);
							col = 0;
							fprintf(f,"\nconst unsigned char ym_reg%.2d[%d]={\n",j,o*blocksize);
							for(l=0;l<o*blocksize;l++)
							{
								fprintf(f,"0x%.2X", packed_regs_dump[l] );

								col = col + 1;
								if(col<16)
								{
									fprintf(f,",");
								}
								else
								{
									col=0;
									if(l!= (o*blocksize)-1)
									{
										fprintf(f,",");
									}

									fprintf(f,"\r\n");
								}

							}
							fprintf(f,"};\n");

							//fprintf(f2,"\nconst unsigned char ym_reg_blockmap%.2d[%d];\n",j,ymctx->nb_of_frames/blocksize);

							col = 0;
							fprintf(f,"\nconst unsigned char ym_reg_blockmap%.2d[%d]={\n",j,ymctx->nb_of_frames/blocksize);
							for(l=0;l<ymctx->nb_of_frames/blocksize;l++)
							{
								fprintf(f,"0x%.2X", packed_block_map[l] );

								col = col + 1;
								if(col<16)
								{
									if(l!= (ymctx->nb_of_frames/blocksize)-1)
										fprintf(f,",");
								}
								else
								{
									col=0;
									if(l!= (ymctx->nb_of_frames/blocksize)-1)
									{
										fprintf(f,",");
									}

									fprintf(f,"\r\n");
								}
							}
							fprintf(f,"};\n");

							//write_binary_file("regs_%d_o.bin",j,regs_dump,ymctx->nb_of_frames);
							//write_binary_file("regs_%d_p.bin",j,unpacked_regs_dump,ymctx->nb_of_frames);

							//free(unpacked_regs_dump);
							free(packed_regs_dump);
							free(packed_block_map);
						}

						for(n=0;n<14;n++)
						{
							count = 0;
							for(l=0;l<ymctx->nb_of_frames/blocksize;l++)
							{
								if(block_map2[n][l]	&& block_map[n][l] != l)
								{
									count++;
									total_count++;
								}
							}
							printf("Reg %.2d : Block %.2d, Duplicate : %.2d  (%d entries)\n",n,ymctx->nb_of_frames/blocksize,count, (ymctx->nb_of_frames/blocksize) - count);

						}

						printf("Total block %.2d (%d bytes), Duplicate : %.2d (%d bytes)\n",(ymctx->nb_of_frames/blocksize)*14,(ymctx->nb_of_frames/blocksize)*14*blocksize,total_count,(total_count*blocksize));

						printf("Total packed size : %d Bytes (blocksize : %d bytes)\n", ((ymctx->nb_of_frames / blocksize) * 14) + (((ymctx->nb_of_frames/blocksize)*14*blocksize) - (total_count*blocksize)) , blocksize );

						for(j=0;j<14;j++)
						{
							free(block_map[j]);
							free(block_map2[j]);
						}

						fprintf(f2,"extern const unsigned char * ym_reg_blockmaps[14];\n");

						fprintf(f,"\nconst unsigned char * ym_reg_blockmaps[14]={\n");
						for(n=0;n<14;n++)
						{
							fprintf(f,"\t(const unsigned char *)&ym_reg_blockmap%.2d",n);
							if(n!=13)
								fprintf(f,",");
							fprintf(f,"\n");
						}
						fprintf(f,"};\n\n");

						fprintf(f2,"extern const unsigned char * ym_reg_blocks[14];\n");
						fprintf(f2,"#define YM_PAGES_NUMBERS %d\n",ymctx->nb_of_frames/blocksize);
						fprintf(f2,"#define YM_PAGES_SIZE %d\n",blocksize);

						fprintf(f,"\nconst unsigned char * ym_reg_blocks[14]={\n");
						for(n=0;n<14;n++)
						{
							fprintf(f,"\t(const unsigned char *)&ym_reg%.2d",n);
							if(n!=13)
								fprintf(f,",");
							fprintf(f,"\n");
						}
						fprintf(f,"};\n");

						fclose(f);
						fclose(f2);
					}
					free(regs_dump);
				}

#ifdef TEST
				blocknum = 0;
				blockpos = 0;
				for(j=0;j<14;j++)
				{
					cur_blockmaps[j] = ym_reg_blocks[j] + (YM_PAGES_SIZE) * *(ym_reg_blockmaps[j]);
				}

				f = fopen("raw_ym2.txt","w+b");

				for(l=0;l<YM_PAGES_NUMBERS*YM_PAGES_SIZE;l++)
				{
					irq_handler();

					fprintf(f," ");

					for(j=0;j<14;j++)
					{
						fprintf(f,"0x%.2X", ymptrbuf[j]);
						//if(j!=13)
							fprintf(f,",");
					}
					fprintf(f,"\r\n");
				}

				fclose(f);
#endif
////
/*
				f = fopen("rle_ym.txt","wb");

				col = 0;
				count = 0;
				for(j=0;j<14;j++)
				{
					rle_len = 0;
					curchar = get_reg( ymctx, i, j );

					for(i=0;i<ymctx->nb_of_frames;i++)
					{
						if(curchar == get_reg( ymctx, i, j ) )
						{
							rle_len++;
							if ( rle_len > 0x7F )
							{
								push_hex(f, &col,  0xFF );
								push_hex(f, &col,  curchar );
								count++;
								rle_len -= 0x7F;
							}
						}
						else
						{
							if(rle_len || (curchar&0x80))
							{
								push_hex(f, &col,  0x80 | rle_len );
								count++;
							}

							push_hex(f, &col,  curchar );
							count++;

							rle_len = 0;
							curchar = get_reg( ymctx, i, j );
						}
					}
				}
				fclose(f);

				printf("YM Size : %d bytes, RLE packed size : %d bytes\n",ymctx->nb_of_frames*14,count);
*/
			}
			else
				printf("Unsupported header...\n");

			return 0;
		}
	}

	return 0;
}

void hxcym_unload( ymcontext * ymctx )
{
	if(ymctx)
	{
		ymctx->tablepos = 0;
		ymctx->patternpos = 0;
		ymctx->patterndelay  = 0;
		ymctx->jump_loop_effect = 0;
		ymctx->bpm = 0;
		ymctx->patternticks = 0;
		ymctx->patterntickse = 0;
		ymctx->patternticksaim = 0;
		ymctx->sampleticksconst = 0;

		ymctx->samplenb = 0;

		ymctx->number_of_channels = 0;

		ymctx->mod_loaded = 0;

		ymctx->last_r_sample = 0;
		ymctx->last_l_sample = 0;
	}
}
