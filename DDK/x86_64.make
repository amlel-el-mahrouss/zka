##################################################
# (C) ZKA Technologies, all rights reserved.
# This is the DDK's makefile.
##################################################

CC=x86_64-w64-mingw32-gcc
INC=-I./ -I../
FLAGS=-ffreestanding -shared -std=c17 -std=c++20 -fno-rtti -fno-exceptions -D__DDK_AMD64__ -Wl,--subsystem=17
VERSION=-DcDDKVersionLowest=1 -DcDDKVersionHighest=1
OUTPUT=libDDK.dll

.PHONY: all
all:
	$(CC) $(INC) $(FLAGS) $(VERSION) $(wildcard *.c) $(wildcard *.S) $(wildcard *.cxx) -o $(OUTPUT)