##################################################
# (C) SoftwareLabs, all rights reserved.
# This is the sample driver makefile.
##################################################

CC_GNU=x86_64-w64-mingw32-gcc
LD_GNU=x86_64-w64-mingw32-ld

WINDRES=x86_64-w64-mingw32-windres

ADD_FILE=touch
COPY=cp
HTTP_GET=wget

LD_FLAGS=-e __ImageStart --subsystem=17

OBJ=*.o


REM=rm
REM_FLAG=-f

FLAG_ASM=-f win64
FLAG_GNU=-fshort-wchar -D__EFI_x86_64__ -mgeneral-regs-only -mno-red-zone -D__KERNEL__ -DEFI_FUNCTION_WRAPPER -I../ -I../../ -I./ -c -ffreestanding -D__HAVE_MAHROUSS_APIS__ -D__MAHROUSS__ -D__BOOTLOADER__ -I./

.PHONY: invalid-recipe
invalid-recipe:
	@echo "invalid-recipe: Use make all instead."

.PHONY: all
all: compile-amd64
	$(LD_GNU) $(OBJ) $(LD_FLAGS) -o MahroussUpdate.exe
	cp MahroussUpdate.exe ../../Root/Boot/MahroussUpdate.exe

ifneq ($(DEBUG_SUPPORT), )
DEBUG =  -D__DEBUG__
endif

.PHONY: compile-amd64
compile-amd64:
	$(WINDRES) DriverRsrc.rsrc -O coff -o DriverRsrc.o
	$(CC_GNU) $(FLAG_GNU) -std=c17  $(DEBUG)  $(wildcard *.c) $(wildcard ../../DriverKit/*.c) $(wildcard ../../DriverKit/*.S)
	$(CC_GNU) $(FLAG_GNU) -std=c++17 -fno-rtti -fno-exceptions $(DEBUG) $(wildcard *.cc) $(wildcard ../../DriverKit/*.cc)

.PHONY: clean
clean:
	$(REM) $(REM_FLAG) $(OBJ) MahroussUpdate.exe

.PHONY: help
help:
	@echo "=== HELP ==="
	@echo "clean: Clean driver."
	@echo "compile-amd64: Build driver."