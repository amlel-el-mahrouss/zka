/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#ifdef __FSKIT_NEWFS__

#include <Builtins/AHCI/AHCI.hxx>
#include <Builtins/ATA/ATA.hxx>
#include <FSKit/NewFS.hxx>
#include <KernelKit/HError.hpp>
#include <NewKit/Crc32.hpp>
#include <NewKit/KernelCheck.hpp>
#include <NewKit/String.hpp>
#include <NewKit/Utils.hpp>

using namespace NewOS;

///! BUGS: 0

/***********************************************************************************/
/* This file implements the New File System.
/***********************************************************************************/

STATIC MountpointInterface sMountpointInterface;

/// @brief Creates a new fork inside the New filesystem partition.
/// @param catalog it's catalog
/// @param theFork the fork itself.
/// @return the fork
_Output NewFork* NewFSParser::CreateFork(_Input NewCatalog* catalog,
										 _Input NewFork& theFork)
{
	if (!sMountpointInterface.GetAddressOf(this->fDriveIndex))
		return nullptr;

	if (catalog && theFork.ForkName[0] != 0 &&
		theFork.DataSize == kNewFSForkSize)
	{
		Lba lba = (theFork.Kind == kNewFSDataForkKind) ? catalog->DataFork
													   : catalog->ResourceFork;

		kcout << "Fork Lba: " << hex_number(lba) << endl;

		if (lba <= kNewFSCatalogStartAddress)
			return nullptr;

		auto drv = sMountpointInterface.GetAddressOf(this->fDriveIndex);

		/// special treatment.
		rt_copy_memory((VoidPtr) "fs/newfs-packet", drv->fPacket.fPacketMime,
					   rt_string_len("fs/newfs-packet"));

		NewFork curFork{0};
		NewFork prevFork{0};
		Lba		lbaOfPreviousFork = lba;

		/// do not check for anything. Loop until we get what we want, that is a free fork zone.
		while (true)
		{
			if (lba <= kNewFSCatalogStartAddress)
				break;

			drv->fPacket.fLba			= lba;
			drv->fPacket.fPacketSize	= sizeof(NewFork);
			drv->fPacket.fPacketContent = &curFork;

			drv->fInput(&drv->fPacket);

			kcout << "New OS: Next-Fork: " << hex_number(curFork.NextSibling) << endl;

			if (curFork.Flags == kNewFSFlagCreated)
			{
				kcout << "New OS: Fork already exists.\r";

				/// sanity check.
				if (StringBuilder::Equals(curFork.ForkName, theFork.ForkName) &&
					StringBuilder::Equals(curFork.CatalogName, catalog->Name))
					return nullptr;

				kcout << "Next-Fork: " << hex_number(curFork.NextSibling) << endl;

				lbaOfPreviousFork = lba;
				lba				  = curFork.NextSibling;

				prevFork = curFork;
			}
			else
			{
				/// This is a check that we have, in order to link the previous fork
				/// entry.
				if (lba >= kNewFSCatalogStartAddress)
				{
					drv->fPacket.fLba			= lbaOfPreviousFork;
					drv->fPacket.fPacketSize	= sizeof(NewFork);
					drv->fPacket.fPacketContent = &prevFork;

					prevFork.NextSibling = lba;

					/// write to disk.
					drv->fOutput(&drv->fPacket);
				}

				break;
			}
		}

		constexpr auto cForkPadding =
			4; /// this value gives us space for the data offset.

		theFork.Flags			= kNewFSFlagCreated;
		theFork.DataOffset		= lba - sizeof(NewFork) * cForkPadding;
		theFork.PreviousSibling = lbaOfPreviousFork;
		theFork.NextSibling		= theFork.DataOffset - theFork.DataSize;

		drv->fPacket.fLba			= lba;
		drv->fPacket.fPacketSize	= sizeof(NewFork);
		drv->fPacket.fPacketContent = &theFork;

		drv->fOutput(&drv->fPacket);

		/// log what we have now.
		kcout << "New OS: Wrote fork data at: " << hex_number(theFork.DataOffset)
			  << endl;

		kcout << "New OS: Wrote fork at: " << hex_number(lba) << endl;

		return &theFork;
	}

	return nullptr;
}

/// @brief Find fork inside New filesystem.
/// @param catalog the catalog.
/// @param name the fork name.
/// @return the fork.
_Output NewFork* NewFSParser::FindFork(_Input NewCatalog* catalog,
									   _Input const Char* name,
									   Boolean			  isDataFork)
{
	auto	 drv	 = sMountpointInterface.GetAddressOf(this->fDriveIndex);
	NewFork* theFork = nullptr;

	Lba lba = isDataFork ? catalog->DataFork : catalog->ResourceFork;

	while (lba != 0)
	{
		drv->fPacket.fLba			= lba;
		drv->fPacket.fPacketSize	= sizeof(NewFork);
		drv->fPacket.fPacketContent = (VoidPtr)theFork;

		rt_copy_memory((VoidPtr) "fs/newfs-packet", drv->fPacket.fPacketMime, 16);

		if (auto res =
				fs_newfs_read(&sMountpointInterface, *drv, this->fDriveIndex);
			res)
		{
			switch (res)
			{
			case 1:
				DbgLastError() = kErrorDiskReadOnly;
				break;
			case 2:
				DbgLastError() = kErrorDiskIsFull;
				break;
				DbgLastError() = kErrorNoSuchDisk;
				break;

			default:
				break;
			}
			return nullptr;
		}

		if (StringBuilder::Equals(theFork->ForkName, name))
		{
			break;
		}

		lba = theFork->NextSibling;
	}

	return theFork;
}

/// @brief Simpler factory to create a catalog (assumes you want to create a
/// file.)
/// @param name
/// @return catalog pointer.
_Output NewCatalog* NewFSParser::CreateCatalog(_Input const char* name)
{
	return this->CreateCatalog(name, 0, kNewFSCatalogKindFile);
}

/// @brief Creates a new catalog into the disk.
/// @param name the catalog name.
/// @param flags the flags of the catalog.
/// @param kind the catalog kind.
/// @return catalog pointer.
_Output NewCatalog* NewFSParser::CreateCatalog(_Input const char* name,
											   _Input const Int32& flags,
											   _Input const Int32& kind)
{
	if (!sMountpointInterface.GetAddressOf(this->fDriveIndex))
		return nullptr;

	Lba outLba = 0UL;

	/// a directory should have a slash in the end.
	if (kind == kNewFSCatalogKindDir &&
		name[rt_string_len(name) - 1] != NewFilesystemHelper::Separator())
		return nullptr;

	/// a file shouldn't have a slash in the end.
	if (kind != kNewFSCatalogKindDir &&
		name[rt_string_len(name) - 1] == NewFilesystemHelper::Separator())
		return nullptr;

	NewCatalog* checkForCpy = this->FindCatalog(name, outLba);

	if (checkForCpy)
	{
		return checkForCpy;
	}

	char parentName[kNewFSNodeNameLen] = {0};

	for (SizeT indexName = 0UL; indexName < rt_string_len(name); ++indexName)
	{
		parentName[indexName] = name[indexName];
	}

	if (*parentName == 0)
	{
		DbgLastError() = kErrorFileNotFound;
		return nullptr;
	}

	/// Locate parent catalog, to then allocate right after it.

	for (SizeT indexFill = 0; indexFill < rt_string_len(name); ++indexFill)
	{
		parentName[indexFill] = name[indexFill];
	}

	SizeT indexReverseCopy = rt_string_len(parentName);

	// zero character.
	parentName[--indexReverseCopy] = 0;

	// mandatory / character.
	parentName[--indexReverseCopy] = 0;

	while (parentName[indexReverseCopy] != NewFilesystemHelper::Separator())
	{
		parentName[indexReverseCopy] = 0;
		--indexReverseCopy;
	}

	NewCatalog* catalog = this->FindCatalog(parentName, outLba);

	if (catalog && catalog->Kind == kNewFSCatalogKindFile)
	{
		delete catalog;
		return nullptr;
	}
	else if (!catalog)
	{
		outLba = kNewFSCatalogStartAddress;
	}

	constexpr SizeT cDefaultForkSize = kNewFSForkSize;

	NewCatalog* catalogChild = new NewCatalog();

	catalogChild->ResourceForkSize = cDefaultForkSize;
	catalogChild->DataForkSize	   = cDefaultForkSize;

	catalogChild->NextSibling = outLba;
	catalogChild->PrevSibling = outLba;
	catalogChild->Kind		  = kind;
	catalogChild->Flags		  = kNewFSFlagCreated;

	rt_copy_memory((VoidPtr)name, (VoidPtr)catalogChild->Name,
				   rt_string_len(name));

	UInt16 catalogBuf[kNewFSSectorSz] = {0};

	auto drive = sMountpointInterface.GetAddressOf(this->fDriveIndex);

	Lba startFree = outLba;

	rt_copy_memory((VoidPtr) "fs/newfs-packet", drive->fPacket.fPacketMime,
				   rt_string_len("fs/newfs-packet"));

	drive->fPacket.fPacketContent = catalogBuf;
	drive->fPacket.fPacketSize	  = kNewFSSectorSz;
	drive->fPacket.fLba			  = startFree;

	drive->fInput(&drive->fPacket);

	NewCatalog* nextSibling = (NewCatalog*)catalogBuf;

	startFree = nextSibling->NextSibling;

	catalogChild->PrevSibling = outLba;

	drive->fPacket.fLba = startFree;
	drive->fInput(&drive->fPacket);

	while (drive->fPacket.fPacketGood)
	{
		nextSibling = (NewCatalog*)catalogBuf;

		if (startFree <= kNewFSAddressAsLba)
		{
			delete catalogChild;
			delete catalog;

			return nullptr;
		}

		/// allocation or reallocation or catalog...
		if (nextSibling->Flags != kNewFSFlagCreated)
		{
			Char sectorBufPartBlock[kNewFSSectorSz] = {0};

			drive->fPacket.fPacketContent = sectorBufPartBlock;
			drive->fPacket.fPacketSize	  = kNewFSSectorSz;
			drive->fPacket.fLba			  = kNewFSAddressAsLba;

			drive->fInput(&drive->fPacket);

			constexpr auto cNewFSCatalogPadding = 4;

			NewPartitionBlock* partBlock = (NewPartitionBlock*)sectorBufPartBlock;

			if (partBlock->FreeCatalog < 1)
			{
				delete catalogChild;
				return nullptr;
			}

			catalogChild->DataFork = partBlock->DiskSize - partBlock->StartCatalog;

			catalogChild->ResourceFork = catalogChild->DataFork;

			catalogChild->NextSibling =
				startFree + (sizeof(NewCatalog) * cNewFSCatalogPadding);

			drive->fPacket.fPacketContent = catalogChild;
			drive->fPacket.fPacketSize	  = sizeof(NewCatalog);
			drive->fPacket.fLba			  = startFree;

			drive->fOutput(&drive->fPacket);

			drive->fPacket.fPacketContent = catalogBuf;
			drive->fPacket.fPacketSize	  = kNewFSSectorSz;
			drive->fPacket.fLba =
				startFree - (sizeof(NewCatalog) * cNewFSCatalogPadding);

			drive->fInput(&drive->fPacket);

			nextSibling->NextSibling = startFree;

			drive->fOutput(&drive->fPacket);

			kcout << "New OS: Create new catalog, status: "
				  << hex_number(catalogChild->Flags) << endl;
			kcout << "New OS: Create new catalog, status: " << catalogChild->Name
				  << endl;

			drive->fPacket.fPacketContent = sectorBufPartBlock;
			drive->fPacket.fPacketSize	  = kNewFSSectorSz;
			drive->fPacket.fLba			  = kNewFSAddressAsLba;

			drive->fInput(&drive->fPacket);

			partBlock->SectorCount -= 1;
			partBlock->CatalogCount += 1;
			partBlock->FreeCatalog -= 1;
			partBlock->FreeCatalog = catalogChild->NextSibling;

			drive->fOutput(&drive->fPacket);

			delete catalog;
			return catalogChild;
		}

		constexpr auto cNewFSCatalogPadding = 4;

		//// @note that's how we find the next catalog in the partition block.
		startFree = startFree + (sizeof(NewCatalog) * cNewFSCatalogPadding);

		drive->fPacket.fPacketContent = catalogBuf;
		drive->fPacket.fPacketSize	  = kNewFSSectorSz;
		drive->fPacket.fLba			  = startFree;

		drive->fInput(&drive->fPacket);
	}

	delete catalog;
	return nullptr;
}

/// @brief Make a EPM+NewFS drive out of the disk.
/// @param drive The drive to write on.
/// @return If it was sucessful, see DbgLastError().
bool NewFSParser::Format(_Input _Output DriveTrait* drive)
{
	/// verify disk.
	drive->fVerify(&drive->fPacket);

	rt_copy_memory((VoidPtr) "fs/newfs-packet", drive->fPacket.fPacketMime,
				   rt_string_len("fs/newfs-packet"));

	/// if disk isn't good, then error out.
	if (false == drive->fPacket.fPacketGood)
	{
		DbgLastError() = kErrorDiskIsCorrupted;
		return false;
	}

	Char sectorBuf[kNewFSSectorSz] = {0};

	drive->fPacket.fPacketContent = sectorBuf;
	drive->fPacket.fPacketSize	  = kNewFSSectorSz;
	drive->fPacket.fLba			  = kNewFSAddressAsLba;

	drive->fInput(&drive->fPacket);

	/// disk isnt faulty and data has been fetched.
	if (drive->fPacket.fPacketGood)
	{
		NewPartitionBlock* partBlock = (NewPartitionBlock*)sectorBuf;

		/// check for an empty partition here.
		if (partBlock->PartitionName[0] == 0 &&
			rt_string_cmp(partBlock->Ident, kNewFSIdent, kNewFSIdentLen))
		{
			/// partition is free and valid.

			partBlock->Version = kNewFSVersionInteger;

			const auto cUntitledHD = "New OS HD\0";

			rt_copy_memory((VoidPtr)kNewFSIdent, (VoidPtr)partBlock->Ident,
						   kNewFSIdentLen);

			rt_copy_memory((VoidPtr)cUntitledHD, (VoidPtr)partBlock->PartitionName,
						   rt_string_len(cUntitledHD));

			SizeT catalogCount = 0;
			SizeT sectorCount  = drv_std_get_sector_count();
			SizeT diskSize	   = drv_std_get_drv_size();

			partBlock->Kind			= kNewFSPartitionTypeStandard;
			partBlock->StartCatalog = kNewFSCatalogStartAddress;
			partBlock->Flags		= kNewFSPartitionTypeStandard;
			partBlock->CatalogCount = sectorCount / sizeof(NewCatalog);
			partBlock->SectorCount	= sectorCount;
			partBlock->DiskSize		= diskSize;
			partBlock->FreeCatalog	= sectorCount / sizeof(NewCatalog);

			drive->fPacket.fPacketContent = sectorBuf;
			drive->fPacket.fPacketSize	  = kNewFSSectorSz;
			drive->fPacket.fLba			  = kNewFSAddressAsLba;

			drive->fOutput(&drive->fPacket);

			kcout << "Drive-Kind: " << drive->fDriveKind() << endl;

			kcout << "Partition-Name: " << partBlock->PartitionName << endl;
			kcout << "Start-Catalog: " << hex_number(partBlock->StartCatalog) << endl;
			kcout << "Catalog-Count: " << hex_number(partBlock->CatalogCount) << endl;
			kcout << "Free-Catalog: " << hex_number(partBlock->FreeCatalog) << endl;
			kcout << "Free-Sectors: " << hex_number(partBlock->FreeSectors) << endl;
			kcout << "Sector-Size: " << hex_number(partBlock->SectorSize) << endl;

			/// write the root catalog.
			this->CreateCatalog(kNewFSRoot, 0, kNewFSCatalogKindDir);

			return true;
		}

		kcout << "New OS: PartitionBlock already exists.\r";

		/// return success as well, do not ignore that partition.
		return true;
	}

	return false;
}

/// @brief Writes the data fork into a specific catalog.
/// @param catalog the catalog itself
/// @param data the data.
/// @return if the catalog w rote the contents successfully.
bool NewFSParser::WriteCatalog(_Input _Output NewCatalog* catalog, voidPtr data, SizeT sizeOfData, _Input const char* forkName)
{
	if (sizeOfData > catalog->DataForkSize)
		return false;
	if (!sMountpointInterface.GetAddressOf(this->fDriveIndex))
		return false;

	NewFork* forkData = new NewFork();
	rt_set_memory(forkData, 0, sizeof(NewFork));

	auto drive = sMountpointInterface.GetAddressOf(this->fDriveIndex);

	rt_copy_memory((VoidPtr) "fs/newfs-packet", drive->fPacket.fPacketMime,
				   rt_string_len("fs/newfs-packet"));

	auto startFork = catalog->DataFork;

	rt_copy_memory(catalog->Name, forkData->CatalogName, kNewFSNodeNameLen);

	/// sanity check of the fork position as the condition to run the loop.
	while (startFork >= kNewFSCatalogStartAddress)
	{
		drive->fPacket.fPacketContent = forkData;
		drive->fPacket.fPacketSize	  = sizeof(NewFork);
		drive->fPacket.fLba			  = startFork;

		drive->fInput(&drive->fPacket);

		kcout << "Fork-Name: " << forkData->ForkName << endl;

		/// sanity check the fork.
		if (forkData->DataOffset <= kNewFSCatalogStartAddress)
		{
			DbgLastError() = kErrorDiskIsCorrupted;

			kcout << "New OS: Invalid fork offset.\r";

			delete forkData;
			return false;
		}

		if (forkData->Flags != kNewFSFlagUnallocated &&
			forkData->Flags != kNewFSFlagDeleted &&
			StringBuilder::Equals(forkData->ForkName, forkName) &&
			StringBuilder::Equals(forkData->CatalogName, catalog->Name))
		{
			drive->fPacket.fPacketContent = data;
			drive->fPacket.fPacketSize	  = sizeOfData;
			drive->fPacket.fLba			  = forkData->DataOffset;
			kcout << "Fork-Offset: " << hex_number(forkData->DataOffset) << endl;

			drive->fOutput(&drive->fPacket);

			delete forkData;
			return true;
		}
		else if (auto catalog = this->GetCatalog(forkData->CatalogName);
				 catalog == nullptr)
		{
			delete catalog;
			drive->fPacket.fPacketContent = data;
			drive->fPacket.fPacketSize	  = sizeOfData;
			drive->fPacket.fLba			  = forkData->DataOffset;
			kcout << "Fork-Offset: " << hex_number(forkData->DataOffset) << endl;

			drive->fOutput(&drive->fPacket);

			delete forkData;
			return true;
		}

		startFork = forkData->NextSibling;
	}

	delete forkData;
	return false;
}

/// @brief
/// @param catalogName the catalog name.
/// @return the newly found catalog.
_Output NewCatalog* NewFSParser::FindCatalog(_Input const char* catalogName,
											 Lba&				outLba)
{
	if (!sMountpointInterface.GetAddressOf(this->fDriveIndex))
		return nullptr;

	Char* sectorBuf = new Char[sizeof(NewPartitionBlock)];
	auto  drive		= sMountpointInterface.GetAddressOf(this->fDriveIndex);

	rt_copy_memory((VoidPtr) "fs/newfs-packet", drive->fPacket.fPacketMime,
				   rt_string_len("fs/newfs-packet"));

	drive->fPacket.fPacketContent = sectorBuf;
	drive->fPacket.fPacketSize	  = sizeof(NewPartitionBlock);
	drive->fPacket.fLba			  = kNewFSAddressAsLba;

	drive->fInput(&drive->fPacket);

	NewPartitionBlock* part = (NewPartitionBlock*)sectorBuf;

	auto	   startCatalogList	 = part->StartCatalog;
	const auto cCtartCatalogList = part->StartCatalog;

	auto localSearchFirst = false;

	drive->fPacket.fLba			  = startCatalogList;
	drive->fPacket.fPacketContent = sectorBuf;
	drive->fPacket.fPacketSize	  = sizeof(NewCatalog);

	drive->fInput(&drive->fPacket);

	if (!StringBuilder::Equals(catalogName, NewFilesystemHelper::Root()))
	{
		Char parentName[kNewFSNodeNameLen] = {0};

		for (SizeT indexFill = 0; indexFill < rt_string_len(catalogName); ++indexFill)
		{
			parentName[indexFill] = catalogName[indexFill];
		}

		SizeT indexReverseCopy = rt_string_len(parentName);

		// zero character.
		parentName[--indexReverseCopy] = 0;

		// mandatory '/' character.
		parentName[--indexReverseCopy] = 0;

		while (parentName[indexReverseCopy] != NewFilesystemHelper::Separator())
		{
			parentName[indexReverseCopy] = 0;
			--indexReverseCopy;
		}

		NewCatalog* parentCatalog = this->FindCatalog(parentName, outLba);

		if (parentCatalog &&
			!StringBuilder::Equals(parentName, NewFilesystemHelper::Root()))
		{
			startCatalogList = outLba;
			delete parentCatalog;

			localSearchFirst = true;
		}
		else if (parentCatalog)
		{
			delete parentCatalog;
		}
	}

_NewFSSearchThroughCatalogList:
	while (drive->fPacket.fPacketGood)
	{
		NewCatalog* catalog = (NewCatalog*)sectorBuf;

		if (StringBuilder::Equals(catalogName, catalog->Name))
		{
			/// ignore unallocated catalog, break
			if (catalog->Flags != kNewFSFlagCreated)
			{
				goto _NewFSContinueSearch;
			}

			NewCatalog* catalogPtr = new NewCatalog();
			rt_copy_memory(catalog, catalogPtr, sizeof(NewCatalog));

			kcout << "New OS: Found catalog at: " << hex_number(startCatalogList) << endl;

			outLba = startCatalogList;
			delete[] sectorBuf;
			return catalogPtr;
		}

	_NewFSContinueSearch:
		startCatalogList = catalog->NextSibling;

		if (startCatalogList <= kNewFSAddressAsLba)
			break;

		drive->fPacket.fLba			  = startCatalogList;
		drive->fPacket.fPacketContent = sectorBuf;
		drive->fPacket.fPacketSize	  = sizeof(NewCatalog);

		drive->fInput(&drive->fPacket);
	}

	if (localSearchFirst)
	{
		localSearchFirst = false;
		startCatalogList = cCtartCatalogList;

		goto _NewFSSearchThroughCatalogList;
	}

	outLba = 0UL;
	delete[] sectorBuf;

	return nullptr;
}

/// @brief
/// @param name
/// @return
_Output NewCatalog* NewFSParser::GetCatalog(_Input const char* name)
{
	Lba unused = 0;
	return this->FindCatalog(name, unused);
}

/// @brief
/// @param catalog
/// @return
Boolean NewFSParser::CloseCatalog(_Input _Output NewCatalog* catalog)
{
	if (!catalog)
		return false;

	delete catalog;
	catalog = nullptr;

	return true;
}

/// @brief Mark catalog as removed.
/// @param catalog The catalog structure.
/// @return if the catalog was removed or not.
Boolean NewFSParser::RemoveCatalog(_Input const Char* catalogName)
{
	if (!catalogName ||
		StringBuilder::Equals(catalogName, NewFilesystemHelper::Root()))
	{
		DbgLastError() = kErrorInternal;
		return false;
	}

	Lba	 outLba	 = 0;
	auto catalog = this->FindCatalog(catalogName, outLba);

	if (outLba >= kNewFSCatalogStartAddress ||
		catalog->Flags == kNewFSFlagCreated)
	{
		catalog->Flags = kNewFSFlagDeleted;

		auto drive = sMountpointInterface.GetAddressOf(this->fDriveIndex);

		rt_copy_memory((VoidPtr) "fs/newfs-packet", drive->fPacket.fPacketMime,
					   rt_string_len("fs/newfs-packet"));

		drive->fPacket.fLba = outLba; // the catalog position.
		drive->fPacket.fPacketSize =
			sizeof(NewCatalog);					 // size of catalog. roughly the sector size.
		drive->fPacket.fPacketContent = catalog; // the catalog itself.

		drive->fOutput(&drive->fPacket); // send packet.

		Char partitionBlockBuf[sizeof(NewPartitionBlock)] = {0};

		drive->fPacket.fLba			  = kNewFSAddressAsLba;
		drive->fPacket.fPacketContent = partitionBlockBuf;
		drive->fPacket.fPacketSize	  = sizeof(NewPartitionBlock);

		drive->fInput(&drive->fPacket);

		NewPartitionBlock* partBlock =
			reinterpret_cast<NewPartitionBlock*>(partitionBlockBuf);

		++partBlock->FreeCatalog;
		--partBlock->CatalogCount;

		drive->fOutput(&drive->fPacket);

		return true;
	}

	delete catalog;
	return false;
}

/// ***************************************************************** ///
/// Reading,Seek,Tell are unimplemented on catalogs, refer to forks I/O instead.
/// ***************************************************************** ///

/***********************************************************************************/
/// @brief Read the catalog data fork.
/// @param catalog
/// @param dataSz
/// @return
/***********************************************************************************/

VoidPtr NewFSParser::ReadCatalog(_Input _Output NewCatalog* catalog,
								 _Input SizeT				dataSz,
								 _Input const char*			forkName)
{
	if (!catalog)
	{
		DbgLastError() = kErrorFileNotFound;
		return nullptr;
	}

	if (!sMountpointInterface.GetAddressOf(this->fDriveIndex))
		return nullptr;

	Lba	 dataForkLba  = catalog->DataFork;
	Size dataForkSize = catalog->DataForkSize;

	kcout << "Found-Catalog: " << catalog->Name
		  << ", Data-Fork: " << hex_number(dataForkLba) << endl;

	Char* sectorBuf = new Char[sizeof(NewFork)];
	auto  drive		= sMountpointInterface.GetAddressOf(this->fDriveIndex);

	rt_copy_memory((VoidPtr) "fs/newfs-packet", drive->fPacket.fPacketMime,
				   rt_string_len("fs/newfs-packet"));

	NewFork* forkData = nullptr;

	while (dataForkLba >= kNewFSCatalogStartAddress)
	{
		drive->fPacket.fLba			  = dataForkLba;
		drive->fPacket.fPacketSize	  = sizeof(NewFork);
		drive->fPacket.fPacketContent = sectorBuf;

		drive->fInput(&drive->fPacket);

		forkData = (NewFork*)sectorBuf;

		kcout << "Fork-Name: " << forkData->ForkName << endl;

		if (forkData->DataOffset <= kNewFSCatalogStartAddress)
		{
			delete[] sectorBuf;

			kcout << "Fail-Data-Offset: " << hex_number(forkData->DataOffset) << endl;

			return nullptr;
		}

		if (StringBuilder::Equals(forkName, forkData->ForkName) &&
			StringBuilder::Equals(catalog->Name, forkData->CatalogName))
			break;

		dataForkLba = forkData->NextSibling;
	}

	if (dataForkLba <= kNewFSCatalogStartAddress)
	{
		delete[] sectorBuf;
		return nullptr;
	}

	Char* forkBuf = new Char[dataSz];

	drive->fPacket.fLba			  = forkData->DataOffset;
	drive->fPacket.fPacketSize	  = dataSz;
	drive->fPacket.fPacketContent = forkBuf;

	drive->fInput(&drive->fPacket);

	delete[] sectorBuf;
	return forkBuf;
}

/***********************************************************************************/
/// @brief Seek in the data fork.
/// @param catalog the catalog offset.
/// @param off where to seek.
/// @return if the seeking was successful.
/***********************************************************************************/

bool NewFSParser::Seek(_Input _Output NewCatalog* catalog, SizeT off)
{
	if (!catalog)
	{
		DbgLastError() = kErrorFileNotFound;
		return false;
	}

	DbgLastError() = kErrorUnimplemented;
	return false;
}

/***********************************************************************************/
/// @brief Tell where we are inside the data fork.
/// @param catalog
/// @return The position on the file.
/***********************************************************************************/

SizeT NewFSParser::Tell(_Input _Output NewCatalog* catalog)
{
	if (!catalog)
	{
		DbgLastError() = kErrorFileNotFound;
		return 0;
	}

	DbgLastError() = kErrorUnimplemented;
	return 0;
}

namespace NewOS::Detail
{
	/***********************************************************************************/
	/// @brief Construct NewFS drives.
	/***********************************************************************************/
	Boolean fs_init_newfs(Void) noexcept
	{
		sMountpointInterface.A() = construct_main_drive();
		sMountpointInterface.B() = construct_drive();
		sMountpointInterface.C() = construct_drive();
		sMountpointInterface.D() = construct_drive();

		sMountpointInterface.A().fVerify(&sMountpointInterface.A().fPacket);

		Char partitionBlockBuf[sizeof(NewPartitionBlock)] = {0};

		sMountpointInterface.A().fPacket.fLba			= kNewFSAddressAsLba;
		sMountpointInterface.A().fPacket.fPacketContent = partitionBlockBuf;
		sMountpointInterface.A().fPacket.fPacketSize	= sizeof(NewPartitionBlock);

		sMountpointInterface.A().fInput(&sMountpointInterface.A().fPacket);

		NewPartitionBlock* partBlock =
			reinterpret_cast<NewPartitionBlock*>(partitionBlockBuf);

		if (!StringBuilder::Equals(partBlock->Ident, kNewFSIdent))
		{
			kcout << "New OS: New FS Partition is corrupt.\r";
			return false;
		}

		kcout << "New OS: Read partition: " << partBlock->PartitionName << ", with success!\r";

		return true;
	}
} // namespace NewOS::Detail

#endif // ifdef __FSKIT_NEWFS__