/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

/*
 *
 * @file Platform.cxx
 * @brief Platform Specific Functions.
 *
 */

#include <BootKit/BootKit.hxx>
#include <BootKit/Platform.hxx>
#include <EFIKit/Api.hxx>

extern "C" void rt_halt() { asm volatile("hlt"); }

extern "C" void rt_cli() { asm volatile("cli"); }

extern "C" void rt_sti() { asm volatile("sti"); }

extern "C" void rt_cld() { asm volatile("cld"); }

extern "C" void rt_std() { asm volatile("std"); }

/// @brief Stack Checker, leave empty.

extern "C" void ___chkstk_ms(void) {}

namespace EFI {
Void Stop() noexcept {
  while (true) {
    rt_cli();
    rt_halt();
  }
}

}  // namespace EFI