/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <NewKit/String.hxx>
#include <NewKit/Utils.hxx>
#include <KernelKit/DebugOutput.hxx>

namespace Kernel
{
	Char* StringView::Data()
	{
		return fData;
	}

	const Char* StringView::CData() const
	{
		return fData;
	}

	Size StringView::Length() const
	{
		return fSz;
	}

	bool StringView::operator==(const StringView& rhs) const
	{
		if (rhs.Length() != this->Length())
			return false;

		for (Size index = 0; index < this->Length(); ++index)
		{
			if (rhs.fData[index] != fData[index])
				return false;
		}

		return true;
	}

	bool StringView::operator==(const Char* rhs) const
	{
		if (rt_string_len(rhs) != this->Length())
			return false;

		for (Size index = 0; index < rt_string_len(rhs); ++index)
		{
			if (rhs[index] != fData[index])
				return false;
		}

		return true;
	}

	bool StringView::operator!=(const StringView& rhs) const
	{
		if (rhs.Length() != this->Length())
			return false;

		for (Size index = 0; index < rhs.Length(); ++index)
		{
			if (rhs.fData[index] == fData[index])
				return false;
		}

		return true;
	}

	bool StringView::operator!=(const Char* rhs) const
	{
		if (rt_string_len(rhs) != this->Length())
			return false;

		for (Size index = 0; index < rt_string_len(rhs); ++index)
		{
			if (rhs[index] == fData[index])
				return false;
		}

		return true;
	}

	ErrorOr<StringView> StringBuilder::Construct(const Char* data)
	{
		if (!data || *data == 0)
			return {};

		StringView view(rt_string_len(data));

		view += data;

		return ErrorOr<StringView>(view);
	}

	const char* StringBuilder::FromInt(const char* fmt, int i)
	{
		if (!fmt)
			return ("-1");

		char* ret = (char*)ALLOCA(sizeof(char) * 8 + rt_string_len(fmt));

		if (!ret)
			return ("-1");

		Char result[8];

		if (!rt_to_string(result, sizeof(int), i))
		{
			return ("-1");
		}

		const auto fmt_len = rt_string_len(fmt);
		const auto res_len = rt_string_len(result);

		for (Size idx = 0; idx < fmt_len; ++idx)
		{
			if (fmt[idx] == '%')
			{
				SizeT result_cnt = idx;

				for (auto y_idx = idx; y_idx < res_len; ++y_idx)
				{
					ret[result_cnt] = result[y_idx];
					++result_cnt;
				}

				break;
			}

			ret[idx] = fmt[idx];
		}

		return ret; /* Copy that ret into a buffer, 'ALLOCA' allocates to the stack */
	}

	const char* StringBuilder::FromBool(const char* fmt, bool i)
	{
		if (!fmt)
			return ("?");

		const char* boolean_expr = i ? "true" : "false";
		char*		ret			 = (char*)ALLOCA((sizeof(char) * i) ? 4 : 5 + rt_string_len(fmt));

		if (!ret)
			return ("?");

		const auto fmt_len = rt_string_len(fmt);
		const auto res_len = rt_string_len(boolean_expr);

		for (Size idx = 0; idx < fmt_len; ++idx)
		{
			if (fmt[idx] == '%')
			{
				SizeT result_cnt = idx;

				for (auto y_idx = idx; y_idx < res_len; ++y_idx)
				{
					ret[result_cnt] = boolean_expr[y_idx];
					++result_cnt;
				}

				break;
			}

			ret[idx] = fmt[idx];
		}

		return ret;
	}

	bool StringBuilder::Equals(const char* lhs, const char* rhs)
	{
		if (rt_string_len(rhs) != rt_string_len(lhs))
			return false;

		for (Size index = 0; index < rt_string_len(rhs); ++index)
		{
			if (rhs[index] != lhs[index])
				return false;
		}

		return true;
	}

	const char* StringBuilder::Format(const char* fmt, const char* fmt2)
	{
		if (!fmt || !fmt2)
			return ("?");

		char* ret =
			(char*)ALLOCA(sizeof(char) * rt_string_len(fmt2) + rt_string_len(fmt2));

		if (!ret)
			return ("?");

		for (Size idx = 0; idx < rt_string_len(fmt); ++idx)
		{
			if (fmt[idx] == '%')
			{
				Size result_cnt = idx;
				for (Size y_idx = 0; y_idx < rt_string_len(fmt2); ++y_idx)
				{
					ret[result_cnt] = fmt2[y_idx];
					++result_cnt;
				}

				break;
			}

			ret[idx] = fmt[idx];
		}

		return ret;
	}

	STATIC void rt_string_append(Char* lhs, const Char* rhs, Int32 cur)
	{
		SizeT sz_rhs = rt_string_len(rhs);
		SizeT rhs_i = 0;

		for (; rhs_i < sz_rhs; ++rhs_i)
		{
			lhs[rhs_i + cur] = rhs[rhs_i];
		}
	}

	StringView& StringView::operator+=(const Char* rhs)
	{
		rt_string_append(this->fData, rhs, this->fCur);
		this->fCur += rt_string_len(rhs);

		return *this;
	}

	StringView& StringView::operator+=(const StringView& rhs)
	{
		if (rt_string_len(rhs.fData) > this->Length())
			return *this;

		rt_string_append(this->fData, const_cast<Char*>(rhs.fData), this->fCur);
		this->fCur += rt_string_len(const_cast<Char*>(rhs.fData));

		return *this;
	}
} // namespace Kernel