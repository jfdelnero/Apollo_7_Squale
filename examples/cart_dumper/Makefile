OUTPUT=cart_dumper
OBJECTS=$(OUTPUT).bin
CFLAGS=-Os

PREFIX=m6809-unknown

CC=$(PREFIX)-gcc
AS=$(PREFIX)-as
LD=$(PREFIX)-ld

all: $(OBJECTS)
	$(CUR_FOLDER)/Squale_Tools/build_cartridge $(CUR_FOLDER)/Squale_entry/cartridge_loader.bin $(OUTPUT).bin $(OUTPUT).rom
	$(CUR_FOLDER)/Squale_Tools/build_cassette_wave $(OUTPUT).bin $(OUTPUT).wav

cart_dumper.bin: cart_dumper.o
	$(LD) -Ttext 0x0000 --oformat=raw -o cart_dumper.bin cart_dumper.o

clean:
	rm -f $(OBJECTS) *.o *.rom *.wav

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $< -o $@
