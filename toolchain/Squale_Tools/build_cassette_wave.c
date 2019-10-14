// Squale Tape wave generator
// (c) 2016-2019 HxC2001 / Jean-François DEL NERO

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SAMPLERATE 44100
#define PI 3.1415926535897932384626433832795

#define FREQ_1 1300
#define FREQ_0 2100

double sinoffset;
double OldFrequence;

#pragma pack(1)
typedef struct wav_hdr_ //
{
	char                     RIFF[4];        // RIFF Header
	int                      ChunkSize;      // RIFF Chunk Size
	char                     WAVE[4];        // WAVE Header
	char                     fmt[4];         // FMT header
	int                      Subchunk1Size;  // Size of the fmt chunk
	short int                AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
	short int                NumOfChan;      // Number of channels 1=Mono 2=Stereo
	int                      SamplesPerSec;  // Sampling Frequency in Hz
	int                      bytesPerSec;    // bytes per second */
	short int                blockAlign;     // 2=16-bit mono, 4=16-bit stereo
	short int                bitsPerSample;  // Number of bits per sample
	char                     Subchunk2ID[4]; // "data"  string
	int                      Subchunk2Size;  // Sampled data length
}wav_hdr;
#pragma pack()

void fillwave(int short * BufOut,int Size,double Frequence,int Amplitude)
{
	int i;

	if(Frequence!=0 && Frequence!=OldFrequence)
	{
		//pour 'synchroniser' le changement de frequence...
		sinoffset=(OldFrequence*sinoffset)/Frequence;
		OldFrequence=Frequence;
	}

	for(i=0;i<Size;i++)
	{
		sinoffset++;
		*(BufOut+i) = (int)(sin((double)((double)2*(double)PI*Frequence*sinoffset)/(double)SAMPLERATE)*(double)Amplitude);
	}
}

void fillfile(FILE * f, int usecond, double freq,int vol)
{
	int usecond_done;
	short buffer[SAMPLERATE];

	usecond_done = 0;

	if(usecond < 1000000)
	{
		fillwave(buffer,(SAMPLERATE*usecond)/1000000,freq,vol);
		fwrite(buffer,((SAMPLERATE*usecond)/1000000)*2,1,f);
	}
	else
	{
		while(usecond_done < usecond)
		{
			fillwave(buffer,(SAMPLERATE*usecond)/1000000,freq,vol);
			fwrite(buffer,((SAMPLERATE*usecond)/1000000)*2,1,f);
			usecond_done++;
		}
	}
}

void push_byte(FILE * f, unsigned char byte,int vol)
{
	int i;

	fillfile(f, 833, FREQ_0,vol);

	for(i=0;i<8;i++)
	{
		if(byte&(0x01<<i))
			fillfile(f, 833, FREQ_1,vol);
		else
			fillfile(f, 833, FREQ_0,vol);
	}

	fillfile(f, 833, FREQ_1,vol);
}


int main(int argc, char* argv[])
{
	short buffer[8*1024];
	int samplerate,i,vol;
	unsigned char byte;
	unsigned short sample_value;
	int in_file_size;
	unsigned char waveHeader[50];

	FILE *finput,*foutput;
	wav_hdr wavhdr;

	printf("Squale Tape wave generator v1.1\n");
	printf("(c) 2016-2019 HxC2001 / Jean-François DEL NERO\n");

	if(argc == 3)
	{
		vol =0;
		samplerate = 44100;
		memset((void*)&wavhdr,0,sizeof(wav_hdr));
		memcpy((char*)&wavhdr.RIFF,"RIFF",4);
		wavhdr.ChunkSize = 0;
		memcpy((char*)&wavhdr.WAVE,"WAVE",4);
		memcpy((char*)&wavhdr.fmt,"fmt ",4);
		wavhdr.Subchunk1Size = 16;
		wavhdr.AudioFormat = 1;
		wavhdr.NumOfChan = 1;
		wavhdr.SamplesPerSec = samplerate;
		wavhdr.bytesPerSec = ((samplerate*16)/8);
		wavhdr.blockAlign = (16/8);
		wavhdr.bitsPerSample = 16;
		memcpy((char*)&wavhdr.Subchunk2ID,"data",4);
		wavhdr.Subchunk2Size = 0;

		sinoffset = 0;
		OldFrequence = 1200;

		finput = fopen(argv[1],"r+b");
		fseek(finput,0,SEEK_END);
		in_file_size = ftell(finput);
		fseek(finput,0,SEEK_SET);

		foutput = fopen(argv[2],"w+b");
		if( foutput && finput)
		{
			printf("Create wave file...\n");

			fwrite((void*)&wavhdr,sizeof(wav_hdr),1,foutput);

			// Header
			for(i=0;i<30000;i++)
			{
				fillfile(foutput, 100, FREQ_1,vol);
				if (vol<26000)
					vol=vol+6;
			}

			// start adr
			push_byte(foutput, 0x00,vol);
			push_byte(foutput, 0x00,vol);

			// end adr
			push_byte(foutput, (in_file_size>>8)&0xFF,vol);
			push_byte(foutput, (in_file_size)&0xFF,vol);

			push_byte(foutput, 0x00,vol);
			push_byte(foutput, 0x00,vol);

			for(i=0;i<in_file_size;i++)
			{
				if(fread(&byte,1,1,finput))
				{
					push_byte(foutput, byte,vol);
				}
			}

			push_byte(foutput, 0x00,vol);

			// End
			for(i=0;i<1500;i++)
			{
				fillfile(foutput, 1000, FREQ_1,vol);
			}

			// fadeout
			while(vol > 0)
			{
				fillfile(foutput, 100, FREQ_1,vol);
				vol=vol-6;
			}

			// update wave header
			wavhdr.ChunkSize = ftell(foutput)-8;
			wavhdr.Subchunk2Size = ftell(foutput) - sizeof(wav_hdr);
			fseek(foutput,0,SEEK_SET);
			fwrite(&wavhdr,sizeof(wav_hdr),1,foutput);

			fclose( foutput );
			fclose( finput );
		}
	}
}
