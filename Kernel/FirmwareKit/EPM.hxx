/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

/**
    @brief The Explicit Partition Map scheme.
*/

#ifndef __PARTITION_MAP__
#define __PARTITION_MAP__

#include <NewKit/Defines.hpp>

#define kEPMNameLength		 32
#define kEPMFilesystemLength 16
#define kEPMMagicLength		 5

/* The first 0 > 128 addresses of a disk contains these headers. */

/// @brief EPM GUID block.
typedef struct BlockGUID
{
	NewOS::UInt32 Data1;
	NewOS::UInt16 Data2;
	NewOS::UInt16 Data3;
	NewOS::UInt8  Data4[8];
} BlockGUID;

/**
 * @brief The EPM bootloader block.
 * @note NumBlock and LbaStart are ignored on UEFI.
 */
struct PACKED BootBlock
{
	NewOS::Char	 Magic[kEPMMagicLength];
	NewOS::Char	 Name[kEPMNameLength];
	BlockGUID	 Uuid;
	NewOS::Int32 Version;
	NewOS::Int64 NumBlocks;
	NewOS::Int64 SectorSz;
	NewOS::Int64 LbaStart;
};

/**
 * @brief The EPM partition block.
 * used to describe a partition inside a media, doesn't exist on uefi.
 */
struct PACKED PartitionBlock
{
	NewOS::Char	 Name[kEPMNameLength];
	NewOS::Int32 Version;
	NewOS::Int64 LbaEnd;
	NewOS::Int64 SectorSz;
	NewOS::Int64 LbaStart;
	NewOS::Int16 Kind;
	NewOS::Int32 FsVersion;
	NewOS::Char	 Fs[kEPMFilesystemLength]; /* NewFS, ffs2... */
};

/* @brief AMD64 magic for EPM */
#define kEPMMagic86 "EPMAM"

/* @brief RISC-V magic for EPM */
#define kEPMMagicRISCV "EPMRV"

/* @brief ARM magic for EPM */
#define kEPMMagicARM "EPMAR"

/* @brief 64x0 magic for EPM */
#define kEPMMagic64k "EPM64"

/* @brief 32x0 magic for EPM */

#define kEPMMagic32k "EPM32"

/* @brief POWER magic for EPM */

#define kEPMMagicPPC "EPMPC"

/* @brief UEFI magic for EPM */

#define kEPMMagicUEFI "EPMUE"

/* @brief Invalid magic for EPM */

#define kEPMMagicError "EPM??"

#define kEPMMaxBlks 128

///! @brief Version kind enum.
///! @brief Use in boot block version field.

enum kEPMKind
{
	kEPMMpUx  = 0xcf,
	kEPMLinux = 0x8f,
	kEPMBSD	  = 0x9f,
	kEPMNewOS = 0x1f,
};

typedef struct BootBlock	  BootBlockType;
typedef struct PartitionBlock PartitionBlockType;

#ifdef __x86_64__
#define kEPMMagic kEPMMagic86
#else
#ifdef __powerpc
#define kEPMMagic kEPMMagicPPC
#else
#define kEPMMagic kEPMMagicError
#endif
#endif

///! @brief partition must start at this address.
///! Anything below is reserved for Data backup by the Main OS.
#define kEPMStartPartitionBlk (sizeof(BootBlock))

///! @brief Current EPM revision (2)
#define kEPMRevision (2)

///! @brief Current EPM revision (2)
#define kEPMRevisionUEFI (0xF)

/// END OF SPECS

#endif // ifndef __PARTITION_MAP__