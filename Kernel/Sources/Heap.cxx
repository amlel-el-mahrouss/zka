/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/LPC.hxx>
#include <KernelKit/Heap.hxx>
#include <NewKit/Crc32.hpp>
#include <NewKit/PageManager.hpp>

//! @file KernelHeap.cxx
//! @brief Kernel heap allocator.

#define kKernelHeapMagic		   (0xD4D7D5)
#define kKernelHeapHeaderPaddingSz (16U)

namespace Kernel
{
	STATIC SizeT	   kHeapCount = 0UL;
	STATIC PageManager kHeapPageManager;

	namespace Detail
	{
		/// @brief Kernel heap information block.
		/// Located before the address bytes.
		/// | HIB |  ADDRESS  |
		struct PACKED HeapInformationBlock final
		{
			///! @brief 32-bit value which contains the magic number of the executable.
			UInt32 fMagic;
			///! @brief Boolean value which tells if the pointer is allocated.
			Boolean fPresent;
			///! @brief 32-bit CRC checksum
			UInt32 fCRC32;
			/// @brief 64-bit pointer size.
			SizeT fTargetPtrSize;
			/// @brief 64-bit target pointer.
			UIntPtr fTargetPtr;
			/// @brief Is this a page pointer?
			Boolean fPagePtr;
			/// @brief Padding bytes for header.
			UInt8 fPadding[kKernelHeapHeaderPaddingSz];
		};

		typedef HeapInformationBlock* HeapInformationBlockPtr;
	} // namespace Detail

	/// @brief Declare a new size for allocatedPtr.
	/// @param allocatedPtr the pointer.
	/// @return
	voidPtr ke_realloc_ke_heap(voidPtr allocatedPtr, SizeT newSz)
	{
		if (!allocatedPtr || newSz < 1)
			return nullptr;

		Detail::HeapInformationBlockPtr heapInfoBlk =
			reinterpret_cast<Detail::HeapInformationBlockPtr>(
				(UIntPtr)allocatedPtr - sizeof(Detail::HeapInformationBlock));

		heapInfoBlk->fTargetPtrSize = newSz;

		if (heapInfoBlk->fCRC32 > 0)
		{
			MUST_PASS(ke_protect_ke_heap(allocatedPtr));
		}

		return allocatedPtr;
	}

	/// @brief allocate chunk of memory.
	/// @param sz size of pointer
	/// @param rw read write (true to enable it)
	/// @param user is it accesible by user processes?
	/// @return the pointer
	VoidPtr ke_new_ke_heap(const SizeT sz, const bool rw, const bool user)
	{
		auto szFix = sz;

		if (szFix == 0)
			++szFix;

		auto wrapper = kHeapPageManager.Request(rw, user, false, szFix);

		Detail::HeapInformationBlockPtr heapInfo =
			reinterpret_cast<Detail::HeapInformationBlockPtr>(
				wrapper.VirtualAddress());

		heapInfo->fTargetPtrSize = szFix;
		heapInfo->fMagic		 = kKernelHeapMagic;
		heapInfo->fCRC32		 = 0; // dont fill it for now.
		heapInfo->fTargetPtr	 = wrapper.VirtualAddress();
		heapInfo->fPagePtr		 = 0;

		++kHeapCount;

		return reinterpret_cast<VoidPtr>(wrapper.VirtualAddress() +
										 sizeof(Detail::HeapInformationBlock));
	}

	/// @brief Makes a page heap.
	/// @param heapPtr
	/// @return
	Int32 ke_make_ke_page(VoidPtr heapPtr)
	{
		if (kHeapCount < 1)
			return -kErrorInternal;
		if (((IntPtr)heapPtr - sizeof(Detail::HeapInformationBlock)) <= 0)
			return -kErrorInternal;
		if (((IntPtr)heapPtr - kBadPtr) < 0)
			return -kErrorInternal;

		Detail::HeapInformationBlockPtr heapInfoBlk =
			reinterpret_cast<Detail::HeapInformationBlockPtr>(
				(UIntPtr)heapPtr - sizeof(Detail::HeapInformationBlock));

		heapInfoBlk->fPagePtr = 1;

		return 0;
	}

	/// @brief Declare pointer as free.
	/// @param heapPtr the pointer.
	/// @return
	Int32 ke_delete_ke_heap(VoidPtr heapPtr)
	{
		if (kHeapCount < 1)
			return -kErrorInternal;
		if (((IntPtr)heapPtr - sizeof(Detail::HeapInformationBlock)) <= 0)
			return -kErrorInternal;
		if (((IntPtr)heapPtr - kBadPtr) < 0)
			return -kErrorInternal;

		Detail::HeapInformationBlockPtr heapInfoBlk =
			reinterpret_cast<Detail::HeapInformationBlockPtr>(
				(UIntPtr)heapPtr - sizeof(Detail::HeapInformationBlock));

		if (heapInfoBlk && heapInfoBlk->fMagic == kKernelHeapMagic)
		{
			if (!heapInfoBlk->fPresent)
			{
				return -kErrorHeapNotPresent;
			}

			if (heapInfoBlk->fCRC32 != 0)
			{
				if (heapInfoBlk->fCRC32 !=
					ke_calculate_crc32((Char*)heapInfoBlk->fTargetPtr,
									   heapInfoBlk->fTargetPtrSize))
				{
					ke_stop(RUNTIME_CHECK_POINTER);
				}
			}

			heapInfoBlk->fTargetPtrSize = 0UL;
			heapInfoBlk->fPresent		= false;
			heapInfoBlk->fTargetPtr		= 0;
			heapInfoBlk->fCRC32			= 0;
			heapInfoBlk->fMagic			= 0;

			PTEWrapper		 pageWrapper(false, false, false, reinterpret_cast<UIntPtr>(heapInfoBlk));
			Ref<PTEWrapper*> pteAddress{&pageWrapper};

			kHeapPageManager.Free(pteAddress);

			--kHeapCount;
			return 0;
		}

		return -kErrorInternal;
	}

	/// @brief Check if pointer is a valid kernel pointer.
	/// @param heapPtr the pointer
	/// @return if it exists.
	Boolean ke_is_valid_heap(VoidPtr heapPtr)
	{
		if (kHeapCount < 1)
			return false;

		if (heapPtr)
		{
			Detail::HeapInformationBlockPtr virtualAddress =
				reinterpret_cast<Detail::HeapInformationBlockPtr>(
					(UIntPtr)heapPtr - sizeof(Detail::HeapInformationBlock));

			if (virtualAddress->fPresent && virtualAddress->fMagic == kKernelHeapMagic)
			{
				return true;
			}
		}

		return false;
	}

	/// @brief Protect the heap with a CRC value.
	/// @param heapPtr HIB pointer.
	/// @return if it valid: point has crc now., otherwise fail.
	Boolean ke_protect_ke_heap(VoidPtr heapPtr)
	{
		if (heapPtr)
		{
			Detail::HeapInformationBlockPtr heapInfoBlk =
				reinterpret_cast<Detail::HeapInformationBlockPtr>(
					(UIntPtr)heapPtr - sizeof(Detail::HeapInformationBlock));

			if (heapInfoBlk->fPresent && kKernelHeapMagic == heapInfoBlk->fMagic)
			{
				heapInfoBlk->fCRC32 =
					ke_calculate_crc32((Char*)heapInfoBlk->fTargetPtr, heapInfoBlk->fTargetPtrSize);

				return true;
			}
		}

		return false;
	}
} // namespace Kernel