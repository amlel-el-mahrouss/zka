/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <SeekerSrv/Seeker.hxx>

extern "C" Void DrawDesktopService(Void);
extern "C" Void DrawWindowService(Void);
extern "C" Void DrawMenuService(Void);
extern "C" Void DrawDialogService(Void);

extern "C" Void PumpMessageService(Void);

// @brief each app must call this for their region, otherwise it'll show a black rectangle.
extern "C" Void SeekerMessageLoop(Void)
{
    DrawDesktopService();
    DrawMenuService();
    DrawWindowService();
    DrawDialogService();
    PumpMessageService();
}