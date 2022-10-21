
#include "utils.h"

#include"actions/action.h"
#include"actions/playlist.h"
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

    void ComboBox(const char* label, std::string& selectedValue, const std::vector<std::string>& selectableValues)
    {
		ImGui::PushID(label);
        if (ImGui::BeginCombo("", selectedValue.c_str()))
        {
            for (const auto& value : selectableValues)
            {
                bool bSelected = selectedValue == value;
                ImGui::Selectable(value.c_str(), &bSelected);

                if (bSelected)
                    selectedValue = value;
            }
            ImGui::EndCombo();
        }
		ImGui::PopID();
    }

    bool Checkbox(const char* label, bool& bChecked)
	{
		ImGui::PushID(label);
		const bool bPressed = ImGui::Checkbox("", &bChecked);
		ImGui::PopID();
        return bPressed;
    }

    bool PlaylistTree(coffeepot::Playlist& playlist, bool bCanRun /*= false*/)
    {
        ImGui::PushID(&playlist);
        const std::string& playlistName = playlist.m_Name;

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        const bool bTreeNode = ImGui::TreeNode("Playlist", playlistName.c_str());
        
        ImGui::TableSetColumnIndex(1);
        const bool bResult = bCanRun && ImGui::Button("Run");
        

        if (bTreeNode)
        {
            auto& actions = playlist.getActions();         
            std::for_each(actions.begin(), actions.end(), [](auto& elem) { ActionTree(elem); });

            ImGui::TreePop();
        }

        ImGui::PopID();
        return bResult;
    }

    bool ActionTree(coffeepot::Action& action, bool bCanRun /*= false*/)
    {
        ImGui::PushID(&action);
        const std::string& actionName = action.m_Name;

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();

        bool bTreeNode = false;
        if (action.m_Options.size() != 0)
            bTreeNode = ImGui::TreeNode("Action", actionName.c_str());
        else
            ImGui::BulletText(actionName.c_str());

        ImGui::TableSetColumnIndex(1);
        const bool bResult = bCanRun && ImGui::Button("Run");

        if (bTreeNode)
        {
            auto& options = action.m_Options;
			std::for_each(options.begin(), options.end(), [](auto& elem) { OptionTree(elem); });

            ImGui::TreePop();
        }

        ImGui::PopID();
        return bResult;
    }

    void OptionTree(coffeepot::Option& option)
    {
		ImGui::PushID(&option);
		ImGui::TableNextRow();

		// > Column 0
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx("OptionName", treeNodeFlags, option.m_Details.m_Name.c_str());

		// > Column 1
		ImGui::TableSetColumnIndex(1);

		// Remove ItemSpacing
		const float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        const float tmpItemSpacing = itemSpacing * 0.5f;
		ImGui::GetStyle().ItemSpacing.x = tmpItemSpacing;

		// Set ItemWidth to leave space for the Button
		const float buttonSize = ImGui::GetFrameHeight();
        
		// Draw OptionInput
        switch (option.m_Details.m_Type)
        {
		case coffeepot::InputType::Text:
		{
			ImGui::SetNextItemWidth(-buttonSize - tmpItemSpacing);
			ImGui::InputString("OptionValue", option.m_Value, ImGuiInputTextFlags_CharsNoBlank);
		} break;
		case coffeepot::InputType::OptionalText:
		{
			if (ImGui::Checkbox("OptionEnabled", option.b_Enabled) && !option.b_Enabled)
				option.m_Value = "";
			ImGui::SameLine();
			if (!option.b_Enabled) ImGui::BeginDisabled();
			ImGui::SetNextItemWidth(-buttonSize - tmpItemSpacing);
			ImGui::InputString("OptionValue", option.m_Value, ImGuiInputTextFlags_CharsNoBlank);
			if (!option.b_Enabled) ImGui::EndDisabled();
		} break;
		case coffeepot::InputType::Checkbox:
		{
			ImGui::Checkbox("OptionEnabled", option.b_Enabled);
		} break;
		case coffeepot::InputType::ComboBox:
		{
			ImGui::SetNextItemWidth(-buttonSize - tmpItemSpacing);
			ImGui::ComboBox("OptionValue", option.m_Value, option.m_Details.m_ValueList);
		} break;
        }

		// Draw Button
		ImGui::SameLine();
        if (ImGui::Button("", ImVec2(buttonSize, buttonSize)))
		{
			option.b_Enabled = true;
			option.m_Value = !option.m_Details.m_ValueList.empty() ? option.m_Details.m_ValueList[0] : "";
        }

		// Restore ItemSpacing
		ImGui::GetStyle().ItemSpacing.x = itemSpacing;

		ImGui::PopID();
    }

    BuilderFooterResult BuilderFooter(const char* label, bool& value)
    {
        if (value)
        {
            std::string saveButton = "Save ";
            saveButton += label;

            if (ImGui::Button(saveButton.c_str()))
            {
                value = false;
                return BuilderFooterResult::Save;
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                value = false;
                return BuilderFooterResult::Cancel;
            }
        }
        else
        {
            std::string createButton = "Create new ";
            createButton += label;

            if (ImGui::Button(createButton.c_str()))
            {
                value = true;
                return BuilderFooterResult::Start;
            }
        }

        return BuilderFooterResult::None;
    }
}
