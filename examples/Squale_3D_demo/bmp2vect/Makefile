CFLAGS = -O3 -Wall
LDFLAGS =

EXEC=bmp2vect

all: $(EXEC)

bmp2vect: bmp_file.o main.o
	$(CC) -o $@    $^ $(LDFLAGS)

main.o: main.c
	$(CC) -o $@ -c $< $(CFLAGS)

bmp_file.o: bmp_file.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)

.PHONY: clean mrproper
