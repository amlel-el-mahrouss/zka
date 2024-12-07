/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <HALKit/AMD64/Processor.h>
#include <KernelKit/UserProcessScheduler.h>

namespace Kernel
{
	/***********************************************************************************/
	/// @brief Unimplemented function (crashes by default)
	/// @param void
	/***********************************************************************************/

	EXTERN_C Void __zka_pure_call(void)
	{
		UserProcessScheduler::The().GetCurrentProcess().Leak().Crash();
	}

	/***********************************************************************************/
	/// @brief Validate user stack.
	/// @param stack_ptr the frame pointer.
	/***********************************************************************************/

	Bool hal_check_stack(HAL::StackFramePtr stack_ptr)
	{
		if (!stack_ptr)
			return No;

		return stack_ptr->SP != 0 && stack_ptr->BP != 0;
	}
} // namespace Kernel