/* -------------------------------------------

    Copyright SoftwareLabs

    File: NewFS.hxx
    Purpose:

    Revision History:

    ?/?/?: Added file (amlel)
    12/02/24: Add UUID macro for EPM and GPT partition schemes.
    3/16/24: Add mandatory sector size, kNewFSSectorSz is set to 2048 by
default.

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.hxx>
#include <HintKit/CompilerHint.hxx>
#include <KernelKit/DriveManager.hxx>
#include <NewKit/Defines.hpp>

/**
    @brief New File System specification.
    @author Amlal EL Mahrouss
*/

#define kNewFSInvalidFork	 (-1)
#define kNewFSInvalidCatalog (-1)
#define kNewFSNodeNameLen	 (256)

#define kNewFSSectorSz (512)

#define kNewFSIdentLen (8)
#define kNewFSIdent	   " NewFS"
#define kNewFSPadLen   (400)

/// @brief Partition GUID on EPM and GPT disks.
#define kNewFSUUID "@{DD997393-9CCE-4288-A8D5-C0FDE3908DBE}"

#define kNewFSVersionInteger (0x125)
#define kNewFSVerionString	 "1.25"

/// @brief Standard fork types.
#define kNewFSDataFork	   "data"
#define kNewFSResourceFork "rsrc"

#define kNewFSCatalogKindFile  (1)
#define kNewFSCatalogKindDir   (2)
#define kNewFSCatalogKindAlias (3)

#define kNewFSForkSize (8192)

//! shared between network or
//! other filesystems. Export forks as .zip when copying.
#define kNewFSCatalogKindShared (4)

#define kNewFSCatalogKindResource	(5)
#define kNewFSCatalogKindExecutable (6)

#define kNewFSCatalogKindPage (8)

#define kNewFSPartitionTypeStandard (7)
#define kNewFSPartitionTypePage		(8)
#define kNewFSPartitionTypeBoot		(9)

#define kNewFSCatalogKindDevice (9)
#define kNewFSCatalogKindLock	(10)

#define kNewFSSeparator '/'

#define kNewFSUpDir ".."
#define kNewFSRoot	"/"

#define kNewFSLF  '\r'
#define kNewFSEOF (-1)

#define kNewFSBitWidth (sizeof(NewCharType))
#define kNewFSLbaType  (NewOS::Lba)

/// Start After the PM headers, pad 1024 bytes.
#define kNewFSAddressAsLba		  (512)
#define kNewFSCatalogStartAddress (1024 + sizeof(NewPartitionBlock) + sizeof(NewCatalog))

#define kResourceTypeDialog (10)
#define kResourceTypeString (11)
#define kResourceTypeMenu	(12)

#define kConfigLen (64)
#define kPartLen   (32)

#define kNewFSFlagDeleted	  (70)
#define kNewFSFlagUnallocated (0)
#define kNewFSFlagCreated	  (71)

#define kNewFSMimeNameLen (200)

#define kNewFSForkNameLen (200U)

typedef NewOS::Char NewCharType;

enum
{
	kNewFSHardDrive			= 0xC0, // Hard Drive (SSD, HDD)
	kNewFSOpticalDrive		= 0x0C, // Blu-Ray/DVD
	kNewFSMassStorageDevice = 0xCC, // USB
	kNewFSScsi				= 0xC4, // SCSI Hard Drive
	kNewFSUnknown			= 0xFF, // Unknown device. (floppy)
	kNewFSDriveCount		= 5,
};

/// @brief Catalog type.
struct PACKED NewCatalog final
{
	NewCharType Name[kNewFSNodeNameLen];
	NewCharType Mime[kNewFSMimeNameLen];

	/// Catalog status flag.
	NewOS::UInt16 Flags;
	/// Custom catalog flags.
	NewOS::UInt16 FileFlags;
	/// Catalog kind.
	NewOS::Int32 Kind;

	/// Size of the data fork.
	NewOS::Lba DataForkSize;

	/// Size of all resource forks.
	NewOS::Lba ResourceForkSize;

	NewOS::Lba DataFork;
	NewOS::Lba ResourceFork;

	NewOS::Lba NextSibling;
	NewOS::Lba PrevSibling;
};

/// @brief Fork type, contains a data page.
/// @note The way we store is way different than how other filesystems do, specific chunk of code are
/// written into either the data fork or resource fork, the resource fork is reserved for file metadata.
/// whereas the data fork is reserved for file data.
struct PACKED NewFork final
{
	NewCharType ForkName[kNewFSForkNameLen];
	NewOS::Char CatalogName[kNewFSNodeNameLen];

	NewOS::Int32 Flags;
	NewOS::Int32 Kind;

	NewOS::Int64 ResourceId;
	NewOS::Int32 ResourceKind;
	NewOS::Int32 ResourceFlags;

	NewOS::Lba	 DataOffset; // 8 Where to look for this data?
	NewOS::SizeT DataSize;	 /// Data size according using sector count.

	NewOS::Lba NextSibling;
	NewOS::Lba PreviousSibling;
};

/// @brief Partition block type
struct PACKED NewPartitionBlock final
{
	NewCharType Ident[kNewFSIdentLen];
	NewCharType PartitionName[kPartLen];

	NewOS::Int32 Flags;
	NewOS::Int32 Kind;

	NewOS::Lba	 StartCatalog;
	NewOS::SizeT CatalogCount;

	NewOS::SizeT DiskSize;

	NewOS::SizeT FreeCatalog;
	NewOS::SizeT FreeSectors;

	NewOS::SizeT SectorCount;
	NewOS::SizeT SectorSize;

	NewOS::UInt64 Version;

	NewOS::Char Pad[kNewFSPadLen];
};

namespace NewOS
{

	enum
	{
		kNewFSSubDriveA,
		kNewFSSubDriveB,
		kNewFSSubDriveC,
		kNewFSSubDriveD,
		kNewFSSubDriveInvalid,
		kNewFSSubDriveCount,
	};

	/// \brief Resource fork kind.
	enum
	{
		kNewFSRsrcForkKind = 0,
		kNewFSDataForkKind = 1
	};

	///
	/// \name NewFSParser
	/// \brief NewFS parser class. (catalog creation, remove removal, root,
	/// forks...) Designed like the DOM, detects the filesystem automatically.
	///

	class NewFSParser final
	{
	public:
		explicit NewFSParser() = default;
		~NewFSParser()		   = default;

	public:
		NEWOS_COPY_DEFAULT(NewFSParser);

	public:
		/// @brief Creates a new fork inside the New filesystem partition.
		/// @param catalog it's catalog
		/// @param theFork the fork itself.
		/// @return the fork
		_Output NewFork* CreateFork(_Input NewCatalog* catalog,
									_Input NewFork& theFork);

		/// @brief Find fork inside New filesystem.
		/// @param catalog the catalog.
		/// @param name the fork name.
		/// @return the fork.
		_Output NewFork* FindFork(_Input NewCatalog* catalog,
								  _Input const Char* name,
								  Boolean			 dataOrRsrc);

		_Output Void RemoveFork(_Input NewFork* fork);

		_Output Void CloseFork(_Input NewFork* fork);

		_Output NewCatalog* FindCatalog(_Input const char* catalogName, Lba& outLba);

		_Output NewCatalog* GetCatalog(_Input const char* name);

		_Output NewCatalog* CreateCatalog(_Input const char* name,
										  _Input const Int32& flags,
										  _Input const Int32& kind);

		_Output NewCatalog* CreateCatalog(_Input const char* name);

		bool WriteCatalog(_Input _Output NewCatalog* catalog,
						  voidPtr					 data,
						  SizeT						 sizeOfData,
						  _Input const char*		 forkName);

		VoidPtr ReadCatalog(_Input _Output NewCatalog* catalog,
							SizeT					   dataSz,
							_Input const char*		   forkName);

		bool Seek(_Input _Output NewCatalog* catalog, SizeT off);

		SizeT Tell(_Input _Output NewCatalog* catalog);

		bool RemoveCatalog(_Input const Char* catalog);

		bool CloseCatalog(_InOut NewCatalog* catalog);

		/// @brief Make a EPM+NewFS drive out of the disk.
		/// @param drive The drive to write on.
		/// @return If it was sucessful, see DbgLastError().
		bool Format(_Input _Output DriveTrait* drive);

	public:
		Int32 fDriveIndex{kNewFSSubDriveA};
	};

	///
	/// \name NewFilesystemHelper
	/// \brief Filesystem helper and utils.
	///

	class NewFilesystemHelper final
	{
	public:
		static const char* Root();
		static const char* UpDir();
		static const char  Separator();
	};

	namespace Detail
	{
		Boolean fs_init_newfs(Void) noexcept;
	} // namespace Detail
} // namespace NewOS

/// @brief Write to newfs disk.
/// @param Mnt mounted interface.
/// @param DrvTrait drive info
/// @param DrvIndex drive index.
/// @return
NewOS::Int32 fs_newfs_write(NewOS::MountpointInterface* Mnt,
							NewOS::DriveTrait&			DrvTrait,
							NewOS::Int32				DrvIndex);

/// @brief Read from newfs disk.
/// @param Mnt mounted interface.
/// @param DrvTrait drive info
/// @param DrvIndex drive index.
/// @return
NewOS::Int32 fs_newfs_read(NewOS::MountpointInterface* Mnt,
						   NewOS::DriveTrait&		   DrvTrait,
						   NewOS::Int32				   DrvIndex);