
#include "action.h"
#include "core/actionmanager.h"

#include <algorithm>
#include <cassert>
#include <charconv>
#include <cstring>

namespace coffeepot
{
#pragma region Helpers
    const char* ElectivityToString(Electivity value)
	{
        switch (value)
        {
        case Electivity::Required:
            return "Required";
        case Electivity::Optional:
            return "Optional";
        default:
            return "";
        }
	}

	Electivity StringToElectivity(const char* value)
	{
		if (0 == strcmp(value, "Required"))
			return Electivity::Required;
		if (0 == strcmp(value, "Optional"))
			return Electivity::Optional;

		return Electivity::Optional;
	}

    const char* InputTypeToString(InputType value)
	{
        switch (value)
        {
		case InputType::Text:
			return "Text";
		case InputType::MultiInput:
			return "DoubleText";
		case InputType::ComboBox:
			return "ComboBox";
		case InputType::Fixed:
			return "Fixed";
        default:
            return "";
        }
	}

    InputType StringToInputType(const char* value)
    {
        if (0 == strcmp(value, "Text"))
            return InputType::Text;
        if (0 == strcmp(value, "MultiText"))
            return InputType::MultiInput;
        if (0 == strcmp(value, "ComboBox"))
			return InputType::ComboBox;
		if (0 == strcmp(value, "Fixed"))
			return InputType::Fixed;

        return InputType::Text;
    }
#pragma endregion

    std::string Action::createFullCommand() const
    {
#if CP_WINDOWS
		std::string result{ "cmd /c " };
#else // CP_LINUX
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
                    auto& globalOptions = ActionsManager::get()->GlobalOptions;

                    assert(globalOptions.find(optionStr) != globalOptions.end());
                    result += globalOptions[optionStr];
                }
            }

			// 4. Reposition it
			it = optionEnd;
        }

        return result;
    }

	std::string Action::getOptionValueByID(int32_t id) const
	{
        for (const auto& option : m_Options)
        {
            if (option.m_Details.m_ID == id && option.b_Active)
                return option.m_Details.m_Prefix + option.m_Value;
        }

        return "";
    }
}