/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 *	File: PermissionSelector.cpp
 * 	Purpose: Permission primitives and types.
 *
 * 	========================================================
 */

#include <KernelKit/PermissionSelector.hxx>
#include <NewKit/Panic.hpp>

namespace hCore
{
    PermissionSelector::PermissionSelector(const Int32& sel)
        : fRing((RingKind)sel) 
    { MUST_PASS(sel > 0); }

    PermissionSelector::PermissionSelector(const RingKind& ringKind)
        : fRing(ringKind)
    {}

    PermissionSelector::~PermissionSelector() = default;

    bool PermissionSelector::operator==(const PermissionSelector& lhs) { return lhs.fRing == this->fRing; }
    
    bool PermissionSelector::operator!=(const PermissionSelector& lhs) { return lhs.fRing != this->fRing; }

    const RingKind& PermissionSelector::Ring() noexcept { return this->fRing; }
}