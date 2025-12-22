CC=gcc

UNAME := $(shell uname)

DEBUG ?= 0

ifeq ($(DEBUG), 1)
	CFLAGS=-O0 $(INCLUDES) -Wall -g
	LDFLAGS= -lm #-lasan
else
	CFLAGS=-O3 $(INCLUDES) -Wall
	LDFLAGS= -lm -s
endif

EXEC=demod

ifeq ($(TARGET),)
	ifeq ($(UNAME), Darwin)
		TARGET = Darwin
	endif

	ifeq ($(UNAME), Linux)
		TARGET = Linux
	endif
endif

ifeq ($(TARGET), Linux)
	CFLAGS += -Wl,-Map,foo.map
	LDFLAGS += -Wl,-rpath=.
endif

ifeq ($(TARGET), mingw32)
	CC=i686-w64-mingw32-gcc
	RESC=i686-w64-mingw32-windres
	LDFLAGS += -static-libgcc -lws2_32 -lpthread -Wl,-Map=output.map --static
	CFLAGS +=
	EXEC=demod.exe
endif

ifeq ($(TARGET), mingw64)
	CC=x86_64-w64-mingw32-gcc
	RESC=x86_64-w64-mingw32-windres
	LDFLAGS += -static-libgcc -lws2_32 -lpthread -Wl,-Map=output.map --static
	CFLAGS +=
	EXEC=demod.exe
endif

ifeq ($(findstring CYGWIN,$(shell uname)),CYGWIN)
	LDFLAGS= -shared -lc
	EXEC=demod.exe
endif

ifeq ($(TARGET), Darwin)
	MACOSX_ARCH ?= -arch arm64 -arch x86_64
	MACOSX_MIN_VER ?= 10.9
	CFLAGS += ${MACOSX_ARCH} -mmacosx-version-min=${MACOSX_MIN_VER}
	LDFLAGS += -lc ${MACOSX_ARCH} -current_version 2.0 -install_name @executable_path/demod -mmacosx-version-min=${MACOSX_MIN_VER}
	EXEC=demod
endif

all: $(EXEC)

$(EXEC):  demod.o fifo.o modem.o
	$(CC) -o $@    $^ $(LDFLAGS)

demod.o: demod.c
	$(CC) -o $@ -c $< $(CFLAGS)

modem.o: modem.c
	$(CC) -o $@ -c $< $(CFLAGS)

fifo.o: fifo.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o
	rm -rf *.so
	rm -rf $(EXEC) *.exe

mrproper: clean
	rm -rf $(EXEC)

.PHONY: clean mrproper

