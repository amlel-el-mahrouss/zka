/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <StorageKit/SCSI.hxx>

///! @brief ATAPI SCSI packet.
const scsi_packet_type kCDRomPacketTemplate = {0x43, 0,  1,    0, 0, 0,
                                           0,    12, 0x40, 0, 0};