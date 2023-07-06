
#include "playlist.h"
#include <cassert>

namespace coffeepot
{
	void Playlist::addAction(const Action& action)
	{
		m_Actions.push_back(action);
	}

	void Playlist::removeAction(size_t actionIndex)
	{
		assert(actionIndex < m_Actions.size());
		m_Actions.erase(m_Actions.begin() + actionIndex);
	}

	void Playlist::removeAllActions()
	{
		m_Actions.clear();
		m_NextActionIndex = 0;
	}

	bool Playlist::startExecution()
	{
		m_NextActionIndex = 0;
		return hasNextActionForExecution();
	}

	void Playlist::stopExecution()
	{

	}

	size_t Playlist::getCurrentActionIndex() const
	{
		return m_NextActionIndex > 0 ? (m_NextActionIndex - 1) : 0;
	}

	bool Playlist::hasNextActionForExecution() const
	{
		return m_NextActionIndex < m_Actions.size();
	}

	const Action& Playlist::getNextActionForExecution()
	{
		assert(hasNextActionForExecution());
		return m_Actions[m_NextActionIndex++];
	}
}
