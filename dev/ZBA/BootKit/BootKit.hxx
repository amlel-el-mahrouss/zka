/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

/***********************************************************************************/
/// @file Boot.hxx
/// @brief Bootloader Programming Interface.
/***********************************************************************************/

#pragma once

#include <BootKit/HW/ATA.hxx>
#include <FirmwareKit/EPM.hxx>
#include <CompilerKit/Version.hxx>

/// include NewFS header and Support header as well.

#include <cstring>
#include <FSKit/NewFS.hxx>
#include <BootKit/Support.hxx>

/***********************************************************************************/
/// Include other APIs.
/***********************************************************************************/

#include <NewKit/Defines.hxx>
#include <Modules/ATA/ATA.hxx>

#include <FirmwareKit/EFI.hxx>

/***********************************************************************************/
/// Framebuffer helpers.
/***********************************************************************************/

class BTextWriter;
class BFileReader;
class BThread;
class BVersionString;

typedef Char* PEFImagePtr;
typedef Char* PEImagePtr;

typedef WideChar CharacterTypeUTF16;
typedef Char	 CharacterTypeUTF8;

using namespace Kernel;

namespace EFI
{
	extern void ThrowError(const CharacterTypeUTF16* errorCode,
						   const CharacterTypeUTF16* reason) noexcept;
} // namespace EFI

/**
 * @brief BootKit Text Writer class
 * Writes to UEFI StdOut.
 */
class BTextWriter final
{
	BTextWriter& _Write(const Long& num);

public:
	BTextWriter& Write(const Long& num);
	BTextWriter& Write(const Char* str);
	BTextWriter& Write(const CharacterTypeUTF16* str);
	BTextWriter& WriteCharacter(CharacterTypeUTF16 c);
	BTextWriter& Write(const UChar* str);

public:
	explicit BTextWriter() = default;
	~BTextWriter()		   = default;

public:
	BTextWriter& operator=(const BTextWriter&) = default;
	BTextWriter(const BTextWriter&)			   = default;
};

Kernel::SizeT BCopyMem(CharacterTypeUTF16* dest, CharacterTypeUTF16* src, const Kernel::SizeT len);

Kernel::SizeT BSetMem(CharacterTypeUTF8* src, const CharacterTypeUTF8 byte, const Kernel::SizeT len);

/// String length functions.

/// @brief get string length.
Kernel::SizeT BStrLen(const CharacterTypeUTF16* ptr);

/// @brief set memory with custom value.
Kernel::SizeT BSetMem(CharacterTypeUTF16* src, const CharacterTypeUTF16 byte, const Kernel::SizeT len);

/**
 * @brief BootKit File Reader class
 * Reads the Firmware Boot partition and filesystem.
 */
class BFileReader final
{
public:
	explicit BFileReader(const CharacterTypeUTF16* path,
						 EfiHandlePtr			   ImageHandle);
	~BFileReader();

public:
	Void ReadAll(SizeT until, SizeT chunk = kib_cast(4), UIntPtr outAddress = 0UL);

	enum
	{
		kOperationOkay,
		kNotSupported,
		kEmptyDirectory,
		kNoSuchEntry,
		kIsDirectory,
		kTooSmall,
		kCount,
	};

	/// @brief error code getter.
	/// @return the error code.
	Int32& Error();

	/// @brief blob getter.
	/// @return the blob.
	VoidPtr Blob();

	/// @breif Size getter.
	/// @return the size of the file.
	UInt64& Size();

public:
	BFileReader& operator=(const BFileReader&) = default;
	BFileReader(const BFileReader&)			   = default;

private:
	Int32			   mErrorCode{kOperationOkay};
	VoidPtr			   mBlob{nullptr};
	CharacterTypeUTF16 mPath[kPathLen];
	BTextWriter		   mWriter;
	EfiFileProtocol*   mFile{nullptr};
	UInt64			   mSizeFile{0};
	EfiFileProtocol*   mRootFs;
};

typedef UInt8* BlobType;

class BVersionString final
{
public:
	static const CharacterTypeUTF8* The()
	{
		return BOOTLOADER_VERSION;
	}
};

/***********************************************************************************/
/// Provide some useful processor features.
/***********************************************************************************/

#ifdef __EFI_x86_64__

/***
 * Common processor instructions.
 */

EXTERN_C void	Out8(UInt16 port, UInt8 value);
EXTERN_C void	Out16(UInt16 port, UInt16 value);
EXTERN_C void	Out32(UInt16 port, UInt32 value);
EXTERN_C UInt8	In8(UInt16 port);
EXTERN_C UInt16 In16(UInt16 port);
EXTERN_C UInt32 In32(UInt16 port);

EXTERN_C void rt_hlt();
EXTERN_C void rt_cli();
EXTERN_C void rt_sti();
EXTERN_C void rt_cld();
EXTERN_C void rt_std();

#endif // __EFI_x86_64__

static inline const UInt32 kRgbRed	 = 0x000000FF;
static inline const UInt32 kRgbGreen = 0x0000FF00;
static inline const UInt32 kRgbBlue	 = 0x00FF0000;
static inline const UInt32 kRgbBlack = 0x00000000;
static inline const UInt32 kRgbWhite = 0x00FFFFFF;

#define kBKBootFileMime "boot-x/file"
#define kBKBootDirMime	"boot-x/dir"

/// @brief BootKit Disk Formatter.
template <typename BootDev>
class BDiskFormatFactory final
{
public:
	/// @brief File entry for **BDiskFormatFactory**.
	struct BFileDescriptor final
	{
		Char  fFileName[kNewFSNodeNameLen];
		Int32 fKind;
	};

public:
	explicit BDiskFormatFactory() = default;
	explicit BDiskFormatFactory(BootDev dev)
		: fDiskDev(dev)
	{
	}

	~BDiskFormatFactory() = default;

	NEWOS_COPY_DELETE(BDiskFormatFactory);

	/// @brief Format disk.
	/// @param Partition Name
	/// @param Blobs.
	/// @param Number of blobs.
	/// @retval True disk has been formatted.
	/// @retval False failed to format.
	Boolean Format(const char* partName, BFileDescriptor* fileBlobs, SizeT blobCount);

	/// @brief check if partition is good.
	Bool IsPartitionValid() noexcept
	{
		fDiskDev.Leak().mBase = (kNewFSStartLba);
		fDiskDev.Leak().mSize = BootDev::kSectorSize;

		Char buf[BootDev::kSectorSize] = {0};

		fDiskDev.Read(buf, BootDev::kSectorSize);

		NFS_ROOT_PARTITION_BLOCK* blockPart = reinterpret_cast<NFS_ROOT_PARTITION_BLOCK*>(buf);

		BTextWriter writer;

		for (SizeT indexMag = 0UL; indexMag < kNewFSIdentLen; ++indexMag)
		{
			if (blockPart->Ident[indexMag] != kNewFSIdent[indexMag])
				return false;
		}

		writer.Write(L"newosldr: disk size: ").Write(this->fDiskDev.GetDiskSize()).Write(L"\r");

		if (blockPart->DiskSize != this->fDiskDev.GetDiskSize() ||
			blockPart->DiskSize < 1 ||
			blockPart->SectorSize != BootDev::kSectorSize ||
			blockPart->Version != kNewFSVersionInteger)
		{
			return false;
		}
		else if (blockPart->PartitionName[0] == 0)
		{
			return false;
		}

		writer.Write(L"newosldr: partition name: ").Write(blockPart->PartitionName).Write(L" is healthy.\r");

		return true;
	}

private:
	/// @brief Write all of the requested catalogs into the filesystem.
	/// @param fileBlobs the blobs.
	/// @param blobCount the number of blobs to write.
	/// @param partBlock the NewFS partition block.
	Boolean WriteRootCatalog(BFileDescriptor* fileBlobs, SizeT blobCount, NFS_ROOT_PARTITION_BLOCK& partBlock)
	{
		if (partBlock.SectorSize != BootDev::kSectorSize)
			return false;

		BFileDescriptor* blob	  = fileBlobs;
		Lba				 startLba = partBlock.StartCatalog;
		BTextWriter		 writer;

		Char bufCatalog[sizeof(NFS_CATALOG_STRUCT)] = {0};

		constexpr auto cNewFSCatalogPadding = 4;

		NFS_CATALOG_STRUCT* catalogKind = (NFS_CATALOG_STRUCT*)bufCatalog;
		catalogKind->PrevSibling		= startLba;
		catalogKind->NextSibling		= (startLba + (sizeof(NFS_CATALOG_STRUCT) * cNewFSCatalogPadding));

		/// Fill catalog kind.
		catalogKind->Kind  = blob->fKind;
		catalogKind->Flags = kNewFSFlagCreated;

		--partBlock.FreeCatalog;
		--partBlock.FreeSectors;

		writer.Write(L"newosldr: Wrote directory: ").Write(blob->fFileName).Write(L"\r");
		writer.Write(L"newosldr: disk formatted.\r");

		CopyMem(catalogKind->Name, blob->fFileName, StrLen(blob->fFileName));

		fDiskDev.Leak().mBase = startLba;
		fDiskDev.Leak().mSize = sizeof(NFS_CATALOG_STRUCT);

		fDiskDev.Write((Char*)bufCatalog, sizeof(NFS_CATALOG_STRUCT));

		return true;
	}

private:
	BootDev fDiskDev;
};

/// @brief Format disk.
/// @param Partition Name
/// @param Blobs.
/// @param Number of blobs.
/// @retval True disk has been formatted.
/// @retval False failed to format.
template <typename BootDev>
inline Boolean BDiskFormatFactory<BootDev>::Format(const char*							partName,
												   BDiskFormatFactory::BFileDescriptor* fileBlobs,
												   SizeT								blobCount)
{
	if (!fileBlobs || !blobCount)
		return false; /// sanity check

	/// convert the sector into something that the disk understands.
	SizeT sectorSz = BootDev::kSectorSize;
	Char* buf	   = new Char[BootDev::kSectorSize];

	NFS_ROOT_PARTITION_BLOCK* partBlock = reinterpret_cast<NFS_ROOT_PARTITION_BLOCK*>(buf);

	memcpy(partBlock->Ident, kNewFSIdent, kNewFSIdentLen - 1);
	memcpy(partBlock->PartitionName, partName, strlen(partName));

	/// @note A catalog roughly equal to a sector.

	constexpr auto cMinimumDiskSize = 4; // at minimum.

	/// @note also look at EPM headers, for free part blocks.

	if (GIB(fDiskDev.GetDiskSize()) < cMinimumDiskSize)
	{
		delete buf;
		EFI::ThrowError(L"Disk-Too-Tiny", L"Can't format a New Filesystem partition here.");
		return false;
	}

	partBlock->Version		= kNewFSVersionInteger;
	partBlock->CatalogCount = blobCount;
	partBlock->Kind			= kNewFSHardDrive;
	partBlock->SectorSize	= sectorSz;
	partBlock->FreeCatalog	= fDiskDev.GetSectorsCount() / sizeof(NFS_CATALOG_STRUCT);
	partBlock->SectorCount	= fDiskDev.GetSectorsCount();
	partBlock->FreeSectors	= fDiskDev.GetSectorsCount();
	partBlock->StartCatalog = kNewFSCatalogStartAddress;
	partBlock->DiskSize		= fDiskDev.GetDiskSize();
	partBlock->Flags |= kNewFSPartitionTypeBoot;

	/// if we can write a root catalog, then write the partition block.
	if (this->WriteRootCatalog(fileBlobs, blobCount, *partBlock))
	{
		fDiskDev.Leak().mBase = kNewFSStartLba;
		fDiskDev.Leak().mSize = sectorSz;

		fDiskDev.Write(buf, sectorSz);

		/// Reset buffer.
		SetMem(buf, 0, sectorSz);

		BOOT_BLOCK_STRUCT* epmBoot = (BOOT_BLOCK_STRUCT*)buf;

		constexpr auto cFsName	  = "NewFS";
		constexpr auto cBlockName = "ZKA:";

		CopyMem(reinterpret_cast<VoidPtr>(const_cast<Char*>(cFsName)), epmBoot->Fs, StrLen(cFsName));

		epmBoot->FsVersion = kNewFSVersionInteger;
		epmBoot->LbaStart  = kNewFSStartLba;
		epmBoot->SectorSz  = partBlock->SectorSize;
		epmBoot->NumBlocks = partBlock->CatalogCount;

		CopyMem(epmBoot->Name, reinterpret_cast<VoidPtr>(const_cast<Char*>(cBlockName)), StrLen(cBlockName));
		CopyMem(epmBoot->Magic, reinterpret_cast<VoidPtr>(const_cast<Char*>(kEPMMagic)), StrLen(kEPMMagic));

		fDiskDev.Leak().mBase = kEpmBase;
		fDiskDev.Leak().mSize = sectorSz;

		fDiskDev.Write(buf, sectorSz);

		delete buf;
		return true;
	}
	else
	{
		delete buf;
		EFI::ThrowError(L"Filesystem-Failure-Part", L"Filesystem couldn't be partitioned.");
	}

	delete buf;
	return false;
}