/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceManager.hpp>
#include <KernelKit/DriveManager.hxx>
#include <NewKit/OwnPtr.hpp>

namespace NewOS
{
	class AHCIDeviceInterface : public DeviceInterface<MountpointInterface*>
	{
	public:
		explicit AHCIDeviceInterface(void (*Out)(MountpointInterface* outpacket),
									 void (*In)(MountpointInterface* inpacket),
									 void (*Cleanup)(void));

		virtual ~AHCIDeviceInterface();

	public:
		AHCIDeviceInterface& operator=(const AHCIDeviceInterface&) = default;
		AHCIDeviceInterface(const AHCIDeviceInterface&)			   = default;

		const char* Name() const override;

	private:
		void (*fCleanup)(void);
	};
} // namespace NewOS