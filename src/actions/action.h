#pragma once

#include <string>
#include <vector>

namespace coffeepot
{
    enum class OptionType
    {
        Alphanumeric,
        Alphabetic,
        Numeric,
        Selection
    };

    struct Option
    {
    public:
        int32_t m_ID;
        std::string m_Name;

        std::string m_Value;
        std::string m_DefaultValue;

        OptionType m_OptionType;
        std::vector<std::string> m_SelectableValues;
    };

    struct Action
    {
    public:
        int32_t m_ID;
        std::string m_Name;

        std::string m_Command;
        std::vector<Option> m_Options;
    };

    class ActionExecutor
    {
    public:
        ActionExecutor(const Action& action);
        ~ActionExecutor() { stop(); }

        bool start();
        bool update();
        void stop();

    protected:
        std::string createFullCommand() const;
        const std::string& getOptionValueByID(int32_t id) const;

    private:
        Action m_Action;
        FILE* m_Pipe;
    };
}
