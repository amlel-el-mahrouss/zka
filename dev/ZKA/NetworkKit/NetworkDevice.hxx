/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#ifndef __INC_NETWORK_DEVICE_HPP__
#define __INC_NETWORK_DEVICE_HPP__

#include <KernelKit/DeviceManager.hxx>
#include <NetworkKit/IP.hxx>

/// @note Can either work with: Ethernet, GPRS, WiFi

namespace Kernel
{
	struct NetworkDeviceCommand;
	class NetworkDevice;

	/**
	 * \brief Network device interface, establishes a connection to the NIC.
	 */
	class NetworkDevice final : public DeviceInterface<NetworkDeviceCommand>
	{
	public:
		NetworkDevice(void (*out)(NetworkDeviceCommand),
					  void (*in)(NetworkDeviceCommand),
					  void (*onCleanup)(void) = nullptr);

		~NetworkDevice() override;

	public:
		NetworkDevice& operator=(const NetworkDevice&) = default;
		NetworkDevice(const NetworkDevice&)			   = default;

	public:
		const char* Name() const override;
		Boolean		Name(const char* strView);

	private:
		static constexpr auto cNetworkNameLen = 512;

		Void (*fCleanup)(void);
		Char fNetworkName[cNetworkNameLen];
	};

	struct NetworkDeviceCommand final
	{
		UInt32	CommandName;
		UInt32	CommandType;
		UInt32	CommandFlags;
		VoidPtr CommandBuffer;
		SizeT	CommandSizeBuffer;
	};

	/// @brief TCP device.
	using TCPNetworkDevice = NetworkDevice;

	/// @brief UDP device.
	using UDPNetworkDevice = NetworkDevice;

	/// @brief PPP device.
	using PPPNetworkDevice = NetworkDevice;

	/// @brief IPC device.
	using IPCEPNetworkDevice = NetworkDevice;

	/// @brief GRPS device.
	using GPRSNetworkDevice = NetworkDevice;

	/// @brief GSM device.
	using GSMNetworkDevice = NetworkDevice;

	/// @brief LTE device.
	using LTENetworkDevice = NetworkDevice;
} // namespace Kernel

#include <NetworkKit/NetworkDevice.inl>

#endif // !__INC_NETWORK_DEVICE_HPP__