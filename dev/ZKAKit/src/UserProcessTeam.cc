/* -------------------------------------------

	Copyright EL Mahrouss Logic.

------------------------------------------- */

/***********************************************************************************/
/// @file UserProcessTeam.cc
/// @brief UserProcess teams implementation.
/***********************************************************************************/

#include <KernelKit/UserProcessScheduler.h>

namespace Kernel
{
	/***********************************************************************************/
	/// @brief UserProcess list array getter.
	/// @return The list of process to schedule.
	/***********************************************************************************/

	Array<UserProcess, kSchedProcessLimitPerTeam>& UserProcessTeam::AsArray()
	{
		return mProcessList;
	}

	/***********************************************************************************/
	/// @brief Get team ID.
	/// @return The team's ID.
	/***********************************************************************************/

	ProcessID& UserProcessTeam::Id() noexcept
	{
		return mTeamId;
	}

	/***********************************************************************************/
	/// @brief Get current process getter as Ref.
	/// @return The current process header.
	/***********************************************************************************/

	Ref<UserProcess>& UserProcessTeam::AsRef()
	{
		return mCurrentProcess;
	}
} // namespace Kernel

// last rev 05-03-24