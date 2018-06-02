CC:=i686-elf-gcc
RM:=/bin/rm -f

CFLAGS:=-m32 -ffreestanding -g -gstabs -fbuiltin -Wall -Wextra -nostdlib

.PHONY: all clean

all: bin/exe

bin/exe: src/main.o src/linker.ld
	$(CC) -T src/linker.ld -o $@ $(CFLAGS) src/main.o -lgcc

src/main.o: src/main.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS)

clean:
	$(RM) src/main.o bin/exe
