/* -------------------------------------------

 Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <Modules/CoreCG/FbRenderer.hxx>
#include <Modules/CoreCG/WindowRenderer.hxx>

namespace System
{
	class UIRenderEffect;
	class UIRenderCanvas;
	class UIRenderView; // phone desktop, computer desktop, remote desktop.

	enum
	{
		eDesktopInvalid,
		eDesktopPC,
		eDesktopMobile,
		eDestkopRemote,
		eDesktopCount,
	};
} // namespace System