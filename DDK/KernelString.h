/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	Purpose: Kernel Strings.

------------------------------------------- */

#pragma once

#include <DDK/KernelStd.h>

/// @brief DDK equivalent of POSIX's string.h.

DK_EXTERN size_t kernelStringLength(const char* str);
DK_EXTERN int	 kernelStringCopy(char* dst, const char* src, size_t len);