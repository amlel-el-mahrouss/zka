/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

//! @file DebuggerPort.cxx
//! @brief UART debug via packets.

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/DebugOutput.hpp>

// after that we have start of additional data.

namespace NewOS
{
	void rt_debug_listen(DebuggerPortHeader* theHook) noexcept
	{
		if (theHook == nullptr)
			return;

		for (UInt32 i = 0U; i < kDebugMaxPorts; ++i)
		{
			HAL::Out16(theHook->fPort[i], kDebugMag0);
			HAL::rt_wait_400ns();

			HAL::Out16(theHook->fPort[i], kDebugMag1);
			HAL::rt_wait_400ns();

			HAL::Out16(theHook->fPort[i], kDebugMag2);
			HAL::rt_wait_400ns();

			HAL::Out16(theHook->fPort[i], kDebugMag3);
			HAL::rt_wait_400ns();

			if (HAL::In16(theHook->fPort[i] != kDebugUnboundPort))
				theHook->fBoundCnt++;
		}
	}
} // namespace NewOS