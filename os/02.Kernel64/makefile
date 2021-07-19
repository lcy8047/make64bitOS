# set build environment & rule
NASM64 = nasm -f elf64
GCC64 = gcc -g -c -m64 -ffreestanding
LD64 = ld -melf_x86_64 -T ../elf_x86_64.x -nostdlib -e main -Ttext 0x200000
OBJCOPY64 = objcopy -j .text -j .data -j .rodata -j .bss -S -O binary

OBJECTDIR = temp
SOURCEDIR = source

# build rule
all: prepare Kernel64.bin

prepare:
	mkdir -p $(OBJECTDIR)

dep:
	@echo === Make Dependency File ===
	make -C $(OBJECTDIR) -f ../makefile InternalDependency
	@echo === Dependency Search Complete ===

ExecuteInternalBuild: dep
	make -C $(OBJECTDIR) -f ../makefile Kernel64.elf

Kernel64.bin: ExecuteInternalBuild
	$(OBJCOPY64) $(OBJECTDIR)/Kernel64.elf $@

clean:
	rm -f *.bin
	rm -f $(OBJECTDIR)/*.*


# execute in temp 
ENTRYPOINTSRCFILE = ../$(SOURCEDIR)/EntryPoint.s
ENTRYPOINTOBJFILE = EntryPoint.o

CSRCFILES = $(wildcard ../$(SOURCEDIR)/*.c)
ASSEMBLYSRCFILES = $(wildcard ../$(SOURCEDIR)/*.asm)
COBJFILES = $(notdir $(patsubst %.c,%.o,$(CSRCFILES)))
ASSEMBLYOBJFILES = $(notdir $(patsubst %.asm,%.o,$(ASSEMBLYSRCFILES)))

$(ENTRYPOINTOBJFILE): $(ENTRYPOINTSRCFILE)
	$(NASM64) -o $@ $<

%.o: ../$(SOURCEDIR)/%.c
	$(GCC64) -c $<

%.o: ../$(SOURCEDIR)/%.asm
	$(NASM64) -o $@ $<

InternalDependency:
	$(GCC64) -MM $(CSRCFILES) > Dependency.dep

Kernel64.elf: $(ENTRYPOINTOBJFILE) $(COBJFILES) $(ASSEMBLYOBJFILES)
	$(LD64) -o $@ $^

ifeq (Dependency.dep, $(wildcard Dependency.dep))
include Dependency.dep
endif
