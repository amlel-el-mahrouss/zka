/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/String.hpp>

namespace HCore {
class MacAddressGetter;

/// \brief This retrieves the MAC address of the device.
/// \note Checks on \Mount\Devices\MacAddr
class MacAddressGetter final {
 public:
  explicit MacAddressGetter() = default;

 public:
  StringView& AsString();
  Array<WideChar, 12>& AsBytes();
};

}  // namespace HCore