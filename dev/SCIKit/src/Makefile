##################################################
# (c) Amlal EL Mahrouss, all rights reserved.
# This is the bootloader makefile.
##################################################

ASM=nasm
FLAGS=-f win64

.PHONY: syscall_unit
syscall_unit:
	$(ASM) $(FLAGS) SysCallDispatcher.asm -o SysCallDispatcher.o
