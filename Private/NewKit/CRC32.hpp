/*
 *	========================================================
 *
 *	hCore 	Date Added: 13/02/2023
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef __CRC32_H__
#define __CRC32_H__

#include <NewKit/Defines.hpp>

#define kCrcCnt (256)

namespace hCore
{
    Int crc32(const Char* crc, Int len);
} // namespace hCore

#endif // !__CRC32_H__