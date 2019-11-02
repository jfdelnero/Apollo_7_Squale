// Squale cartridge ROM image builder
// (c) 2016-2019 HxC2001 / Jean-François DEL NERO

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
	unsigned char * memory;
	FILE * f;
	int size;

	printf("Squale cartridge generator v1.1\n");
	printf("(c) 2016-2019 HxC2001 / Jean-François DEL NERO\n");

	memory = NULL;
	f = NULL;

	if(argc == 4)
	{
		memory = malloc(64*1024);
		if(memory)
		{
			memset(memory,0xFF,64*1024);

			f = fopen(argv[1],"r+b"); //Loader
			if(f)
			{
				fseek(f,0,SEEK_END);
				size = ftell(f);
				fseek(f,0,SEEK_SET);

				if( fread( memory,size,1,f) != 1 )
				{
					printf("read %s error...\n",argv[1]);
					goto error;
				}

				fclose(f);
				f = NULL;
				printf("%s loaded\n",argv[1]);
			}
			else
			{
				printf("Can't open %s !\n",argv[1]);
				goto error;
			}

			f = fopen(argv[2],"r+b"); // program
			if(f)
			{
				fseek(f,0,SEEK_END);
				size = ftell(f);
				fseek(f,0x0,SEEK_SET);

				if( fread( &memory[0x100],size,1,f) != 1 )
				{
					printf("read %s error...\n",argv[2]);
					goto error;
				}

				fclose(f);
				f = NULL;
				printf("%s loaded\n",argv[2]);
			}
			else
			{
				printf("Can't open %s !\n",argv[2]);
				goto error;
			}

			f = fopen(argv[3],"w+b"); // Output rom
			if(f)
			{
				if( (size + 0x100) <= 32*1024 )
					fwrite(memory,32*1024,1,f);
				else
					fwrite(memory,64*1024,1,f);

				fclose(f);
				f = NULL;
				printf("%s created\n",argv[3]);
			}
			else
			{
				printf("Can't create %s !\n",argv[3]);
				goto error;
			}

			free(memory);
		}
	}

	return 0;

error:
	if(memory)
		free(memory);

	if(f)
		fclose(f);

	return -1;
}
