/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

//! @file ThreadLocalStorage.inl
//! @brief Allocate resources from the process's heap storage.

#ifndef _INC_PROCESS_SCHEDULER_HXX_
#include <KernelKit/UserProcessScheduler.hxx>
#endif

template <typename T>
inline T* tls_new_ptr(void) noexcept
{
	using namespace Kernel;

	auto ref_process = UserProcessScheduler::The().CurrentProcess();
	MUST_PASS(ref_process);

	T* pointer = (T*)ref_process.Leak().New(sizeof(T));
	return pointer;
}

//! @brief TLS delete implementation.
template <typename T>
inline Kernel::Bool tls_delete_ptr(T* ptr) noexcept
{
	if (!ptr)
		return false;

	using namespace Kernel;

	auto ref_process = UserProcessScheduler::The().CurrentProcess();
	MUST_PASS(ref_process);

	return ref_process.Leak().Delete(ptr, sizeof(T));
}

/// @brief Allocate a C++ class, and then call the constructor of it.
/// @tparam T
/// @tparam ...Args
/// @param ...args
/// @return
template <typename T, typename... Args>
T* tls_new_class(Args&&... args)
{
	T* ptr = tls_new_ptr<T>();

	using namespace Kernel;

	if (ptr)
	{
		*ptr = T(forward(args)...);
		return ptr;
	}

	return nullptr;
}

/// @brief Delete a C++ class (call constructor first.)
/// @tparam T
/// @param ptr
/// @return
template <typename T>
inline Kernel::Bool tls_delete_class(T* ptr)
{
	if (!ptr)
		return false;

	ptr->~T();
	return tls_delete_ptr(ptr);
}