
#include "actionmanager.h"

#include "core/log.h"
#include "core/serializer.h"

#include <assert.h>
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
        if (!m_Executor) // no running action
            return;
            
        char* output = m_OutputBuffer.data();
        if (m_Executor->update(output)) // still executing
            return;
        
        if (m_CurrentPlaylist.hasNextAction())
            startNextAction();
        else
            m_Executor.reset();
    }

    bool ActionsManager::executeAction(const Action& action)
    {
        if (!m_Executor)
            m_CurrentPlaylist.removeAllAction(); // reset

        m_CurrentPlaylist.addAction(action);
        return m_Executor || startNextAction();
    }

    const char* ActionsManager::readOutput()
    {
        if (!m_Executor)
            return nullptr;

        return m_OutputBuffer.data();
    }

    bool ActionsManager::startNextAction()
    {
        const Action& nextAction = m_CurrentPlaylist.getNextAction();
        m_Executor = std::make_unique<ActionExecutor>(nextAction);
        
        if (m_Executor->start())
            return true;
        
        CP_ERROR("Couldnt start action {}", nextAction.m_Name);
        m_Executor.reset();
        return false;
    }
}