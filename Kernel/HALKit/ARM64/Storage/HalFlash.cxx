/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <NewKit/Defines.hpp>
#include <ArchKit/ArchKit.hpp>

/// @file Flash.cxx
/// @brief Flash memory builtin.

#ifdef __FLASH_MEM__

#define cMaxFlash (4U)

namespace NewOS
{
	/// /:\\BRIDGE\\FLSH\\1
	constexpr auto cFlashBridgeMagic	= "FLSH";
	constexpr auto cFlashBridgeRevision = 1;

	STATIC const Boolean kFlashEnabled			   = No;
	STATIC SizeT		 kFlashSize[cMaxFlash]	   = {};
	STATIC SizeT		 kFlashSectorSz[cMaxFlash] = {};

	/// @brief Enable flash memory builtin.
	STATIC Void drv_enable_flash(Int32 slot);

	/// @brief Disable flash memory builtin.
	STATIC Void drv_disable_flash(Int32 slot);

	/// @brief get sector count.
	/// @return drive sector count.
	SizeT drv_std_get_sector_count(Int32 slot)
	{
		if (slot > cMaxFlash)
			return 0;

		return kFlashSectorSz[slot];
	}

	/// @brief get device size.
	/// @return drive size
	SizeT drv_std_get_drv_size(Int32 slot)
	{
		if (slot > cMaxFlash)
			return 0;

		return kFlashSize[slot];
	}

	/// @brief Enable flash memory at slot.
	STATIC Void drv_enable_flash(Int32 arg)
	{
		kcout << "newoskrnl: enabled hardware.\r";
	}

	/// @brief Disable flash memory at slot.
	STATIC Void drv_disable_flash(Int32 arg)
	{
		kcout << "newoskrnl: disabled hardware.\r";
	}
} // namespace NewOS

#endif // if __FLASH_MEM__ (Bridge)