/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/Arch.hpp>
#include <KernelKit/DebugOutput.hpp>
#include <NewKit/PageAllocator.hpp>

// empty for now.
namespace hCore::Detail
{
    UIntPtr create_page_wrapper(Boolean rw, Boolean user)
    {
        auto addr = HAL::hal_create_page(rw, user);

        if (addr == kBadAddress)
        {
            kcout << "[create_page_wrapper] kBadAddress returned\n";
            panic(RUNTIME_CHECK_POINTER);
        }

        return addr;
    }

    void exec_disable(UIntPtr VirtualAddr)
    {
        PTE *VirtualAddrTable = reinterpret_cast<PTE*>(VirtualAddr);
        MUST_PASS(!VirtualAddrTable->Accessed);
        VirtualAddrTable->ExecDisable = true;

        flush_tlb(VirtualAddr);
    }

    bool page_disable(UIntPtr VirtualAddr)
    {
        if (VirtualAddr) {
            auto VirtualAddrTable = (PTE * )(VirtualAddr);
            MUST_PASS(!VirtualAddrTable->Accessed);

            if (VirtualAddrTable->Accessed)
                return false;
            VirtualAddrTable->Present = false;

            flush_tlb(VirtualAddr);

            return true;
        }

        return false;
    }
} // namespace hCore::Detail