/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <modules/FB/FB.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/FileMgr.h>
#include <KernelKit/Heap.h>
#include <KernelKit/PEFCodeMgr.h>
#include <KernelKit/UserProcessScheduler.h>
#include <NewKit/Json.h>
#include <KernelKit/CodeMgr.h>
#include <modules/ACPI/ACPIFactoryInterface.h>
#include <NetworkKit/IPC.h>
#include <CFKit/Property.h>

Kernel::Void hal_real_init(Kernel::Void) noexcept;
EXTERN_C Kernel::Void ke_dll_entrypoint(Kernel::Void);

EXTERN_C void hal_init_platform(
	Kernel::HEL::HANDOVER_INFO_HEADER* HandoverHeader)
{

	/************************************************** */
	/*     INITIALIZE AND VALIDATE HEADER.              */
	/************************************************** */

	kHandoverHeader = HandoverHeader;

	if (kHandoverHeader->f_Magic != kHandoverMagic &&
		kHandoverHeader->f_Version != kHandoverVersion)
	{
		return;
	}

	/************************************** */
	/*     INITIALIZE BIT MAP.              */
	/************************************** */

	while (Yes)
	{
	}
}