
/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>

namespace Kernel
{
	void ke_runtime_check(bool bExpression, const char* file, const char* line);
}

#define MUST_PASS_COMPILER(EXPR, MSG) static_assert(EXPR, MSG)
#define __MUST_PASS(EXPR, FILE, LINE) \
	Kernel::ke_runtime_check(EXPR, FILE, STRINGIFY(LINE))
#define MUST_PASS(EXPR) __MUST_PASS(EXPR, __FILE__, __LINE__)
#define assert(EXPR)	MUST_PASS(EXPR, RUNTIME_CHECK_EXPRESSION)

enum RUNTIME_CHECK
{
	RUNTIME_CHECK_FAILED  = -1,
	RUNTIME_CHECK_POINTER = 0,
	RUNTIME_CHECK_EXPRESSION,
	RUNTIME_CHECK_FILE,
	RUNTIME_CHECK_IPC,
	RUNTIME_CHECK_TLS,
	RUNTIME_CHECK_HANDSHAKE,
	RUNTIME_CHECK_ACPI,
	RUNTIME_CHECK_INVALID_PRIVILEGE,
	RUNTIME_CHECK_PROCESS,
	RUNTIME_CHECK_BAD_BEHAVIOR,
	RUNTIME_CHECK_BOOTSTRAP,
	RUNTIME_CHECK_UNEXCPECTED,
	RUNTIME_CHECK_COUNT,
};

namespace Kernel
{
	/// @brief Dumping factory class.
	class RecoveryFactory final
	{
	public:
		STATIC Void Recover() noexcept;
	};

	void ke_stop(const Int& id);
} // namespace Kernel

#ifdef TRY
#undef TRY
#endif

#define TRY(FN)           \
	if (!FN())            \
	{                     \
		MUST_PASS(false); \
	}