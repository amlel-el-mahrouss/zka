/* -------------------------------------------

	Copyright ZKA Technologies

	Purpose: DDK Text I/O.

------------------------------------------- */

#include <DDKit/KernelDev.h>
#include <DDKit/KernelString.h>

/// @brief Open a new binary device from path.
DK_EXTERN kernelDeviceRef kernelOpenDevice(const char* devicePath)
{
	if (!devicePath)
		return nil;

	return kernelCall("OpenDevice", 1, (void*)devicePath, kernelStringLength(devicePath));
}

/// @brief Close any device.
/// @param device valid device.
DK_EXTERN void kernelCloseDevice(kernelDeviceRef device)
{
	if (!device)
		return;

	kernelCall("CloseDevice", 1, device, sizeof(kernelDevice));
}