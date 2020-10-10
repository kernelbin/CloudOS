TOOLPATH = ./tools/

MAKE     = $(TOOLPATH)make/make.exe --no-builtin-rules
NASM     = $(TOOLPATH)nasm/nasm.exe
FATIMG   = $(TOOLPATH)fat_imgen/fat_imgen.exe

default : ./bin/cloudos.img

./bin/cloudos.img : ./bin/ipl.bin ./bin/boot.bin
	$(FATIMG) -c -F -f ./bin/cloudos.img -s ./bin/ipl.bin
	$(FATIMG) -m -F -f ./bin/cloudos.img -i ./bin/boot.bin -n BOOT.BIN

./bin/ipl.bin : ./src/ipl.asm
	cmd /C if not exist bin md bin
	$(NASM) -i./src/include/ ./src/ipl.asm -o ./bin/ipl.bin

./bin/boot.bin : ./src/boot.asm
	cmd /C if not exist bin md bin
	$(NASM) ./src/boot.asm -o ./bin/boot.bin

.PHONY: clean
clean:
	del /Q /S .\bin