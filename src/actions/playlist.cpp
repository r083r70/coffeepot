
#include "playlist.h"
#include <assert.h>

namespace coffeepot
{
    void Playlist::addAction(const Action& action)
    {
        m_Actions.push_back(action);
    }

    void Playlist::removeAllAction()
    {
        m_Actions.clear();
        m_NextActionIndex = 0;
    }

    bool Playlist::startExecution()
    {
        m_NextActionIndex = 0;
        return hasNextAction();
    }

    bool Playlist::hasNextAction() const
    {
        return m_NextActionIndex < m_Actions.size();
    }

    const Action& Playlist::getNextAction()
    {
        assert(hasNextAction());
        return m_Actions[m_NextActionIndex++];
    }
}
