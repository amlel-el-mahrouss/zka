;; /*
;; *	========================================================
;; *
;; *	HCore
;; * 	Copyright Mahrouss Logic, all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

;; Global symbol of this unit
[global MainLong]
[global MainUnsupported]

%define kTypeKernel 100
%define kArchAmd64 122

section .NewBoot

HandoverMagic: dq 0xBAD55
HandoverType: dw kTypeKernel
HandoverArch: dw kArchAmd64
;; This NewBootStart points to Main.
HandoverStart: dq Main

section .text

global Main
extern RuntimeMain

;; Just a simple setup, we'd also need to tell some before
Main:
    push rcx
    call RuntimeMain
    pop rcx
L0:
    cli
    hlt
    jmp $