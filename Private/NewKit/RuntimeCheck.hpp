
/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>

namespace HCore
{
void runtime_check(bool bExpression, const char *file, const char *line);
}

#define MUST_PASS_COMPILER(EXPR, MSG) static_assert(EXPR, MSG)
#define MUST_PASS(EXPR) HCore::runtime_check(EXPR, __FILE__, " %d -> ")
#define assert(EXPR) MUST_PASS(EXPR, RUNTIME_CHECK_EXPRESSION)

enum RUNTIME_CHECK
{
    RUNTIME_CHECK_FAILED = -1,
    RUNTIME_CHECK_POINTER = 0,
    RUNTIME_CHECK_EXPRESSION,
    RUNTIME_CHECK_FILE,
    RUNTIME_CHECK_IPC,
    RUNTIME_CHECK_TLS,
    RUNTIME_CHECK_LD,
    RUNTIME_CHECK_HANDSHAKE,
    RUNTIME_CHECK_ACPI,
    RUNTIME_CHECK_INVALID_PRIVILEGE,
    RUNTIME_CHECK_PROCESS,
    RUNTIME_CHECK_BAD_BEHAVIOR,
    RUNTIME_CHECK_COUNT,
    RUNTIME_CHECK_BOOTSTRAP,
};

namespace HCore
{
class DumpManager final
{
  public:
    static void Dump(void)
    {
        // TODO:
    }
};

void ke_stop(const Int &id);
} // namespace HCore

#ifdef TRY
#undef TRY
#endif

#define INIT(OBJ, TYPE, ...) TYPE OBJ = TYPE(__VA_ARGS__)

#define TRY(FN)                                                                                                        \
    if (!FN())                                                                                                         \
    {                                                                                                                  \
        MUST_PASS(false);                                                                                              \
    }