
#include "action.h"
#include "core/log.h"

#include <array>
#include <cassert>
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

    ActionExecutor::ActionExecutor(const Action& action)
        : m_Action(action)
        , m_Pipe(nullptr)
    {}

    bool ActionExecutor::start()
    {
        m_Pipe = pOpen(createFullCommand().c_str());
        return m_Pipe != nullptr;
    }

    bool ActionExecutor::update(char* output, size_t size)
    {
        if (fgets(output, size, m_Pipe) != nullptr)
            return true;

        stop();
        return false;
    }

    void ActionExecutor::stop()
    {
        if (m_Pipe != nullptr)
            pClose(m_Pipe);
        m_Pipe = nullptr;
    }

    std::string ActionExecutor::createFullCommand() const
    {
        std::string result{""};
        const std::string& command = m_Action.m_Command;
        for (auto it = command.cbegin(); it != command.cend(); ++it)
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
            while (last != command.cend() && std::isdigit(*last));
            
            std::string idStr{start, last};
            int id = std::atoi(idStr.data());
            result += getOptionValueByID(id);

            it = --last;
        }

        return result;
    }

    const std::string& ActionExecutor::getOptionValueByID(int32_t id) const
    {
        const std::vector<Option>& options = m_Action.m_Options;
        for (const auto& option : options)
        {
            if (option.m_ID == id)
                return option.m_Value;
        }

        static std::string s_Empty{""};
        return s_Empty;
    }
}