MAKE     = make.exe --no-builtin-rules
NASM     = nasm.exe
FATIMG   = fat_imgen.exe
LD       = i686-elf-ld.exe
GCC      = i686-elf-gcc.exe

default : ./bin/cloudos.img

./bin/cloudos.img : ./bin/ipl.bin ./bin/boot.bin ./src/font.fnt
	$(FATIMG) -c -F -f ./bin/cloudos.img -s ./bin/ipl.bin
	$(FATIMG) -m -F -f ./bin/cloudos.img -i ./bin/boot.bin -n BOOT.BIN
	$(FATIMG) -m -F -f ./bin/cloudos.img -i ./src/font.fnt -n FONT.FNT

./bin/ipl.bin : ./src/ipl.asm
	cmd /C if not exist bin md bin
	$(NASM) -i./src/include/ ./src/ipl.asm -o ./bin/ipl.bin

./bin/boot.bin : ./bin/boot.o ./bin/boot_c.o ./src/boot_linker.ld
	cmd /C if not exist bin md bin
	$(LD) -T ./src/boot_linker.ld -nostdlib ./bin/boot.o ./bin/boot_c.o -o ./bin/boot.bin

./bin/boot.o : ./src/boot.asm
	cmd /C if not exist bin md bin
	$(NASM) -i./src/include/ ./src/boot.asm -f elf -o ./bin/boot.o

./bin/boot_c.o : ./src/boot.c
	cmd /C if not exist bin md bin
	$(GCC) -c ./src/boot.c -o ./bin/boot_c.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

.PHONY: clean
clean:
	cmd /C del /Q /S .\bin
