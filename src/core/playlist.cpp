
#include "playlist.h"
#include <cassert>

namespace coffeepot
{
	//--------------------------------------------------//
	//-------------------- Playlist --------------------//
	//--------------------------------------------------//

	void Playlist::addAction(const Action& action)
	{
		m_Actions.push_back(action);
	}

	void Playlist::removeAction(int32_t actionIndex)
	{
		assert(actionIndex >= 0);
		assert(actionIndex < m_Actions.size());

		m_Actions.erase(m_Actions.begin() + actionIndex);
	}

	void Playlist::removeAllActions()
	{
		m_Actions.clear();
	}

	//--------------------------------------------------//
	//--------------- ExecutionPlaylist ----------------//
	//--------------------------------------------------//

	void ExecutionPlaylist::removeAction(int32_t actionIndex)
	{
		m_Playlist.removeAction(actionIndex);
		if (actionIndex <= m_LastCompletedActionIndex)
			m_LastCompletedActionIndex--;
	}

	void ExecutionPlaylist::removeAllActions()
	{
		m_Playlist.removeAllActions();
		m_LastCompletedActionIndex = -1;
	}

	void ExecutionPlaylist::restart()
	{
		m_ActiveActionIndex = 0;
		m_LastCompletedActionIndex = -1;
	}

	void ExecutionPlaylist::stop()
	{
		m_ActiveActionIndex = m_Playlist.getActionsCount();
	}

	void ExecutionPlaylist::advance()
	{
		m_LastCompletedActionIndex = m_ActiveActionIndex;
		m_ActiveActionIndex++;
	}

	bool ExecutionPlaylist::isActive() const
	{
		return 0 <= m_ActiveActionIndex && m_ActiveActionIndex < m_Playlist.getActionsCount();
	}

	const coffeepot::Action& ExecutionPlaylist::getActiveAction() const
	{
		assert(isActive()); // Check for valid ActionIndex
		return m_Playlist.getActions()[m_ActiveActionIndex];
	}
}
