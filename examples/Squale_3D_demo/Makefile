OUTPUT=Squale_3D_demo
OBJECTS=demo.o vid_asm_func.o progymregs_asm_func.o ym_music.o deuxd_func.o troisd_func.o
CFLAGS=-O2 -I$(CUR_FOLDER)/include

PREFIX=m6809-unknown

CC=$(PREFIX)-gcc
AS=$(PREFIX)-as
LD=$(PREFIX)-ld

LIBPATH=$(CUR_FOLDER)/build/m6809-unknown/libgcc
LIBOBJ=$(LIBPATH)/_muldi3.o $(LIBPATH)/_mulhi3.o $(LIBPATH)/_divhi3.o $(LIBPATH)/_seuclid.o $(LIBPATH)/_euclid.o

all: $(OBJECTS)
	$(LD) -o $(OUTPUT).bin  $(OBJECTS) $(LIBOBJ) $(CUR_FOLDER)/Squale_entry/crt0.o -Map=mapfile.map --script=$(CUR_FOLDER)/Squale_entry/squale.link
	$(CUR_FOLDER)/Squale_Tools/build_cartridge $(CUR_FOLDER)/Squale_entry/cartridge_loader.bin $(OUTPUT).bin $(OUTPUT).rom
	$(CUR_FOLDER)/Squale_Tools/build_cassette_wave $(OUTPUT).bin $(OUTPUT).wav

clean:
	rm -f $(OUTPUT).bin $(OUTPUT).rom $(OUTPUT).map $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $(CFLAGS) -S $< -o $@.S

%.o: %.s
	$(AS) $< -o $@
