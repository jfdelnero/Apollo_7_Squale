/*
 Squale Tape output demodulator

 Used to extract Squale cartridge dump.

 (C) 2O25 Jean-François DEL NERO
*/

#include <stdio.h>
#include <stdint.h>

#ifdef WIN32
#include <windows.h>
#endif

#include "fifo.h"
#include "modem.h"

#define NBSMP DEFAULT_SOUND_BUFFER_SIZE

int main(int argc, char* argv[])
{
	short buf[NBSMP];
	unsigned char c;
	FILE * f_in;
	FILE * f_out;
	uint16_t checksum;
	int totalbytes;

	modem_ctx mdm_ctx;

	mdm_init(&mdm_ctx);

	// Expect a raw file with this format :
	// mono, little endian 16 bits signed samples.
	// 44100 Hz sample rate.
	// Supported modulation : V22 1200 baud, 8 bits + even parity data.

	f_in = fopen("cart.raw","rb");
	if(!f_in)
	{
		printf("Error : Can't read the raw file (cart.raw)\n");
		return -1;
	}

	f_out = fopen("out.bin","wb");
	if(!f_out)
	{
		printf("Error : Can't create the output file. (out.bin)\n");
		return -2;
	}

	totalbytes = 0;
	checksum = 0x0000;
	while(!feof(f_in))
	{
		if( fread(&buf,sizeof(buf),1,f_in) == 1 )
		{
			mdm_demodulate(&mdm_ctx, &mdm_ctx.demodulators[1],(short *)&buf, NBSMP);

			while( !is_fifo_empty(&mdm_ctx.rx_fifo[1]) )
			{
				pop_from_fifo(&mdm_ctx.rx_fifo[1], &c);
				fwrite(&c,1,1,f_out);
				checksum += c;
				totalbytes++;
			}
		}
	}

	printf("\nTotal bytes : %d (0x%.4X)\nChecksum : 0x%.4X\n",totalbytes,totalbytes,checksum);

	fclose(f_in);
	fclose(f_out);

	return 0;
}
