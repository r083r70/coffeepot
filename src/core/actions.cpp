
#include "actions.h"
#include "log.h"

#include <cstdio>

namespace coffeepot
{
    FILE* pOpen(const char* command)
	{
#if CP_WIN
		return _popen(command, "r");
#elif CP_LINUX
		return popen(command, "r");
#else
        return nullptr;
#endif
    }

	void pClose(FILE* pipe)
	{
#if CP_WIN
		_pclose(pipe);
#elif CP_LINUX
		pclose(pipe);
#endif
    }

    CmdAction::CmdAction(const std::string& command)
        : Action()
        , m_Command(command)
        , m_Buffer()
        , m_Pipe(nullptr)
    {}

    bool CmdAction::isRunning() const
    {
        return m_Pipe != nullptr;
    }

    std::string CmdAction::getName() const
    {
        static std::string prefix = "cmd: ";
        return prefix + m_Command;
    }

    bool CmdAction::start()
    {
        m_Pipe = pOpen(m_Command.c_str());
        return m_Pipe != nullptr;
    }

    bool CmdAction::update()
    {
        if (fgets(m_Buffer.data(), 128, m_Pipe) != nullptr)
        {
            const size_t eolIndex = std::strcspn(m_Buffer.data(), "\r\n\0");
            m_Buffer[eolIndex] = '\0';
            
            CP_INFO(m_Buffer.data());
            
            return true;
        }

        stop();
        return false;
    }

    void CmdAction::abort()
    {
        stop();
    }

    void CmdAction::stop()
    {
        if (m_Pipe != nullptr)
            pClose(m_Pipe);
        m_Pipe = nullptr;
    }

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
        m_Actions.push_back(new CmdAction{"git status"});
        m_Actions.push_back(new CmdAction("git --version"));
        m_Actions.push_back(new CmdAction("ls -lX"));
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