/* -------------------------------------------

	Copyright EL Mahrouss Logic.

------------------------------------------- */

#include <KernelKit/DebugOutput.h>
#include <KernelKit/LPC.h>
#include <KernelKit/Heap.h>
#include <NewKit/Crc32.h>
#include <NewKit/PageMgr.h>
#include <NewKit/Utils.h>

/* -------------------------------------------

 Revision History:
	10/8/24: FIX: Fix useless long name, alongside a new WR (WriteRead) field.
	20/10/24: Fix mm_new_ and mm_delete_ APIs inside Heap.h header. (amlal)

 ------------------------------------------- */

//! @file Heap.cc
//! @brief Kernel's heap manager, serves as the main memory manager.

#define kKernelHeapMagic (0xD4D7D5)
#define kKernelAlignSz	 (__BIGGEST_ALIGNMENT__)

namespace Kernel
{
	/// @brief Contains data structures and algorithms for the heap.
	namespace Detail
	{
		struct PACKED HEAP_INFORMATION_BLOCK;

		/// @brief Kernel heap information block.
		/// Located before the address bytes.
		/// | HIB |  CLASS/STRUCT/DATA TYPES... |
		struct PACKED HEAP_INFORMATION_BLOCK final
		{
			///! @brief 32-bit value which contains the magic number of the heap.
			UInt32 fMagic : 24;

			///! @brief Boolean value which tells if the heap is allocated.
			Boolean fPresent : 1;

			/// @brief Is this valued owned by the user?
			Boolean fWr : 1;

			/// @brief Is this valued owned by the user?
			Boolean fUser : 1;

			/// @brief Is this a page pointer?
			Boolean fPage : 1;

			///! @brief 32-bit CRC checksum.
			UInt32 fCRC32;

			/// @brief 64-bit pointer size.
			SizeT fHeapSize;

			/// @brief 64-bit target pointer.
			UIntPtr fHeapPtr;

			/// @brief Padding bytes for header.
			UInt8 fPadding[kKernelAlignSz];
		};

		/// @brief Check for heap address validity.
		/// @param heap_ptr The address_ptr to check.
		/// @return Bool if the pointer is valid or not.
		auto mm_check_heap_address(VoidPtr heap_ptr) -> Bool
		{
			if (!heap_ptr)
				return false;

			/// Add that check in case we're having an integer underflow. ///

			auto base_heap = (IntPtr)(heap_ptr) - sizeof(Detail::HEAP_INFORMATION_BLOCK);

			if (base_heap < 0)
			{
				return false;
			}

			return true;
		}

		typedef HEAP_INFORMATION_BLOCK* HEAP_INFORMATION_BLOCK_PTR;
	} // namespace Detail

	/// @brief Declare a new size for ptr_heap.
	/// @param ptr_heap the pointer.
	/// @return Newly allocated heap header.
	VoidPtr mm_realloc_heap(VoidPtr ptr_heap, SizeT new_sz)
	{
		if (Detail::mm_check_heap_address(ptr_heap) == No)
			return nullptr;

		if (!ptr_heap || new_sz < 1)
			return nullptr;

		kcout << "This function is not implemented in the kernel, please refrain from using that.\r";
		ke_stop(RUNTIME_CHECK_PROCESS);

		return nullptr;
	}

	/// @brief Allocate chunk of memory.
	/// @param sz Size of pointer
	/// @param wr Read Write bit.
	/// @param user User enable bit.
	/// @return The newly allocated pointer.
	VoidPtr mm_new_heap(const SizeT sz, const bool wr, const bool user)
	{
		auto sz_fix = sz;

		if (sz_fix == 0)
			return nullptr;

		sz_fix += sizeof(Detail::HEAP_INFORMATION_BLOCK);

		PageMgr heap_mgr;
		auto	wrapper = heap_mgr.Request(wr, user, No, sz_fix);

		Detail::HEAP_INFORMATION_BLOCK_PTR heap_info_ptr =
			reinterpret_cast<Detail::HEAP_INFORMATION_BLOCK_PTR>(
				wrapper.VirtualAddress() + sizeof(Detail::HEAP_INFORMATION_BLOCK));

		heap_info_ptr->fHeapSize = sz_fix;
		heap_info_ptr->fMagic	 = kKernelHeapMagic;
		heap_info_ptr->fCRC32	 = No; // dont fill it for now.
		heap_info_ptr->fHeapPtr	 = reinterpret_cast<UIntPtr>(heap_info_ptr) + sizeof(Detail::HEAP_INFORMATION_BLOCK);
		heap_info_ptr->fPage	 = No;
		heap_info_ptr->fWr		 = wr;
		heap_info_ptr->fUser	 = user;
		heap_info_ptr->fPresent	 = Yes;

		rt_set_memory(heap_info_ptr->fPadding, 0, kKernelAlignSz);

		auto result = reinterpret_cast<VoidPtr>(heap_info_ptr->fHeapPtr);

		kcout << "Created Heap address: " << hex_number(reinterpret_cast<UIntPtr>(heap_info_ptr)) << endl;

		return result;
	}

	/// @brief Makes a page heap.
	/// @param heap_ptr the pointer to make a page heap.
	/// @return kErrorSuccess if successful, otherwise an error code.
	Int32 mm_make_ke_page(VoidPtr heap_ptr)
	{
		if (Detail::mm_check_heap_address(heap_ptr) == No)
			return -kErrorHeapNotPresent;

		Detail::HEAP_INFORMATION_BLOCK_PTR heap_info_ptr =
			reinterpret_cast<Detail::HEAP_INFORMATION_BLOCK_PTR>(
				(UIntPtr)heap_ptr - sizeof(Detail::HEAP_INFORMATION_BLOCK));

		if (!heap_ptr)
			return -kErrorHeapNotPresent;

		heap_info_ptr->fPage = true;

		kcout << "Created Page address: " << hex_number(reinterpret_cast<UIntPtr>(heap_info_ptr)) << endl;

		return kErrorSuccess;
	}

	/// @brief Declare pointer as free.
	/// @param heap_ptr the pointer.
	/// @return
	Int32 mm_delete_heap(VoidPtr heap_ptr)
	{
		if (Detail::mm_check_heap_address(heap_ptr) == No)
			return -kErrorHeapNotPresent;

		Detail::HEAP_INFORMATION_BLOCK_PTR heap_info_ptr =
			reinterpret_cast<Detail::HEAP_INFORMATION_BLOCK_PTR>(
				(UIntPtr)(heap_ptr) - sizeof(Detail::HEAP_INFORMATION_BLOCK));

		if (heap_info_ptr && heap_info_ptr->fMagic == kKernelHeapMagic)
		{
			if (!heap_info_ptr->fPresent)
			{
				return -kErrorHeapNotPresent;
			}

			if (heap_info_ptr->fCRC32 != 0)
			{
				if (heap_info_ptr->fCRC32 !=
					ke_calculate_crc32((Char*)heap_info_ptr->fHeapPtr,
									   heap_info_ptr->fHeapSize))
				{
					if (!heap_info_ptr->fUser)
					{
						ke_stop(RUNTIME_CHECK_POINTER);
					}
				}
			}

			heap_info_ptr->fHeapSize = 0UL;
			heap_info_ptr->fPresent	= No;
			heap_info_ptr->fHeapPtr	= 0;
			heap_info_ptr->fCRC32	= 0;
			heap_info_ptr->fWr		= No;
			heap_info_ptr->fUser		= No;
			heap_info_ptr->fMagic	= 0;

			PTEWrapper		pageWrapper(No, No, No, reinterpret_cast<UIntPtr>(heap_info_ptr) - sizeof(Detail::HEAP_INFORMATION_BLOCK));
			Ref<PTEWrapper> pteAddress{pageWrapper};

			PageMgr heap_mgr;
			heap_mgr.Free(pteAddress);

			kcout << "Freed Heap address: " << hex_number(reinterpret_cast<UIntPtr>(heap_info_ptr)) << endl;

			return kErrorSuccess;
		}

		return -kErrorInternal;
	}

	/// @brief Check if pointer is a valid Kernel pointer.
	/// @param heap_ptr the pointer
	/// @return if it exists.
	Boolean mm_is_valid_heap(VoidPtr heap_ptr)
	{
		if (heap_ptr)
		{
			Detail::HEAP_INFORMATION_BLOCK_PTR heap_info_ptr =
				reinterpret_cast<Detail::HEAP_INFORMATION_BLOCK_PTR>(
					(UIntPtr)(heap_ptr) - sizeof(Detail::HEAP_INFORMATION_BLOCK));

			if (heap_info_ptr && heap_info_ptr->fPresent && heap_info_ptr->fMagic == kKernelHeapMagic)
			{
				return Yes;
			}
		}

		return No;
	}

	/// @brief Protect the heap with a CRC value.
	/// @param heap_ptr HIB pointer.
	/// @return if it valid: point has crc now., otherwise fail.
	Boolean mm_protect_heap(VoidPtr heap_ptr)
	{
		if (heap_ptr)
		{
			Detail::HEAP_INFORMATION_BLOCK_PTR heap_info_ptr =
				reinterpret_cast<Detail::HEAP_INFORMATION_BLOCK_PTR>(
					(UIntPtr)heap_ptr - sizeof(Detail::HEAP_INFORMATION_BLOCK));

			if (heap_ptr && heap_info_ptr->fPresent && kKernelHeapMagic == heap_info_ptr->fMagic)
			{
				heap_info_ptr->fCRC32 =
					ke_calculate_crc32((Char*)heap_info_ptr->fHeapPtr, heap_info_ptr->fHeapSize);

				return Yes;
			}
		}

		return No;
	}
} // namespace Kernel