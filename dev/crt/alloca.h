/* -------------------------------------------

	Copyright ZKA Web Services Co.

	File: alloca.h
	Purpose: Stack allocation functions.

------------------------------------------- */

#pragma once

#include <crt/defines.h>

inline ptr_type __rt_alloca(size_type sz)
{
	return __builtin_alloca(sz);
}

#define alloca __rt_alloca