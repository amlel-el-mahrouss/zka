/* -------------------------------------------

	Copyright EL Mahrouss Logic.

------------------------------------------- */

#pragma once

/// @file LPC.h
/// @brief Local Process Codes.
///
#define ErrLocalIsOk()	 (kLastError == kErrorSuccess)
#define ErrLocalFailed() (kLastError != kErrorSuccess)
#define ErrLocal()		 (kLastError)

typedef SInt32 ErrObject;

inline constexpr ErrObject kErrorSuccess			= 0;
inline constexpr ErrObject kErrorExecutable			= 33;
inline constexpr ErrObject kErrorExecutableLib		= 34;
inline constexpr ErrObject kErrorFileNotFound		= 35;
inline constexpr ErrObject kErrorDirectoryNotFound	= 36;
inline constexpr ErrObject kErrorDiskReadOnly		= 37;
inline constexpr ErrObject kErrorDiskIsFull			= 38;
inline constexpr ErrObject kErrorProcessFault		= 39;
inline constexpr ErrObject kErrorSocketHangUp		= 40;
inline constexpr ErrObject kErrorThreadLocalStorage = 41;
inline constexpr ErrObject kErrorMath				= 42;
inline constexpr ErrObject kErrorNoNetwork			= 43;
inline constexpr ErrObject kErrorHeapOutOfMemory	= 44;
inline constexpr ErrObject kErrorNoSuchDisk			= 45;
inline constexpr ErrObject kErrorFileExists			= 46;
inline constexpr ErrObject kErrorFormatFailed		= 47;
inline constexpr ErrObject kErrorNetworkTimeout		= 48;
inline constexpr ErrObject kErrorInternal			= 49;
inline constexpr ErrObject kErrorForkAlreadyExists	= 50;
inline constexpr ErrObject kErrorOutOfTeamSlot		= 51;
inline constexpr ErrObject kErrorHeapNotPresent		= 52;
inline constexpr ErrObject kErrorNoEntrypoint		= 53;
inline constexpr ErrObject kErrorDiskIsCorrupted	= 54;
inline constexpr ErrObject kErrorDisk				= 55;
inline constexpr ErrObject kErrorInvalidData		= 56;
inline constexpr ErrObject kErrorAsync				= 57;
inline constexpr ErrObject kErrorNonBlocking		= 58;
inline constexpr ErrObject kErrorIPC				= 59;
inline constexpr ErrObject kErrorSign				= 60;
inline constexpr ErrObject kErrorInvalidCreds		= 61;
inline constexpr ErrObject kErrorUnimplemented		= 0;

/// @brief The last error reported by the system to the process.
IMPORT_C ErrObject kLastError;