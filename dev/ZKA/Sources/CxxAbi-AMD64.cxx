/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#ifdef __NEWOS_AMD64__

#include <KernelKit/DebugOutput.hxx>
#include <NewKit/CxxAbi.hxx>
#include <KernelKit/LPC.hxx>

atexit_func_entry_t __atexit_funcs[kDSOMaxObjects];

uarch_t __atexit_func_count;

/// @brief Dynamic Shared Object Handle.
Kernel::UIntPtr __dso_handle;

EXTERN_C void __cxa_pure_virtual()
{
	Kernel::kcout << "newoskrnl: C++ placeholder method.\n";
}

EXTERN_C void ___chkstk_ms(void)
{
	Kernel::err_bug_check_raise();
	Kernel::err_bug_check();
}

EXTERN_C int atexit(void (*f)(void*), void* arg, void* dso)
{
	if (__atexit_func_count >= kDSOMaxObjects)
		return -1;

	__atexit_funcs[__atexit_func_count].destructor_func = f;
	__atexit_funcs[__atexit_func_count].obj_ptr			= arg;
	__atexit_funcs[__atexit_func_count].dso_handle		= dso;

	__atexit_func_count++;

	return 0;
}

EXTERN_C void __cxa_finalize(void* f)
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
	EXTERN_C int __cxa_guard_acquire(__guard* g)
	{
		(void)g;
		return 0;
	}

	EXTERN_C int __cxa_guard_release(__guard* g)
	{
		*(char*)g = 1;
		return 0;
	}

	EXTERN_C void __cxa_guard_abort(__guard* g)
	{
		(void)g;
	}
} // namespace cxxabiv1

#endif // ifdef __NEWOS_AMD64__