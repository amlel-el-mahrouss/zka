/* -------------------------------------------

	Copyright EL Mahrouss Logic.

	File: Hart.h
	Purpose: POWER hardware threads.

	Revision History:

	14/04/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

struct HAL_HARDWARE_THREAD;

/// @brief hardware thread indentification type.
typedef Kernel::Int32 PPCHartType;

/// @brief Hardware thread information structure.
typedef struct HAL_HARDWARE_THREAD
{
	Kernel::UIntPtr fStartAddress;
	Kernel::UInt8	fPrivleged : 1;
	Kernel::UInt32	fPagkMMFlags;
	PPCHartType		fIdentNumber;
} HAL_HARDWARE_THREAD;

/// @brief Set PC to specific hart.
/// @param hart the hart
/// @param epc the pc.
/// @return
EXTERN_C Kernel::Void hal_set_pc_to_hart(HAL_HARDWARE_THREAD* hart, Kernel::VoidPtr epc);