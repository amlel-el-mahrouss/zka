/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#include <KernelKit/DebugOutput.h>
#include <NewKit/String.h>
#include <StorageKit/PRDT.h>

namespace Kernel
{
	/// @brief constructs a new PRD.
	/// @param prd PRD reference.
	/// @note This doesnt construct a valid, please fill it by yourself.
	void construct_prdt(Ref<PRDT>& prd)
	{
		prd.Leak().fPhysAddress = 0x0;
		prd.Leak().fSectorCount = 0x0;
		prd.Leak().fEndBit		= 0x0;
	}
} // namespace Kernel