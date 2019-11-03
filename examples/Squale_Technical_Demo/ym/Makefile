CFLAGS=-O3

OBJECTS=packer/lzh.o rawym_to_rleym.o hxcym.o #packed_ym.o

EXEC=rawym_to_rleym

all: $(OBJECTS)
	$(CC) -o $(EXEC)  $(OBJECTS) $(LIBOBJ) -lc

clean:
	rm -f $(EXEC) $(EXEC).map $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

mrproper: clean
	rm -rf $(EXEC)

.PHONY: clean mrproper
