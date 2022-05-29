
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

        // Test
        Action action;
        action.m_ID = m_Actions.size();
        action.m_Name = "AcTest";
        action.m_Command = "";
        
        Option option;
        option.m_ID = 0;
        option.m_Name = "OpTest";
        option.m_Value = "";
        option.m_DefaultValue = "";
        option.m_OptionType = OptionType::Selection;
        option.m_SelectableValues.push_back("One");
        option.m_SelectableValues.push_back("Two");
        option.m_SelectableValues.push_back("Three");
        option.m_SelectableValues.push_back("Four");

        action.m_Options.push_back(option);
        m_Actions.push_back(action);
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