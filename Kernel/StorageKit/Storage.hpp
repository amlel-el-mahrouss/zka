/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <StorageKit/NVME.hpp>
#include <StorageKit/AHCI.hpp>
#include <StorageKit/SCSI.hxx>

#define kDriveSectorSizeHDD	  (512)
#define kDriveSectorSizeSSD	  (4096)
#define kDriveSectorSizeCDROM (2048)