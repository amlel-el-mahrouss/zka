/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <KernelKit/Timer.hpp>

// bugs = 0

using namespace hCore;

HardwareTimer::HardwareTimer(Int64 seconds) : fWaitFor(seconds) {}
HardwareTimer::~HardwareTimer() { fWaitFor = 0; }

Int32 HardwareTimer::Wait()
{
    if (fWaitFor < 1)
        return -1;

    while (*fDigitalTimer < (*fDigitalTimer + fWaitFor))
        ;

    return 0;
}