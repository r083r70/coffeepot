
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

	void ExecutionPlaylist::addAction(const Action& action)
	{
		m_Playlist.addAction(action);
		m_ActionStates.push_back(ActionState::Ready);
	}

	void ExecutionPlaylist::removeAction(int32_t actionIndex)
	{
		m_Playlist.removeAction(actionIndex);
		m_ActionStates.erase(m_ActionStates.begin() + actionIndex);
	}

	void ExecutionPlaylist::removeAllActions()
	{
		m_Playlist.removeAllActions();
		m_ActionStates.clear();
	}

	void ExecutionPlaylist::restart()
	{
		m_ActiveActionIndex = -1;

		m_ActionStates.clear();
		m_ActionStates.resize(m_Playlist.getActionsCount(), ActionState::Ready);
	}

	void ExecutionPlaylist::stop()
	{
		assert(isActive());
		m_ActiveActionIndex = m_Playlist.getActionsCount();

		for (size_t index = 0; index < m_Playlist.getActionsCount(); index++)
		{
			if (m_ActionStates[index] <= ActionState::Running)
				m_ActionStates[index] = ActionState::Aborted;
		}
	}

	void ExecutionPlaylist::advance()
	{
		m_ActiveActionIndex++;

		if (isActive())
		{
			assert(m_ActiveActionIndex < m_ActionStates.size());
			m_ActionStates[m_ActiveActionIndex] = ActionState::Running;
		}
	}

	void ExecutionPlaylist::completeAction(bool bSuccess)
	{
		assert(m_ActiveActionIndex < m_ActionStates.size());
		m_ActionStates[m_ActiveActionIndex] = bSuccess ? ActionState::Success : ActionState::Fail;
	}

	bool ExecutionPlaylist::isActive() const
	{
		// Can be Active with no ActiveAction (ie. m_ActiveActionIndex == -1)
		return m_ActiveActionIndex < m_Playlist.getActionsCount();
	}

	bool ExecutionPlaylist::hasActiveAction() const
	{
		return 0 <= m_ActiveActionIndex && m_ActiveActionIndex < m_Playlist.getActionsCount();
	}

	const coffeepot::Action& ExecutionPlaylist::getActiveAction() const
	{
		assert(hasActiveAction()); // Check for valid ActionIndex
		return m_Playlist.getActions()[m_ActiveActionIndex];
	}
}
