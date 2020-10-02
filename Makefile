TOOLPATH = ./tools/

MAKE     = $(TOOLPATH)make/make.exe --no-builtin-rules
NASM     = $(TOOLPATH)nasm/nasm.exe
FATIMG   = $(TOOLPATH)fat_imgen/fat_imgen.exe

default : ./bin/cloudos.img

./bin/cloudos.img : ./bin/ipl.bin
	$(FATIMG) -c -F -f ./bin/cloudos.img -s ./bin/ipl.bin

./bin/ipl.bin : ./src/ipl.asm
	cmd /C if not exist bin md bin
	$(NASM) ./src/ipl.asm -o ./bin/ipl.bin