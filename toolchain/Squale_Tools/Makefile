CC=gcc
CFLAGS=-O3
LDFLAGS=-lc -lm
EXEC=build_cassette_wave build_cartridge

all: $(EXEC)

build_cassette_wave: build_cassette_wave.o
	$(CC) -o $@    $^ $(LDFLAGS)

build_cassette_wave.o: build_cassette_wave.c
	$(CC) -o $@ -c $< $(CFLAGS)

build_cartridge: build_cartridge.o
	$(CC) -o $@    $^ $(LDFLAGS)

build_cartridge.o: build_cartridge.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)

.PHONY: clean mrproper

