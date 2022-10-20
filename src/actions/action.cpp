
#include "action.h"
#include "core/app.h"

#include <algorithm>
#include <cassert>
#include <charconv>

namespace coffeepot
{
	std::string InputTypeToString(InputType value)
	{
        switch (value)
        {
		case InputType::Text:
			return "Text";
		case InputType::Checkbox:
			return "Checkbox";
		case InputType::ComboBox:
			return "ComboBox";
        default:
            return "";
        }
	}

	InputType StringToInputType(std::string value)
	{
        if (value == "Text")
			return InputType::Text;
		if (value == "OptionalText")
			return InputType::OptionalText;
		if (value == "Checkbox")
			return InputType::Checkbox;
        if (value == "ComboBox")
            return InputType::ComboBox;

        return InputType::Text;
	}

    std::string Action::createFullCommand() const
    {
#if CP_WINDOWS
		std::string result{ "cmd /c " };
#elif CP_LINUX
		std::string result{ "" };
#endif

        uint32_t latestOptionIndex = 0;
        for (auto it = m_Command.cbegin(); it != m_Command.cend(); ++it)
        {
            const char character = *it;
            if (character != '{')
            {
                result += character;
                continue;
            }

            // 1. Found optionsBegin
			auto optionsBegin = ++it;
			assert(optionsBegin != m_Command.cend());

			// 2. Reach optionEnd
            auto optionEnd = optionsBegin;
            while (*optionEnd != '}')
			{
				optionEnd++;
                assert(optionEnd != m_Command.cend());
            }

            // 3a. Empty options
            if (optionsBegin == optionEnd)
			{
				result += getOptionValueByID(latestOptionIndex);
                latestOptionIndex++;
            }
            else
			{
	    		const std::string optionStr{ optionsBegin, optionEnd };
                const bool numberic = std::all_of(optionsBegin, optionEnd, [](const auto& elem) { return std::isdigit(elem); });

				// 3b. Numeric options
                if (numberic)
				{
                    int32_t numbericOption = std::stoi(optionStr);
					result += getOptionValueByID(numbericOption);
				}
				// 3c. Named options
                else
                {
                    auto& globalOptions = App::get()->getGlobalOptions();

                    assert(globalOptions.find(optionStr) != globalOptions.end());
                    result += globalOptions[optionStr];
                }
            }

			// 4. Reposition it
			it = optionEnd;
        }

        return result;
    }

    const std::string& Action::getOptionValueByID(int32_t id) const
	{
		static std::string s_Empty;

        for (const auto& option : m_Options)
        {
            if (option.m_Details.m_ID != id)
                continue;

            switch (option.m_Details.m_Type)
            {
			case InputType::Text:
			case InputType::ComboBox:
				return option.m_Value;
			case InputType::OptionalText:
			case InputType::Checkbox:
				return option.b_Enabled ? option.m_Value : s_Empty;
            }
        }

        return s_Empty;
    }
}