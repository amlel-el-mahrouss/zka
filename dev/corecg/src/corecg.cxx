/* -------------------------------------------

	Copyright ZKA Technologies.

	FILE: corecg.cxx
	PURPOSE: CoreCG Window Manager entrypoint.

------------------------------------------- */

#include <corecg/corecg.hxx>

/** @brief CoreCG main, ZWM acts a proxy to the Installed GPU Driver. */
CG::Int32 ModuleMain(CG::Void)
{
    CG::CGDrawDesktopBackground();

	return 0;
}