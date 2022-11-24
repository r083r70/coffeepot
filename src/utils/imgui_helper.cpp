
#include "utils/imgui_helper.h"

#include "core/action.h"
#include "fa_icons.h"

#include <algorithm>

namespace ImGui
{
    int inputStringCallback(ImGuiInputTextCallbackData* data)
	{
        if (data->EventFlag != ImGuiInputTextFlags_CallbackResize)
            return 0;

		auto str = static_cast<std::string*>(data->UserData);
		str->resize(data->BufTextLen);
		data->Buf = const_cast<char*>(str->c_str());
		return 1;
	}

    void InputString(const char* label, std::string& str, ImGuiInputTextFlags flags /*= ImGuiInputTextFlags_None*/)
    {
		ImGui::PushID(label);
        flags |= ImGuiInputTextFlags_CallbackResize;
		ImGui::InputText("", str.data(), str.size() + 1, flags, &inputStringCallback, &str);
		ImGui::PopID();
    }

	bool Checkbox(const char* label, bool& bChecked)
	{
		ImGui::PushID(label);
		const bool bPressed = ImGui::Checkbox("", &bChecked);
		ImGui::PopID();
        return bPressed;
    }

	bool IconButton(const char* label)
	{
		auto OldFramePaddingX = ImGui::GetStyle().FramePadding.x;
		ImGui::GetStyle().FramePadding.x = 2;
		const bool bResult = ImGui::Button(label);

		ImGui::GetStyle().FramePadding.x = OldFramePaddingX;
		return bResult;
	}

    bool  ComboBoxStrings(const char* label, std::string& selectedValue, const std::vector<std::string>& selectableValues)
    {
		ImGui::PushID(label);
		bool bSelected = false;

        if (ImGui::BeginCombo("", selectedValue.c_str()))
        {
            for (const auto& value : selectableValues)
			{
				const bool bIsSelectedValue = selectedValue == value;
                if (ImGui::Selectable(value.c_str(), bIsSelectedValue))
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

	bool ComboBoxActions(const char* label, coffeepot::Action& selectedValue, const std::vector<coffeepot::Action>& selectableValues, bool bShowEmpty /*= false*/)
	{
		ImGui::PushID("TemplateActionSelector");
		bool bSelected = false;

		if (ImGui::BeginCombo("", selectedValue.m_Name.c_str()))
		{
			// Empty Action
			const bool bIsSelectedValueEmpty = selectedValue.m_ID == -1;
			if (bShowEmpty && ImGui::Selectable("-- Empty --", bIsSelectedValueEmpty))
			{
				selectedValue = coffeepot::Action{};
				bSelected = true;
			}

			// Existing Actions
			for (const auto& action : selectableValues)
			{
				const bool bIsSelectedValue = selectedValue.m_ID == action.m_ID;
				if (ImGui::Selectable(action.m_Name.c_str(), bIsSelectedValue))
				{
					selectedValue = action;
					bSelected = true;
				}
			}

			ImGui::EndCombo();
		}

		ImGui::PopID();
		return bSelected;
	}

	void OptionRow(coffeepot::Option& option)
    {
		ImGui::PushID(&option);
		ImGui::TableNextRow();

		// > Column 0
		{
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			ImGui::TreeNodeEx("OptionName", treeNodeFlags, option.m_Details.m_Name.c_str());
		}

		// > Column 1
		{
			ImGui::TableSetColumnIndex(1);
			ImGui::AlignTextToFramePadding();

			// Checkbox
			const bool bIsRequired = option.m_Details.m_Electivity == coffeepot::Electivity::Required;
			if (!bIsRequired)
			{
				if (ImGui::Checkbox("OptionActive", option.b_Active))
					option.m_Value = option.b_Active && !option.m_Details.m_ValueList.empty() ? option.m_Details.m_ValueList[0] : "";

				ImGui::SameLine();
			}

			ImGui::BeginDisabled(!bIsRequired && !option.b_Active);

			// Input
			switch (option.m_Details.m_InputType)
			{
			case coffeepot::InputType::Text:
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputString("OptionValue", option.m_Value, ImGuiInputTextFlags_CharsNoBlank);
				break;
			case coffeepot::InputType::MultiInput:
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputString("OptionValue", option.m_Value);
				break;
			case coffeepot::InputType::ComboBox:
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::ComboBoxStrings("OptionValue", option.m_Value, option.m_Details.m_ValueList);
				break;
			}

			ImGui::EndDisabled();
		}

		ImGui::PopID();
    }
}
