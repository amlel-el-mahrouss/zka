/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#ifndef __cplusplus
#error This API is meant to be used with C++
#endif

#ifdef CA_MUST_PASS
#undef CA_MUST_PASS
#endif

#define CA_UNREFERENCED_PARAMETER(e) ((void)e)

/// Assertion macros.

#ifdef _DEBUG
#define CA_MUST_PASS(e) __assert(e)
#else
#define CA_MUST_PASS(e) CA_UNREFERENCED_PARAMETER(e)
#endif

#ifdef __cplusplus

#define CA_EXTERN_C extern "C"

#else

#define CA_EXTERN_C extern

#endif

#define CA_STDCALL __attribute__((stdcall))
#define CA_CDECL __attribute__((cdecl))
#define CA_MSCALL __attribute__((ms_abi))

#define CA_PASCALL CA_STDCALL

typedef __UINT8_TYPE__ BYTE;
typedef __UINT16_TYPE__ WORD;
typedef __UINT32_TYPE__ DWORD;
typedef __UINT64_TYPE__ QWORD;

typedef void* PVOID;
typedef void VOID;

typedef __WCHAR_TYPE__ WCHAR;
typedef WCHAR* PWCHAR;

#ifdef __x86_64__
#   define _M_AMD64 2
#endif 

#ifdef __aarch64__
#   define _M_AARCH64 3
#endif 

#ifdef __powerpc64__
#   define _M_PPC64 4 
#endif 

#ifdef __64x0__
#   define _M_64000 5 
#endif 

#define CA_STATIC static
#define CA_INLINE inline

#ifdef __cplusplus
#define CA_CONSTEXPR constexpr
#else
#define CA_CONSTEXPR
#endif // __cplusplus