/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <KernelKit/DebugOutput.hpp>
#include <NewKit/CxxAbi.hpp>
#include <NewKit/KernelCheck.hpp>

atexit_func_entry_t __atexit_funcs[kDSOMaxObjects];

uarch_t __atexit_func_count;

extern "C" void __cxa_pure_virtual()
{
	NewOS::kcout << "newoskrnl: C++ placeholder method.\n";
}

extern "C" void ___chkstk_ms()
{
	while (1)
	{
		asm("cli");
		asm("hlt");
	}
}

extern "C" int atexit(void (*f)(void*), void* arg, void* dso)
{
	if (__atexit_func_count >= kDSOMaxObjects)
		return -1;

	__atexit_funcs[__atexit_func_count].destructor_func = f;
	__atexit_funcs[__atexit_func_count].obj_ptr			= arg;
	__atexit_funcs[__atexit_func_count].dso_handle		= dso;

	__atexit_func_count++;

	return 0;
}

extern "C" void __cxa_finalize(void* f)
{
	uarch_t i = __atexit_func_count;
	if (!f)
	{
		while (i--)
		{
			if (__atexit_funcs[i].destructor_func)
			{
				(*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
			};
		}

		return;
	}

	while (i--)
	{
		if (__atexit_funcs[i].destructor_func)
		{
			(*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
			__atexit_funcs[i].destructor_func = 0;
		};
	}
}

namespace cxxabiv1
{
	extern "C" int __cxa_guard_acquire(__guard* g)
	{
		(void)g;
		return 0;
	}

	extern "C" int __cxa_guard_release(__guard* g)
	{
		*(char*)g = 1;
		return 0;
	}

	extern "C" void __cxa_guard_abort(__guard* g)
	{
		(void)g;
	}
} // namespace cxxabiv1