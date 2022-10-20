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
    
    struct OptionDetails
    {
	public:
		int32_t m_ID = 0;
		std::string m_Name;

        InputType m_Type;
		std::vector<std::string> m_ValueList;
    };

    struct Option
    {
    public:
		std::string m_Value;
		bool b_Enabled = true;

        OptionDetails m_Details;
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
