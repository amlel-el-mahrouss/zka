/*
 *      ========================================================
 *
 *      Kernel
 *      Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved., all rights reserved.
 *
 *      ========================================================
 */

#ifndef __KERNELKIT_SHARED_OBJECT_H__
#define __KERNELKIT_SHARED_OBJECT_H__

#include <KernelKit/PEF.h>
#include <NewKit/Defines.h>
#include <KernelKit/PEFCodeMgr.h>
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/IDLLObject.h>

namespace Kernel
{
	/**
	 * @brief Shared Library class
	 * Load library from this class
	 */
	class IPEFDLLObject final ZKA_DLL_OBJECT
	{
	public:
		explicit IPEFDLLObject() = default;
		~IPEFDLLObject()		 = default;

	public:
		ZKA_COPY_DEFAULT(IPEFDLLObject);

	private:
		DLL_TRAITS* fMounted{nullptr};

	public:
		DLL_TRAITS** GetAddressOf()
		{
			return &fMounted;
		}

		DLL_TRAITS* Get()
		{
			return fMounted;
		}

	public:
		void Mount(DLL_TRAITS* to_mount)
		{
			if (!to_mount || !to_mount->ImageObject)
				return;

			fMounted = to_mount;

			if (fLoader && to_mount)
			{
				delete fLoader;
				fLoader = nullptr;
			}

			if (!fLoader)
			{
				fLoader = new PEFLoader(fMounted->ImageObject);
			}
		}

		void Unmount()
		{
			if (fMounted)
				fMounted = nullptr;
		};

		template <typename SymbolType>
		SymbolType Load(const Char* symbol_name, SizeT len, Int32 kind)
		{
			if (symbol_name == nullptr || *symbol_name == 0)
				return nullptr;
			if (len > kPathLen || len < 1)
				return nullptr;

			auto ret =
				reinterpret_cast<SymbolType>(fLoader->FindSymbol(symbol_name, kind));

			if (!ret)
			{
				if (kind == kPefCode)
					return (VoidPtr)&__zka_pure_call;

				return nullptr;
			}

			return ret;
		}

	private:
		PEFLoader* fLoader{nullptr};
	};

	typedef IPEFDLLObject* IDLL;

	EXTERN_C IDLL rtl_init_dll(UserProcess* header);
	EXTERN_C Void rtl_fini_dll(UserProcess* header, IDLL lib, Bool* successful);
} // namespace Kernel

#endif /* ifndef __KERNELKIT_SHARED_OBJECT_H__ */
