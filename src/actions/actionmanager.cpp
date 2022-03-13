
#include "actionmanager.h"

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
        // #todo Load from file (YAML)
        m_Actions.push_back(Action::Create("git status"));
        m_Actions.push_back(Action::Create("git --version"));
        m_Actions.push_back(Action::Create("ls -lX"));
        m_Actions.push_back(Action::CreateDemo());
        return true;
    }

    void ActionsManager::deinit()
    {
        for (auto it = m_Actions.begin(); it != m_Actions.end(); it++)
        {
            Action* const action = *it;
            if (action == nullptr)
                continue;
                
            if (action->isRunning())
                action->abort();

            delete action;
        }

        m_Actions.clear();
    }

    void ActionsManager::tick()
    {
        for (auto it = m_Actions.begin(); it != m_Actions.end(); it++)
        {
            Action* const action = *it;
            if (action != nullptr && action->isRunning())
                action->update();
        }
    }
}