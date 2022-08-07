
#include "action.h"
#include <cassert>

namespace coffeepot
{
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
            int id = std::atoi(idStr.c_str());
            result += getOptionValueByID(id);

            it = --last;
        }

        return result;
    }

    const std::string& Action::getOptionValueByID(int32_t id) const
    {
        for (const auto& option : m_Options)
        {
            if (option.m_ID == id)
                return option.m_Value;
        }

        static std::string s_Empty{""};
        return s_Empty;
    }
}