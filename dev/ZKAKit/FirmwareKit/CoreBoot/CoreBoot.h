/* -------------------------------------------

	Copyright (C) 2024, ELMH Group, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

namespace Firmware::Details::CoreBoot
{
	using namespace Kernel;

	struct LEHeader;

	/// @brief Linear Executable Header
	/// @author ELMH Group
	struct ATTRIBUTE(aligned(4)) LEHeader
	{
		const Char	 fMagic[2];		// magic number
		const Char	 fName[10];		// operating system name
		const UInt32 fRevision;		// firmware revision
		const UInt32 fStartAddress; // start address (master/slave(s) thread)

#ifdef ZKA_IS_EXTENDED_COREBOOT
		const UIntPtr fMasterStructure;		   // master structure for MP/PM and device tree and such (ARM)
		const UIntPtr fMasterStructureVersion; // master structure version.
#endif
	};
} // namespace Firmware::Details::CoreBoot