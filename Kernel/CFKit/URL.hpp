/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#ifndef _INC_URL_HPP_
#define _INC_URL_HPP_

#include <NewKit/Defines.hpp>
#include <NewKit/String.hpp>

namespace NewOS
{
	class URL final
	{
	public:
		explicit URL(StringView& strUrl);
		~URL();

	public:
		Ref<ErrorOr<StringView>> Location() noexcept;
		Ref<ErrorOr<StringView>> Protocol() noexcept;

	private:
		Ref<StringView> fUrlView;
	};

	ErrorOr<StringView> url_extract_location(const char* url);
	ErrorOr<StringView> url_extract_protocol(const char* url);
} // namespace NewOS

#endif /* ifndef _INC_URL_HPP_ */