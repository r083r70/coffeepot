#pragma once

#include <string>
#include <vector>

namespace coffeepot
{
    enum class InputType
    {
        Text,
        Checkbox,
        ComboBox
    };
    
    struct ValueInfo
    {
    public:
        InputType m_Type;

        std::string m_Default;
        std::vector<std::string> m_Choices;
    };

    struct Option
    {
    public:
        int32_t m_ID;
        std::string m_Name;
        std::string m_Value;

        ValueInfo m_ValueInfo;
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
        bool update(char* output);
        void stop();

    protected:
        std::string createFullCommand() const;
        const std::string& getOptionValueByID(int32_t id) const;

    private:
        Action m_Action;
        FILE* m_Pipe;
    };
}
