
#include "actionscreen.h"

#include "actions/actionmanager.h"
#include "core/log.h"

#include "imgui.h"
#include "imgui_internal.h"

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

    void InputString(const char* label, std::string& str, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None)
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

    void Checkbox(const char* label, std::string& value, const std::string& checkableValue)
    {
        ImGui::PushID(label);
        bool bSelected = value == checkableValue;
        ImGui::Checkbox("", &bSelected);
        value = bSelected ? checkableValue : "";
        ImGui::PopID();
    }
}

namespace coffeepot
{
    void ActionsScreen::tick()
    {
        if (!ImGui::Begin("Actions"))
            return;
        
        const float itemSpacing = ImGui::GetStyle().ItemSpacing.y;
        const float footerHeight = ImGui::GetFrameHeight() + itemSpacing;
        if (ImGui::BeginChild("Actions", ImVec2(0, -footerHeight)))
        {
            if (m_CreatingAction)
                renderActionBuilder();
            else
                renderActions();

            ImGui::EndChild();
        }

        renderFooter();
        ImGui::End();
    }
    
    void ActionsScreen::renderActions()
    {
        if (!ImGui::BeginTable("", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
            return;
    
        auto& actions = ActionsManager::get()->getAllActions();
        std::for_each(actions.begin(), actions.end(), [this](auto& elem) { renderAction(elem); });

        ImGui::EndTable();
    }

    void ActionsScreen::renderAction(Action& action)
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
        if (ImGui::Button("Run"))
        {
            const bool bIsActionStarted = ActionsManager::get()->executeAction(action);
            CP_DEBUG("Executing {} => {}", actionName, bIsActionStarted);
        }

        if (bTreeNode)
        {
            auto& options = action.m_Options;
			std::for_each(options.begin(), options.end(), [this](auto& elem) { renderOption(elem); });

            ImGui::TreePop();
        }

        ImGui::PopID();
    }

	void ActionsScreen::renderOption(Option& option)
	{
		ImGui::PushID(&option);
		ImGui::TableNextRow();

		// > Column 0
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx("OptionName", treeNodeFlags, option.m_Name.c_str());

		// > Column 1
		ImGui::TableSetColumnIndex(1);

		// Remove ItemSpacing
		const float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        const float tmpItemSpacing = itemSpacing * 0.5f;
		ImGui::GetStyle().ItemSpacing.x = tmpItemSpacing;

		// Set ItemWidth to leave space for the Button
		const float buttonSize = ImGui::GetFrameHeight();
		ImGui::SetNextItemWidth(- buttonSize - tmpItemSpacing);

		// Draw OptionInput
        switch (option.m_ValueInfo.m_Type)
        {
        case InputType::Text:
            ImGui::InputString("OptionValue", option.m_Value, ImGuiInputTextFlags_CharsNoBlank);
            break;
        case InputType::Checkbox:
            ImGui::Checkbox("OptionValue", option.m_Value, option.m_ValueInfo.m_Default);
            break;
        case InputType::ComboBox:
            ImGui::ComboBox("OptionValue", option.m_Value, option.m_ValueInfo.m_Choices);
            break;
        }

		// Draw Button
		ImGui::SameLine();
		if (ImGui::Button("", ImVec2(buttonSize, buttonSize)))
			option.m_Value = option.m_ValueInfo.m_Default;

		// Restore ItemSpacing
		ImGui::GetStyle().ItemSpacing.x = itemSpacing;

		ImGui::PopID();
	}

    void ActionsScreen::renderActionBuilder()
    {
        ImGui::PushID(&m_ActionTemplate);
        if (!ImGui::BeginTable("", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
            return;
        
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
        ImGui::Text("Name");
        ImGui::TableSetColumnIndex(1);
		ImGui::InputString("ActionName", m_ActionTemplate.m_Name);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
        ImGui::Text("Command");
        ImGui::TableSetColumnIndex(1);
		ImGui::InputString("ActionCommand", m_ActionTemplate.m_Command);

        if (m_ActionTemplate.m_Options.size() != 0)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
		    ImGui::AlignTextToFramePadding();
            ImGui::Text("Options");

            auto& options = m_ActionTemplate.m_Options;
            std::for_each(options.begin(), options.end(), [this](auto& elem) { renderOptionBuilder(elem); });
        }

        ImGui::EndTable();
        ImGui::Separator();

        if (ImGui::Button("Add Option"))
        {
            Option& newOption = m_ActionTemplate.m_Options.emplace_back();
            newOption.m_ID = static_cast<int32_t>(m_ActionTemplate.m_Options.size());
            newOption.m_ValueInfo.m_Type = InputType::Text;
        }

        ImGui::PopID();
    }

    void ActionsScreen::renderOptionBuilder(Option& option)
    {
        ImGui::PushID(&option);

        // First line
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
        ImGui::Bullet(); ImGui::SameLine();
        const float cursonPosX = ImGui::GetCursorPosX(); // for alignment
        ImGui::Text("Name");

        ImGui::TableSetColumnIndex(1);
        ImGui::InputString("OptionName", option.m_Name);

		// Second line
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
        ImGui::SetCursorPosX(cursonPosX); // align to previous line
        ImGui::Text("DefaultValue");

        ImGui::TableSetColumnIndex(1);
        ImGui::InputString("OptionDefaultValue", option.m_ValueInfo.m_Default);

        ImGui::PopID();
    }

    void ActionsScreen::renderFooter()
    {
        if (m_CreatingAction)
        {
            if (ImGui::Button("Save Action"))
            {
                m_CreatingAction = false;
                ActionsManager::get()->addAction(m_ActionTemplate);
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                m_CreatingAction = false;
            }
        }
        else
        {
            if (ImGui::Button("Create new Action"))
            {
                m_CreatingAction = true;
                m_ActionTemplate = Action{};
            }
        }
    }
}