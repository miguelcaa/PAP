.PHONY: all boot kernel os-image run clean

CC=gcc
LD=ld -m elf_i386
CFLAGS=-ffreestanding -m32 -nostdlib -nostartfiles -fno-pic -fno-stack-protector -O2

all: os-image

boot:
	mkdir -p build
	nasm boot/boot.asm -f bin -o build/boot.bin

kernel:
kernel:
	mkdir -p build
	nasm kernel/kernel_entry.asm -f elf -o build/kernel_entry.o
	nasm kernel/isr.asm -f elf -o build/isr.o
	$(CC) $(CFLAGS) -c kernel/kernel.c -o build/kernel.o
	$(CC) $(CFLAGS) -c kernel/vga.c -o build/vga.o
	$(CC) $(CFLAGS) -c kernel/keyboard.c -o build/keyboard.o
	$(CC) $(CFLAGS) -c kernel/idt.c -o build/idt.o
	$(CC) $(CFLAGS) -c kernel/pic.c -o build/pic.o

	$(LD) -T kernel/linker.ld -o build/kernel.elf \
		build/kernel_entry.o build/isr.o build/kernel.o build/vga.o build/keyboard.o build/idt.o build/pic.o

	objcopy -O binary build/kernel.elf build/kernel.bin

os-image: boot kernel
	mkdir -p build
	dd if=/dev/zero of=build/os-image.bin bs=512 count=2880 status=none
	dd if=build/boot.bin of=build/os-image.bin conv=notrunc bs=512 seek=0 status=none
	dd if=build/kernel.bin of=build/os-image.bin conv=notrunc bs=512 seek=1 status=none

run:
	qemu-system-i386 -drive format=raw,file=build/os-image.bin

clean:
	rm -rf build

