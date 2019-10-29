OUTPUT=Squale_3D_demo
OBJECTS=images/mo5_logo.o images/apollo_7_logo.o images/squale_logo.o images/footpage.o music/warp_5.o demo.o \
        demo_segments_part.o demo_3d_part.o demo_startuplogo_part.o demo_presentation_part.o demo_squares_part.o \
        demo_dotschaos_part.o demo_jump_part.o vid_asm_func.o progymregs_asm_func.o deuxd_func.o troisd_func.o

CFLAGS=-O2 -I$(CUR_FOLDER)/include

PREFIX=m6809-unknown

CC=$(PREFIX)-gcc
AS=$(PREFIX)-as
LD=$(PREFIX)-ld

LIBPATH=$(CUR_FOLDER)/build/m6809-unknown/libgcc
LIBOBJ=$(LIBPATH)/_muldi3.o $(LIBPATH)/_mulhi3.o $(LIBPATH)/_divhi3.o $(LIBPATH)/_seuclid.o $(LIBPATH)/_euclid.o

.PRECIOUS: images/%.c music/%.c

all: squalebmp2vect hxcym $(OBJECTS)
	$(LD) -o $(OUTPUT).bin  $(OBJECTS) $(LIBOBJ) $(CUR_FOLDER)/Squale_entry/crt0.o -Map=mapfile.map --script=$(CUR_FOLDER)/Squale_entry/squale.link
	$(CUR_FOLDER)/Squale_Tools/build_cartridge $(CUR_FOLDER)/Squale_entry/cartridge_loader.bin $(OUTPUT).bin $(OUTPUT).rom
	$(CUR_FOLDER)/Squale_Tools/build_cassette_wave $(OUTPUT).bin $(OUTPUT).wav
	
squalebmp2vect:
	cd bmp2vect && $(MAKE)

hxcym:
	cd ym && $(MAKE)

images/%.o: images/%.c
	$(CC) $(CFLAGS) -c $< -o $@

images/%.c: images/%.bmp
	cd images && ../bmp2vect/bmp2vect -file:`basename $<`  -imagename:`basename $< .bmp`

music/%.c: music/%.ym
	cd music && ../ym/rawym_to_rleym `basename $<`
	cd music && mv packed_ym.c `basename $< .ym`.c
	cd music && mv packed_ym.h `basename $< .ym`.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $(CFLAGS) -S $< -o $@.S

%.o: %.s
	$(AS) $< -o $@

clean:
	rm -f $(OUTPUT).bin $(OUTPUT).rom $(OUTPUT).map $(OBJECTS) $(OBJECTS).S
	cd bmp2vect && $(MAKE) clean
	cd ym && $(MAKE) clean

mrproper: clean
	rm -f images/*.c images/*.h music/*.c music/*.h  music/*.o music/*.o.S *.o.S *.o *.wav *.map
