/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#ifndef __SMP_MANAGER__
#define __SMP_MANAGER__

#include <ArchKit/ArchKit.hpp>
#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/Ref.hpp>

// Last Rev
// Sat Feb 24 CET 2024

#define kMaxHarts 8

namespace NewOS
{
	using ThreadID = UInt32;

	enum ThreadKind
	{
		kHartSystemReserved, // System reserved thread, well user can't use it
		kHartStandard,		 // user thread, cannot be used by kernel
		kHartFallback,		 // fallback thread, cannot be used by user if not clear or
							 // used by kernel.
		kHartBoot,			 // The core we booted from, the mama.
		kInvalidHart,
		kHartCount,
	};

	typedef enum ThreadKind SmThreadKind;
	typedef ThreadID		SmThreadID;

	///
	/// \name HardwareThread
	/// @brief CPU Hardware Thread (POWER, x64, or 64x0)
	///

	class HardwareThread final
	{
	public:
		explicit HardwareThread();
		~HardwareThread();

	public:
		NEWOS_COPY_DEFAULT(HardwareThread)

	public:
		operator bool();

	public:
		void Wake(const bool wakeup = false) noexcept;
		void Busy(const bool busy = false) noexcept;

	public:
		bool Switch(HAL::StackFrame* stack);
		bool IsWakeup() noexcept;

	public:
		HAL::StackFrame*  StackFrame() noexcept;
		const ThreadKind& Kind() noexcept;
		bool			  IsBusy() noexcept;
		const ThreadID&	  ID() noexcept;

	private:
		HAL::StackFrame* fStack;
		ThreadKind		 fKind;
		ThreadID		 fID;
		bool			 fWakeup;
		bool			 fBusy;
		Int64			 fPID;

	private:
		friend class SMPManager;
	};

	///
	/// \name SMPManager
	/// @brief Multi processor manager to manage other cores and dispatch tasks.
	///

	class SMPManager final
	{
	private:
		explicit SMPManager();

	public:
		~SMPManager();

	public:
		NEWOS_COPY_DEFAULT(SMPManager);

	public:
		bool			   Switch(HAL::StackFrame* the);
		HAL::StackFramePtr GetStackFrame() noexcept;

	public:
		Ref<HardwareThread> operator[](const SizeT& idx);
		bool				operator!() noexcept;
							operator bool() noexcept;

	public:
		/// @brief Shared instance of the SMP Manager.
		/// @return the reference to the smp manager.
		static Ref<SMPManager> Shared();

	public:
		/// @brief Returns the amount of threads present in the system.
		/// @returns SizeT the amount of cores present.
		SizeT Count() noexcept;

	private:
		Array<HardwareThread, kMaxHarts> fThreadList;
		ThreadID						 fCurrentThread{0};
	};

	/// @brief wakes up thread.
	/// wakes up thread from hang.
	Void rt_wakeup_thread(HAL::StackFramePtr stack);

	/// @brief makes thread sleep.
	/// hooks and hangs thread to prevent code from executing.
	Void rt_hang_thread(HAL::StackFramePtr stack);
} // namespace NewOS

#endif // !__SMP_MANAGER__