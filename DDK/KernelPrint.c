/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	Purpose: Kernel Text I/O.

------------------------------------------- */

#include <DDK/KernelPrint.h>

DK_EXTERN void kernelPrintChar(const char ch)
{
	kernelCall("WriteCharacter", 1, ch, 1);
}

/// @brief print string to UART.
/// @param message UART to transmit.
DK_EXTERN void kernelPrintStr(const char* message)
{
	if (!message)
		return;
	if (*message == 0)
		return;

	size_t index = 0;
	size_t len	 = kernelStringLength(message);

	while (index < len)
	{
		kernelPrintChar(message[index]);
		++index;
	}
}