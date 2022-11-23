#pragma  once

#include "core/action.h"
#include "imgui.h"

#include <map>
#include <string>
#include <vector>

namespace ImGui
{
    void InputString(const char* label, std::string& str, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);
    bool Checkbox(const char* label, bool& bChecked);
	bool IconButton(const char* label);

	bool ComboBoxStrings(const char* label, std::string& selectedValue, const std::vector<std::string>& selectableValues);
	bool ComboBoxActions(const char* label, coffeepot::Action& selectedValue, const std::vector<coffeepot::Action>& selectableValues, bool bShowEmpty = false);

	template<typename Enum>
	bool ComboBoxEnums(const char* label, Enum& selectedValue, const std::map<Enum, const char*>& selectableValuesToNames)
	{
		ImGui::PushID(label);
		bool bSelected = false;

		if (ImGui::BeginCombo("", selectableValuesToNames.at(selectedValue)))
		{
			for (const auto& [value, name] : selectableValuesToNames)
			{
				const bool bIsSelectedValue = selectedValue == value;
				if (ImGui::Selectable(name, bIsSelectedValue))
				{
					selectedValue = value;
					bSelected = true;
				}
			}

			ImGui::EndCombo();
		}

		ImGui::PopID();
		return bSelected;
	}

	void OptionRow(coffeepot::Option& option);
}

