/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

#ifndef KIB
#define KIB(X) ((X) / 1024)
#endif

#ifndef kib_cast
#define kib_cast(X) ((X) * 1024)
#endif

#ifndef MIB
#define MIB(X) ((UInt64)KIB(X) / 1024)
#endif

#ifndef mib_cast
#define mib_cast(X) ((UInt64)kib_cast(X) * 1024)
#endif

#ifndef GIB
#define GIB(X) ((UInt64)MIB(X) / 1024)
#endif

#ifndef gib_cast
#define gib_cast(X) ((UInt64)mib_cast(X) * 1024)
#endif

#ifndef TIB
#define TIB(X) ((UInt64)GIB(X) / 1024)
#endif

#ifndef tib_cast
#define tib_cast(X) ((UInt64)gib_cast(X) * 1024)
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)              \
	(((sizeof(a) / sizeof(*(a))) / \
	  (static_cast<Kernel::Size>(!(sizeof(a) % sizeof(*(a)))))))
#endif

#ifndef ALIGN
#define ALIGN(X) __attribute__((aligned(X)))
#endif // #ifndef ALIGN

#ifndef ATTRIBUTE
#define ATTRIBUTE(X) __attribute__((X))
#endif // #ifndef ATTRIBUTE

#ifndef __MAHROUSS_VER__
#define __MAHROUSS_VER__ (2024)
#endif // !__MAHROUSS_VER__

#ifndef EXTERN_C
#define EXTERN_C extern "C"
#endif

#ifndef MAKE_ENUM
#define MAKE_ENUM(NAME) \
	enum NAME           \
	{
#endif

#ifndef END_ENUM
#define END_ENUM() \
	}              \
	;
#endif

#ifndef MAKE_STRING_ENUM
#define MAKE_STRING_ENUM(NAME) \
	namespace NAME             \
	{
#endif

#ifndef ENUM_STRING
#define ENUM_STRING(NAME, VAL) inline constexpr const char* e##NAME = VAL
#endif

#ifndef END_STRING_ENUM
#define END_STRING_ENUM() }
#endif

#ifndef ALLOCA
#define ALLOCA(Sz) __builtin_alloca(Sz)
#endif // #ifndef ALLOCA

#ifndef CANT_REACH
#define CANT_REACH() __builtin_unreachable()
#endif

#define kBadPtr	 0xFBFBFBFBFBFBFBFB
#define kMaxAddr 0xFFFFFFFFFFFFFFFF
#define kPathLen 255

#define PACKED	ATTRIBUTE(packed)
#define NO_EXEC ATTRIBUTE(noexec)

#define EXTERN extern
#define STATIC static

#define CONST const

#define STRINGIFY(X)	#X
#define NEWOS_UNUSED(X) ((Kernel::Void)X)

#ifndef RGB
#define RGB(R, G, B) (Kernel::UInt32)(R | G << 0x8 | B << 0x10)
#endif // !RGB