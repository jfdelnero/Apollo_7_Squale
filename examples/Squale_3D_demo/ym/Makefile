OUTPUT=rawym_to_rleym 
OBJECTS=packer/lzh.o rawym_to_rleym.o hxcym.o #packed_ym.o
CFLAGS=-O2 -g

PREFIX=

CC=gcc
LD=ld

all: $(OBJECTS)
	$(CC) -o $(OUTPUT)  $(OBJECTS) $(LIBOBJ) -lc

clean:
	rm -f $(OUTPUT) $(OUTPUT).map $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

