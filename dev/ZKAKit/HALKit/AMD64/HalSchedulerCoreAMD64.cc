/* -------------------------------------------

	Copyright EL Mahrouss Logic.

------------------------------------------- */

#include <HALKit/AMD64/Processor.h>
#include <KernelKit/UserProcessScheduler.h>

namespace Kernel
{
	/***********************************************************************************/
	/// @brief Unimplemented function (crashes by default)
	/// @param
	/***********************************************************************************/

	EXTERN_C Void __zka_pure_call(void)
	{
		UserProcessScheduler::The().GetCurrentProcess().Leak().Crash();
	}

	Bool hal_check_stack(HAL::StackFramePtr stack_ptr)
	{
		if (!stack_ptr)
			return false;

		return true;
	}

	/// @brief Wakes up thread.
	/// Wakes up thread from the hang state.
	Void mp_wakeup_thread(HAL::StackFrame* stack)
	{
		Kernel::UserProcessHelper::StartScheduling();
	}

	/// @brief makes the thread sleep on a loop.
	/// hooks and hangs thread to prevent code from executing.
	Void mp_hang_thread(HAL::StackFrame* stack)
	{
		while (Yes)
		{
			/* Nothing to do, code is spinning */
		}
	}
} // namespace Kernel