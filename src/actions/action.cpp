
#include "action.h"
#include "core/log.h"

#include <array>
#include <assert.h>
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

    Action* Action::Create(const std::string& command)
    {
        return new Action(command);
    }

    Action* Action::CreateDemo()
    {
        auto action = new Action("git fetch origin $0");
        action->m_Options.push_back({"Branch", 0, "master"});
        return action;
    }

    bool Action::isRunning() const
    {
        return m_Pipe != nullptr;
    }

    std::string Action::getName() const
    {
        static std::string prefix = "cmd: ";
        return prefix + createFullCommand();
    }

    bool Action::start()
    {
        m_Pipe = pOpen(createFullCommand().c_str());
        return m_Pipe != nullptr;
    }

    bool Action::update()
    {
        static std::array<char, 128> s_Buffer;

        if (fgets(s_Buffer.data(), 128, m_Pipe) != nullptr)
        {
            const size_t eolIndex = std::strcspn(s_Buffer.data(), "\r\n\0");
            s_Buffer[eolIndex] = '\0';
            
            CP_INFO(s_Buffer.data());
            
            return true;
        }

        stop();
        return false;
    }

    void Action::abort()
    {
        stop();
    }

    Action::Action(const std::string& command)
        : m_Command(command)
        , m_Options()
        , m_Pipe(nullptr)
    {}

    std::string Action::createFullCommand() const
    {
        std::string result{""};
        for (auto it = m_Command.cbegin(); it != m_Command.cend(); ++it)
        {
            const char c = *it;
            if (c != '$')
            {
                result += c;
                continue;
            }

            auto start = ++it; // skip $
            assert(std::isdigit(*start));

            auto last = start;
            do
                last++;
            while (last != m_Command.cend() && std::isdigit(*last));
            
            std::string idStr{start, last};
            int id = std::atoi(idStr.data());
            result += getOptionValueByID(id);

            it = --last;
        }

        return result;
    }

    const std::string& Action::getOptionValueByID(int32_t id) const
    {
        for (auto it = m_Options.cbegin(); it != m_Options.cend(); ++it)
        {
            const Option& option = *it;
            if (option.m_ID == id)
                return option.m_Value; 
        }

        static std::string s_Empty{""};
        return s_Empty;
    }

    void Action::stop()
    {
        if (m_Pipe != nullptr)
            pClose(m_Pipe);
        m_Pipe = nullptr;
    }
}