/* -------------------------------------------

 Copyright ZKA Technologies.

------------------------------------------- */

#ifndef _INC_USER_HXX_
#define _INC_USER_HXX_

#include <CompilerKit/CompilerKit.hxx>
#include <KernelKit/LPC.hxx>
#include <NewKit/String.hxx>
#include <NewKit/Defines.hxx>

// user mode users.
#define kSuperUser "ZKA USER\\SUPER"
#define kGuestUser "ZKA USER\\GUEST"

#define kUsersFile "\\Users\\$UsrRcrd"
#define kUsersDir  "\\Users\\"

#define kMaxUserNameLen	 (255)
#define kMaxUserTokenLen (255)

namespace Kernel
{
	class User;

	enum class RingKind
	{
		kRingStdUser   = 1,
		kRingSuperUser = 2,
		kRingGuestUser = 5,
		kRingCount	   = 3,
	};

	class User final
	{
	public:
		explicit User() = delete;

		User(const Int32& sel, const Char* userName);
		User(const RingKind& kind, const Char* userName);

		~User();

	public:
		ZKA_COPY_DEFAULT(User)

	public:
		bool operator==(const User& lhs);
		bool operator!=(const User& lhs);

	public:
		/// @brief Get software ring
		const RingKind& Ring() noexcept;

		/// @brief Get user name
		Char* Name() noexcept;

		/// @brief Is he a standard user?
		Bool IsStdUser() noexcept;

		/// @brief Is she a super user?
		Bool IsSuperUser() noexcept;

		Bool TrySave(const Char* password) noexcept;

	private:
		RingKind fRing{RingKind::kRingStdUser};
		Char	 fUserName[kMaxUserNameLen]	  = {0};
		Char	 fUserToken[kMaxUserTokenLen] = {0};
	};
} // namespace Kernel

#endif /* ifndef _INC_USER_HXX_ */