/* -------------------------------------------

	Copyright EL Mahrouss Logic.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/HardwareThreadScheduler.h>
#include <CFKit/Property.h>

///! BUGS: 0

///! @file MP.cc
///! @brief This file handles multi processing in the Kernel.
///! @brief Multi processing is needed for multi-tasking operations.

namespace Kernel
{
	HardwareThreadScheduler kHardwareThreadScheduler;

	///! A HardwareThread class takes care of it's owned hardware thread.
	///! It has a stack for it's core.

	///! @brief C++ constructor.
	HardwareThread::HardwareThread() = default;

	///! @brief C++ destructor.
	HardwareThread::~HardwareThread() = default;

	//! @brief returns the id of the thread.
	const ThreadID& HardwareThread::ID() noexcept
	{
		return fID;
	}

	//! @brief returns the kind of thread we have.
	const ThreadKind& HardwareThread::Kind() noexcept
	{
		return fKind;
	}

	//! @brief is the thread busy?
	//! @return whether the thread is busy or not.
	Bool HardwareThread::IsBusy() noexcept
	{
		STATIC Int64 busy_timer = 0U;

		if (busy_timer > this->fPTime)
		{
			busy_timer = 0U;
			fBusy	   = No;
		}

		++busy_timer;

		return fBusy;
	}

	/// @brief Get processor stack frame.

	HAL::StackFramePtr HardwareThread::StackFrame() noexcept
	{
		MUST_PASS(fStack);
		return fStack;
	}

	Void HardwareThread::Busy(const Bool busy) noexcept
	{
		fBusy = busy;
	}

	HardwareThread::operator bool()
	{
		return fStack;
	}

	/// @brief Wakeup the processor.

	Void HardwareThread::Wake(const bool wakeup) noexcept
	{
		fWakeup = wakeup;

		if (!fWakeup)
			mp_hang_thread(fStack);
		else
			mp_wakeup_thread(fStack);
	}

	/// @note Those symbols are needed in order to switch and validate the stack.

	EXTERN Bool	  hal_check_stack(HAL::StackFramePtr stackPtr);
	EXTERN_C Bool mp_register_process(VoidPtr image, UInt8* stack_ptr, HAL::StackFramePtr frame_ptr);

	/// @brief Switch to hardware thread.
	/// @param stack the new hardware thread.
	/// @retval true stack was changed, code is running.
	/// @retval false stack is invalid, previous code is running.
	Bool HardwareThread::Switch(VoidPtr image, UInt8* stack_ptr, HAL::StackFramePtr frame)
	{
		if (!frame ||
			!image ||
			!stack_ptr)
			return No;

		if (!this->IsWakeup())
			return No;

		if (this->IsBusy())
			return No;

		fStack = frame;

		Bool ret = mp_register_process(image, stack_ptr, fStack);

		if (ret)
			this->Busy(true);

		return ret;
	}

	///! @brief Tells if processor is waked up.
	bool HardwareThread::IsWakeup() noexcept
	{
		return fWakeup;
	}

	///! @brief Constructor and destructors.

	///! @brief Default constructor.
	HardwareThreadScheduler::HardwareThreadScheduler() = default;

	///! @brief Default destructor.
	HardwareThreadScheduler::~HardwareThreadScheduler() = default;

	/// @brief Shared singleton function
	HardwareThreadScheduler& HardwareThreadScheduler::The()
	{
		return kHardwareThreadScheduler;
	}

	/// @brief Get Stack Frame of Core
	HAL::StackFramePtr HardwareThreadScheduler::Leak() noexcept
	{
		return fThreadList[fCurrentThread].fStack;
	}

	/**
	 * Get Hardware thread at index.
	 * @param idx the index
	 * @return the reference to the hardware thread.
	 */
	Ref<HardwareThread*> HardwareThreadScheduler::operator[](const SizeT& idx)
	{
		if (idx == 0)
		{
			if (fThreadList[idx].Kind() != kHartSystemReserved)
			{
				fThreadList[idx].fKind = kHartBoot;
			}
		}
		else if (idx >= cMaxHartInsideSched)
		{
			static HardwareThread* fakeThread = nullptr;
			return {fakeThread};
		}

		return &fThreadList[idx];
	}

	/**
	 * Check if thread pool isn't empty.
	 * @return
	 */
	HardwareThreadScheduler::operator bool() noexcept
	{
		return !fThreadList.Empty();
	}

	/**
	 * Reverse operator bool
	 * @return
	 */
	bool HardwareThreadScheduler::operator!() noexcept
	{
		return fThreadList.Empty();
	}

	/// @brief Returns the amount of core present.
	/// @return the number of cores.
	SizeT HardwareThreadScheduler::Count() noexcept
	{
		return fThreadList.Capacity();
	}
} // namespace Kernel