#pragma once

#include <string>
#include <vector>

namespace coffeepot
{
    enum class Electivity
    {
        Required = 0,
        Optional = 1
    };

    const char* ElectivityToString(Electivity value);
    Electivity StringToElectivity(const char* value);

    enum class InputType
    {
		Text = 0,
		MultiInput = 1,
		ComboBox = 2,
        Fixed = 3
    };

    const char* InputTypeToString(InputType value);
	InputType StringToInputType(const char* value);
    
    struct OptionDetails
    {
	public:
		int32_t m_ID = 0;
		std::string m_Name;

        Electivity m_Electivity;
        InputType m_InputType;

		std::string m_Prefix;
		std::vector<std::string> m_ValueList;
    };

    struct Option
    {
	public:
		std::string m_Value;
		bool b_Active = true;

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
