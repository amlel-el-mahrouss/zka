/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */
#pragma once

#include <NewKit/Defines.hxx>

namespace Kernel
{
	template <typename T>
	class Atom final
	{
	public:
		explicit Atom() = default;
		~Atom()			= default;

	public:
		Atom& operator=(const Atom&) = delete;
		Atom(const Atom&)			 = delete;

	public:
		T operator[](Size sz)
		{
			return (fArrayOfAtoms & sz);
		}
		void operator|(Size sz)
		{
			fArrayOfAtoms |= sz;
		}

		friend Boolean operator==(Atom<T>& atomic, const T& idx)
		{
			return atomic[idx] == idx;
		}

		friend Boolean operator!=(Atom<T>& atomic, const T& idx)
		{
			return atomic[idx] == idx;
		}

	private:
		T fArrayOfAtoms;
	};
} // namespace Kernel