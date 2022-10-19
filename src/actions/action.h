#pragma once

#include <string>
#include <vector>

namespace coffeepot
{
    enum class InputType
    {
        Text,
        OptionalText,
        Checkbox,
        ComboBox
    };

	std::string InputTypeToString(InputType value);
	InputType StringToInputType(std::string value);
    
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
        std::string createFullCommand() const;
        const std::string& getOptionValueByID(int32_t id) const;

    public:
        int32_t m_ID;
        std::string m_Name;

        std::string m_Command;
        std::vector<Option> m_Options;
    };
}
