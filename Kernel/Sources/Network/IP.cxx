/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <NetworkKit/IP.hpp>
#include <NewKit/Utils.hpp>

namespace NewOS
{
	char* RawIPAddress::Address()
	{
		return fAddr;
	}

	RawIPAddress::RawIPAddress(char bytes[4])
	{
		rt_copy_memory(bytes, fAddr, 4);
	}

	bool RawIPAddress::operator==(const RawIPAddress& ipv4)
	{
		for (Size index = 0; index < 4; ++index)
		{
			if (ipv4.fAddr[index] != fAddr[index])
				return false;
		}

		return true;
	}

	bool RawIPAddress::operator!=(const RawIPAddress& ipv4)
	{
		for (Size index = 0; index < 4; ++index)
		{
			if (ipv4.fAddr[index] == fAddr[index])
				return false;
		}

		return true;
	}

	char& RawIPAddress::operator[](const Size& index)
	{
		kcout << "[RawIPAddress::operator[]] Fetching Index...\r";

		static char IP_PLACEHOLDER = '0';
		if (index > 4)
			return IP_PLACEHOLDER;

		return fAddr[index];
	}

	RawIPAddress6::RawIPAddress6(char bytes[8])
	{
		rt_copy_memory(bytes, fAddr, 8);
	}

	char& RawIPAddress6::operator[](const Size& index)
	{
		kcout << "[RawIPAddress6::operator[]] Fetching Index...\r";

		static char IP_PLACEHOLDER = '0';
		if (index > 8)
			return IP_PLACEHOLDER;

		return fAddr[index];
	}

	bool RawIPAddress6::operator==(const RawIPAddress6& ipv6)
	{
		for (SizeT index = 0; index < 8; ++index)
		{
			if (ipv6.fAddr[index] != fAddr[index])
				return false;
		}

		return true;
	}

	bool RawIPAddress6::operator!=(const RawIPAddress6& ipv6)
	{
		for (SizeT index = 0; index < 8; ++index)
		{
			if (ipv6.fAddr[index] == fAddr[index])
				return false;
		}

		return true;
	}

	ErrorOr<StringView> IPFactory::ToStringView(Ref<RawIPAddress6> ipv6)
	{
		auto str = StringBuilder::Construct(ipv6.Leak().Address());
		return str;
	}

	ErrorOr<StringView> IPFactory::ToStringView(Ref<RawIPAddress> ipv4)
	{
		auto str = StringBuilder::Construct(ipv4.Leak().Address());
		return str;
	}

	bool IPFactory::IpCheckVersion4(const char* ip)
	{
		int cnter = 0;

		for (Size base = 0; base < rt_string_len(ip); ++base)
		{
			if (ip[base] == '.')
			{
				cnter = 0;
			}
			else
			{
				if (cnter == 3)
					return false;

				++cnter;
			}
		}

		return true;
	}
} // namespace NewOS