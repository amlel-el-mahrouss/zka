/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <FirmwareKit/EFI/API.hxx>
#include <FirmwareKit/EFI/EFI.hxx>
#include <FirmwareKit/Handover.hxx>
#include <BootKit/Support.hxx>
#include <KernelKit/MSDOS.hxx>
#include <KernelKit/PE.hxx>

#ifdef __STANDALONE__

/// @brief memset definition in C++.
/// @param dst destination pointer.
/// @param byte value to fill in.
/// @param len length of of src.
EXTERN_C VoidPtr memset(void* dst, int byte, long long unsigned int len)
{
	for (size_t i = 0UL; i < len; ++i)
	{
		((int*)dst)[i] = byte;
	}

	return dst;
}

/// @brief memcpy definition in C++.
/// @param dst destination pointer.
/// @param  src source pointer.
/// @param len length of of src.
EXTERN_C VoidPtr memcpy(void* dst, const void* src, long long unsigned int len)
{
	for (size_t i = 0UL; i < len; ++i)
	{
		((int*)dst)[i] = ((int*)src)[i];
	}

	return dst;
}

/// @brief strlen definition in C++.
EXTERN_C size_t strlen(const char* whatToCheck)
{
	SizeT len = 0;

	while (whatToCheck[len] != 0)
	{
		++len;
	}

	return len;
}

/// @brief strcmp definition in C++.
EXTERN_C int strcmp(const char* whatToCheck, const char* whatToCheckRight)
{
	if (!whatToCheck || *whatToCheck == 0)
		return 0;

	SizeT len = 0;

	while (whatToCheck[len] == whatToCheckRight[len])
	{
		if (whatToCheck[len] == 0)
			return 0;

		++len;
	}

	return whatToCheck[len] == whatToCheckRight[len] ? 0 : len;
}

/// @brief somthing specific to the Microsoft's ABI, When the stack grows too big.
EXTERN_C void ___chkstk_ms(void)
{
}

#endif