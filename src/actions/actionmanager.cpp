
#include "actionmanager.h"

#include "core/serializer.h"

namespace coffeepot
{
    ActionsManager* ActionsManager::m_Instance = nullptr;

    ActionsManager* ActionsManager::get()
    {
        if (!m_Instance)
            m_Instance = new ActionsManager();
        
        return m_Instance;
    }

    bool ActionsManager::init()
    {
        Serializer::loadActions(m_Actions);
        return true;
    }

    void ActionsManager::deinit()
    {
        m_Actions.clear();
    }

    void ActionsManager::tick()
    {
        if (m_Executor && !m_Executor->update())
            m_Executor.reset();
    }

    bool ActionsManager::startAction(Action& action)
    {
        if (m_Executor)
            return false;

        m_Executor = std::make_unique<ActionExecutor>(action);
        return m_Executor->start();
    }
}