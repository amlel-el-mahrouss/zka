/*
 * ========================================================
 *
 * hCore
 * Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 *  ========================================================
 */

#include <KernelKit/ProcessManager.hpp>
#include <KernelKit/ThreadLocalStorage.hxx>

using namespace hCore;

/**
 * Check for cookie inside TIB.
 * @param ptr
 * @return if the cookie is enabled.
 */
Boolean hcore_tls_check(VoidPtr ptr)
{
    if (!ptr)
        return false;

    const char* _ptr = (const char*)ptr;

    kcout << "TLS: Check for cookie...\n";

    return _ptr[0] == kCookieMag0 && _ptr[1] == kCookieMag1 && _ptr[2] == kCookieMag2;
}

/**
 * System call implementation in hCore
 * @param ptr
 * @return
 */
Void hcore_tls_check_syscall_impl(ThreadInformationBlock ptr) noexcept
{
    if (!hcore_tls_check(ptr.Cookie))
    {
        kcout << "TLS: Verification failure, crashing...\n";
        ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
    }
}