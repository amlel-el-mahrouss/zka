/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */
#pragma once

#include <NewKit/Defines.hpp>

namespace NewOS::PCI
{
	enum class PciConfigKind : UShort
	{
		ConfigAddress = 0xCF8,
		ConfigData	  = 0xCFC,
		Invalid		  = 0xFFF
	};

	class Device final
	{
	public:
		Device() = default;

	public:
		explicit Device(UShort bus, UShort device, UShort function, UShort bar);

		Device& operator=(const Device&) = default;

		Device(const Device&) = default;

		~Device();

	public:
		UInt Read(UInt bar, Size szData);
		void Write(UInt bar, UIntPtr data, Size szData);

	public:
		operator bool();

	public:
		template <typename T>
		UInt Read(UInt bar)
		{
			static_assert(sizeof(T) <= 4, "64-bit PCI addressing is unsupported");
			return Read(bar, sizeof(T));
		}

		template <typename T>
		void Write(UInt bar, UIntPtr data)
		{
			static_assert(sizeof(T) <= 4, "64-bit PCI addressing is unsupported");
			Write(bar, data, sizeof(T));
		}

	public:
		UShort DeviceId();
		UShort VendorId();
		UShort InterfaceId();
		UChar  Class();
		UChar  Subclass();
		UChar  ProgIf();
		UChar  HeaderType();

	public:
		void EnableMmio();
		void BecomeBusMaster(); // for PCI-DMA, PC-DMA does not need that.

		UShort Vendor();

	private:
		UShort fBus;
		UShort fDevice;
		UShort fFunction;
		UShort fBar;
	};
} // namespace NewOS::PCI

EXTERN_C void NewOSPCISetCfgTarget(NewOS::UInt bar);
EXTERN_C NewOS::UInt NewOSPCIReadRaw(NewOS::UInt bar);