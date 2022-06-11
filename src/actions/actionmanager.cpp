
#include "actionmanager.h"

#include "core/serializer.h"

#include <cstring>

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
        return Serializer::loadActions(m_Actions);
    }

    void ActionsManager::deinit()
    {
        m_Actions.clear();
    }

    void ActionsManager::tick()
    {
        if (m_Executor && !m_Executor->update(m_OutputBuffer.data()))
            m_Executor.reset();
    }

    bool ActionsManager::startAction(Action& action)
    {
        if (m_Executor)
            return false;

        m_Executor = std::make_unique<ActionExecutor>(action);
        return m_Executor->start();
    }

    const char* ActionsManager::readOutput()
    {
        if (!m_Executor)
            return nullptr;

        return m_OutputBuffer.data();
    }
}