/* -------------------------------------------

	Copyright ZKA Technologies

	File: Main.cxx
	Purpose: Main entrypoint of kernel.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <CompilerKit/Detail.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileManager.hxx>
#include <KernelKit/Framebuffer.hxx>
#include <KernelKit/Heap.hxx>
#include <KernelKit/PEF.hxx>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/ProcessHeap.hxx>
#include <NewKit/Json.hxx>
#include <NewKit/KernelCheck.hxx>
#include <NewKit/String.hxx>
#include <NewKit/Utils.hxx>
#include <KernelKit/CodeManager.hxx>
#include <CFKit/Property.hxx>
#include <Modules/CoreCG/WindowRenderer.hxx>
#include <KernelKit/Timer.hxx>

EXTERN Kernel::Property cKernelVersion;

namespace Kernel::Detail
{
	/// @brief Filesystem auto formatter, additional checks are also done by the class.
	class FilesystemInstaller final
	{
		Kernel::NewFilesystemManager* fNewFS{nullptr};

	public:
		/// @brief wizard constructor.
		explicit FilesystemInstaller()
		{
			if (Kernel::FilesystemManagerInterface::GetMounted())
			{
				// Partition is mounted, cool!
				Kernel::kcout
					<< "newoskrnl: No need to create for a new NewFS (EPM) partition here...\r";

				fNewFS = reinterpret_cast<Kernel::NewFilesystemManager*>(Kernel::FilesystemManagerInterface::GetMounted());
			}
			else
			{
				// Mounts a NewFS from main drive.
				fNewFS = new Kernel::NewFilesystemManager();

				Kernel::FilesystemManagerInterface::Mount(fNewFS);
			}

			if (fNewFS->GetParser())
			{
				constexpr auto cFolderInfo		  = "META-INF";
				const auto	   cDirCount		  = 7;
				const char*	   cDirStr[cDirCount] = {
					   "\\Boot\\", "\\System\\", "\\Support\\", "\\Applications\\",
					   "\\Users\\", "\\Library\\", "\\Mount\\"};

				for (Kernel::SizeT dirIndx = 0UL; dirIndx < cDirCount; ++dirIndx)
				{
					auto catalogDir = fNewFS->GetParser()->GetCatalog(cDirStr[dirIndx]);

					if (catalogDir)
					{
						Kernel::kcout << "newoskrnl: already exists.\r";

						delete catalogDir;
						continue;
					}

					catalogDir = fNewFS->GetParser()->CreateCatalog(cDirStr[dirIndx], 0,
																	kNewFSCatalogKindDir);

					NFS_FORK_STRUCT theFork{0};

					const Kernel::Char* cSrcName = cFolderInfo;

					Kernel::rt_copy_memory((Kernel::VoidPtr)(cSrcName), theFork.ForkName,
										   Kernel::rt_string_len(cSrcName));

					Kernel::rt_copy_memory((Kernel::VoidPtr)(catalogDir->Name),
										   theFork.CatalogName,
										   Kernel::rt_string_len(catalogDir->Name));

					delete catalogDir;

					theFork.DataSize	 = kNewFSForkSize;
					theFork.ResourceId	 = 0;
					theFork.ResourceKind = Kernel::kNewFSRsrcForkKind;
					theFork.Kind		 = Kernel::kNewFSDataForkKind;

					Kernel::StringView metadataFolder(kNewFSSectorSz);

					metadataFolder +=
						"<!properties/>\r<p>Kind: folder</p>\r<p>Created by: system</p>\r<p>Edited by: "
						"system</p>\r<p>Volume Type: Zeta</p>\r";

					metadataFolder += "<p>Path: ";
					metadataFolder += cDirStr[dirIndx];
					metadataFolder += "</p>\r";

					const Kernel::SizeT metadataSz = kNewFSSectorSz;

					auto catalogSystem = fNewFS->GetParser()->GetCatalog(cDirStr[dirIndx]);

					fNewFS->GetParser()->CreateFork(catalogSystem, theFork);

					fNewFS->GetParser()->WriteCatalog(
						catalogSystem, true, (Kernel::VoidPtr)(metadataFolder.CData()),
						metadataSz, cFolderInfo);

					delete catalogSystem;
				}
			}

			NFS_CATALOG_STRUCT* catalogDisk =
				this->fNewFS->GetParser()->GetCatalog("\\Mount\\NUL:");

			const Kernel::Char* cSrcName = "DISK-INF";

			if (catalogDisk)
			{
				delete catalogDisk;
			}
			else
			{
				catalogDisk =
					(NFS_CATALOG_STRUCT*)this->Leak()->CreateAlias("\\Mount\\NUL:");

				Kernel::StringView diskFolder(kNewFSSectorSz);

				diskFolder +=
					"<!properties/><p>Kind: alias to NULL.</p>\r<p>Created by: system</p>\r<p>Edited "
					"by: "
					"system</p>\r<p>Volume Type: NULL.</p>\r";

				diskFolder += "<p>Root: NUL";
				diskFolder += "</p>\r";

				NFS_FORK_STRUCT theDiskFork{0};

				Kernel::rt_copy_memory((Kernel::VoidPtr)(cSrcName), theDiskFork.ForkName,
									   Kernel::rt_string_len(cSrcName));

				Kernel::rt_copy_memory((Kernel::VoidPtr)(catalogDisk->Name),
									   theDiskFork.CatalogName,
									   Kernel::rt_string_len(catalogDisk->Name));

				theDiskFork.DataSize	 = kNewFSForkSize;
				theDiskFork.ResourceId	 = 0;
				theDiskFork.ResourceKind = Kernel::kNewFSRsrcForkKind;
				theDiskFork.Kind		 = Kernel::kNewFSDataForkKind;

				fNewFS->GetParser()->CreateFork(catalogDisk, theDiskFork);
				fNewFS->GetParser()->WriteCatalog(catalogDisk,
												  true,
												  (Kernel::VoidPtr)diskFolder.CData(),
												  kNewFSSectorSz, cSrcName);

				delete catalogDisk;
			}
		}

		~FilesystemInstaller() = default;

		NEWOS_COPY_DEFAULT(FilesystemInstaller);

		/// @brief Grab the disk's NewFS reference.
		/// @return NewFilesystemManager the filesystem interface
		Kernel::NewFilesystemManager* Leak()
		{
			return fNewFS;
		}
	};
} // namespace Kernel::Detail

namespace Kernel
{
	EXTERN ProcessScheduler* cProcessScheduler;
} // namespace Kernel

/// @brief Application entrypoint.
/// @param Void
/// @return Void
EXTERN_C Kernel::Void ke_dll_entrypoint(Kernel::Void)
{
	CGInit();

	CGDrawInRegion(CGColor(0x45, 0x00, 0x06), CG::UIAccessibilty::The().Height(), CG::UIAccessibilty::The().Width(),
				   0, 0);

	CGFini();

	auto root_zka_wnd = CG::CGCreateWindow(CG::cWndFlagWindow, "ZKA Setup", "Window", 0, 0, CG::UIAccessibilty::The().Height() - 20, CG::UIAccessibilty::The().Width() - 20);

	root_zka_wnd->w_x = 10;
	root_zka_wnd->w_y = 10;

	root_zka_wnd->w_needs_repaint = Yes;

	CG::CGDrawWindowList(&root_zka_wnd, 1);

	/// Now run kernel loop, until no process are running.
	Kernel::Detail::FilesystemInstaller(); // automatic filesystem creation.

	/// @note BThread doesn't parse the symbols so doesn't nullify them, .bss is though.
	Kernel::cProcessScheduler = nullptr;

	while (Yes)
	{
		Kernel::ProcessHelper::StartScheduling();
	}
}