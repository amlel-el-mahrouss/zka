/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <HALKit/Alpha/Processor.hpp>

NewOS::Array<void (*)(NewOS::Int32 id, NewOS::HAL::StackFrame*),
			 kKernelMaxSystemCalls>
	kSyscalls;

extern "C" void rt_syscall_handle(NewOS::HAL::StackFrame* stack)
{
	for (NewOS::SizeT index = 0UL; index < kKernelMaxSystemCalls; ++index)
	{
		(kSyscalls[index].Leak().Leak())(stack->ID, stack);
	}
}