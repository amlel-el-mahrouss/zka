/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <KernelKit/DebugOutput.hpp>

#include <ArchKit/Arch.hpp>
#include <NewKit/Utils.hpp>

namespace hCore
{
    namespace Detail
    {
        constexpr short PORT = 0x3F8;

        bool serial_init()
        {
            HAL::out8(PORT + 1, 0x00); // Disable all interrupts
            HAL::out8(PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
            HAL::out8(PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
            HAL::out8(PORT + 1, 0x00); //                  (hi byte)
            HAL::out8(PORT + 3, 0x03); // 8 bits, no parity, one stop bit
            HAL::out8(PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
            HAL::out8(PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
            HAL::out8(PORT + 4, 0x1E); // Set in loopback mode, test the serial chip
            HAL::out8(PORT + 0, 0xAE); // Test serial chip (send byte 0xAE and check if
                                       // serial returns same byte)

            // Check if serial is faulty (i.e: not same byte as sent)
            if (HAL::in8(PORT) != 0xAE)
            {
#ifdef __DEBUG__
                panic(RUNTIME_CHECK_HANDSHAKE);
#else
                return false;
#endif
            }

            // If serial is not faulty set it in normal operation mode
            // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
            HAL::out8(Detail::PORT + 4, 0x0F);

            return true;
        }
    }

    void system_io_print(const char *bytes)
    {
        if (!bytes)
            return;

        SizeT index = 0;
        SizeT len = string_length(bytes, 256);

        while (index < len)
        {
            HAL::out8(Detail::PORT, bytes[index]);
            ++index;
        }
    }

    TerminalDevice kcout(hCore::system_io_print, nullptr);
} // namespace hCore