/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#ifndef __MOD_ACPI_HXX__
#define __MOD_ACPI_HXX__

#include <KernelKit/DebugOutput.hxx>
#include <modules/ACPI/ACPI.hxx>
#include <NewKit/ErrorOr.hxx>
#include <NewKit/Defines.hxx>
#include <NewKit/Ref.hxx>

namespace Kernel
{
	class ACPIFactoryInterface;

	typedef ACPIFactoryInterface PowerFactoryInterface;

	class ACPIFactoryInterface final
	{
	public:
		explicit ACPIFactoryInterface(voidPtr rsp_ptr);
		~ACPIFactoryInterface() = default;

		ACPIFactoryInterface& operator=(const ACPIFactoryInterface&) = default;
		ACPIFactoryInterface(const ACPIFactoryInterface&)			 = default;

	public:
		Void Shutdown(); // shutdown
		Void Reboot();	 // soft-reboot

	public:
		/// @brief Descriptor find factory.
		/// @param signature The signature of the descriptor table (MADT, ACPI...)
		/// @return the blob inside an ErrorOr object.
		ErrorOr<voidPtr> Find(const Char* signature);

		/// @brief Checksum factory.
		/// @param checksum the data to checksum
		/// @param len it's size
		/// @return if it succeed
		bool Checksum(const Char* checksum, SSizeT len); // watch for collides!

	public:
		ErrorOr<voidPtr> operator[](const Char* signature)
		{
			return this->Find(signature);
		}

	private:
		VoidPtr fRsdp;	  // pointer to root descriptor.
		SSizeT	fEntries; // number of entries, -1 tells that no invalid entries were
						  // found.
	};
} // namespace Kernel

#endif // !__MOD_ACPI_HXX__