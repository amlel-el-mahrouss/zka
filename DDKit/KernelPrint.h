/* -------------------------------------------

	Copyright ZKA Technologies

	Purpose: DDK Text I/O.

------------------------------------------- */

#pragma once

#include <DDKit/KernelString.h>

/// @brief print character into UART.
DK_EXTERN void kernelPrintChar(const char ch);

/// @brief print string to UART.
/// @param message string to transmit to UART.
DK_EXTERN void kernelPrintStr(const char* message);