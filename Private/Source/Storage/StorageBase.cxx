/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <StorageKit/Storage.hpp>

///! @brief ATAPI SCSI packet.
const SKScsiPacket kCDRomPacketTemplate = {0x43, 0,  1,    0, 0, 0,
                                           0,    12, 0x40, 0, 0};