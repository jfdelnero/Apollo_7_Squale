OBJECTS=cartridge_loader.o cassette_loader.o crt0.o cartridge_loader.bin
CFLAGS=-Os

PREFIX=m6809-unknown

CC=$(PREFIX)-gcc
AS=$(PREFIX)-as
LD=$(PREFIX)-ld

all: $(OBJECTS)

cartridge_loader.bin:
	$(LD) -Ttext 0xC100 --oformat=raw -o cartridge_loader.bin cartridge_loader.o

clean:
	rm -f $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $< -o $@
