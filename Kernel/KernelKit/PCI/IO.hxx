/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

#include <ArchKit/ArchKit.hxx>
#include <NewKit/Array.hxx>
#include <NewKit/Defines.hxx>
#include <NewKit/Ref.hxx>

namespace Kernel
{
	template <SizeT Sz>
	class IOArray final
	{
	public:
		IOArray() = delete;

		IOArray(nullPtr) = delete;

		explicit IOArray(Array<UShort, Sz>& ports)
			: fPorts(ports)
		{
		}
		~IOArray()
		{
		}

		IOArray& operator=(const IOArray&) = default;

		IOArray(const IOArray&) = default;

		operator bool()
		{
			return !fPorts.Empty();
		}

	public:
		template <typename T>
		T In(SizeT index);

		template <typename T>
		void Out(SizeT index, T value);

	private:
		Array<UShort, Sz> fPorts;
	};

	using IOArray16 = IOArray<16>;
} // namespace Kernel

#ifdef __x86_64__
#include <KernelKit/PCI/IO-Impl-AMD64.inl>
#else
#error Please provide platform specific code for the I/O
#endif // ifdef __x86_64__