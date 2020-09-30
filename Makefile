TOOLPATH = ./tools/

MAKE     = $(TOOLPATH)make/make.exe --no-builtin-rules
NASM     = $(TOOLPATH)nasm/nasm.exe

default :
	$(MAKE)    ./bin/cloudos.img


./bin/cloudos.img : ./src/ipl.asm
	if not exist bin md bin
	$(NASM)    ./src/ipl.asm -o ./bin/ipl.img
