TOOLPATH = ./tools/

MAKE     = $(TOOLPATH)make/make.exe --no-builtin-rules
NASM     = $(TOOLPATH)nasm/nasm.exe
FATIMG   = $(TOOLPATH)fat_imgen/fat_imgen.exe
LD       = $(TOOLPATH)i686-elf-tools-windows/bin/i686-elf-ld.exe

default : ./bin/cloudos.img

./bin/cloudos.img : ./bin/ipl.bin ./bin/boot.bin
	$(FATIMG) -c -F -f ./bin/cloudos.img -s ./bin/ipl.bin
	$(FATIMG) -m -F -f ./bin/cloudos.img -i ./bin/boot.bin -n BOOT.BIN

./bin/ipl.bin : ./src/ipl.asm
	cmd /C if not exist bin md bin
	$(NASM) -i./src/include/ ./src/ipl.asm -o ./bin/ipl.bin

./bin/boot.bin : ./bin/boot.o ./src/boot_linker.ld
	cmd /C if not exist bin md bin
	$(LD)  -T ./src/boot_linker.ld -nostdlib ./bin/boot.o -o ./bin/boot.bin

./bin/boot.o : ./src/boot.asm
	cmd /C if not exist bin md bin
	$(NASM) -i./src/include/ ./src/boot.asm -f elf -o ./bin/boot.o

.PHONY: clean
clean:
	cmd /C del /Q /S .\bin
