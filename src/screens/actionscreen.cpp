
#include "actionscreen.h"

#include "core/actionmanager.h"
#include "utils/utils.h"

#include "imgui.h"

#include <algorithm>

namespace coffeepot
{
     ActionsScreen::ActionsScreen()
        : ScreenWithFooter("Actions")
        , b_CreatingAction(false)
        , m_ActionTemplate()
     {}

    void ActionsScreen::tickContent()
    {
        if (b_CreatingAction)
            renderActionBuilder();
        else
            renderActions();
    }
    
    void ActionsScreen::tickFooter()
    {
        switch (ImGui::BuilderFooter("Action", b_CreatingAction))
        {
            case ImGui::BuilderFooterResult::Save:
                
                // Finalize ActionTemplate: set Default value
                for (auto& option : m_ActionTemplate.m_Options)
					option.m_Value = option.m_Details.m_ValueList[0];

                ActionsManager::get()->Actions.push_back(m_ActionTemplate);
                break;
            case ImGui::BuilderFooterResult::Start:
                m_ActionTemplate = Action{};
                break;
        }
    }
    
    void ActionsScreen::renderActions()
    {
        if (!ImGui::BeginTable("", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
            return;
    
        auto& actions = ActionsManager::get()->Actions;
        std::for_each(actions.begin(), actions.end(), [this](auto& elem) { renderAction(elem); });

        ImGui::EndTable();
    }
    
    void ActionsScreen::renderAction(Action& action)
    {
        if (ImGui::ActionTree(action, /*bCanRun =*/ true))
        {
            const bool bResult = ActionsManager::get()->executeAction(action);
        }
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
            std::for_each(options.begin(), options.end(), [this](auto& elem) { renderOptionBuilder(elem.m_Details); });
        }

        ImGui::EndTable();
        ImGui::Separator();

        if (ImGui::Button("Add Option"))
        {
            Option& newOption = m_ActionTemplate.m_Options.emplace_back();
			newOption.m_Details.m_ID = static_cast<int32_t>(m_ActionTemplate.m_Options.size());
			newOption.m_Details.m_Name = "optionName";
			newOption.m_Details.m_Electivity = Electivity::Required;
			newOption.m_Details.m_InputType = InputType::Text;
			newOption.m_Details.m_ValueList.emplace_back("");
        }

        ImGui::PopID();
    }

    void ActionsScreen::renderOptionBuilder(OptionDetails& optionDetails)
    {
        ImGui::PushID(&optionDetails);
        float horizontalAlignment = 0;

        // 1st line: Name
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGui::Bullet();
            ImGui::SameLine();

            horizontalAlignment = ImGui::GetCursorPosX(); // for alignment
			ImGui::Text("Name");

			ImGui::TableSetColumnIndex(1);
			ImGui::InputString("OptionName", optionDetails.m_Name);
		}

		// 2nd line: Electivity
        {
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGui::SetCursorPosX(horizontalAlignment); // align to previous line
			ImGui::Text("Electivity");

			ImGui::TableSetColumnIndex(1);

			static_assert(sizeof(Electivity) == sizeof(int32_t));
			if (ImGui::InputInt("Electivity", (int32_t*)&optionDetails.m_Electivity))
			{
                auto intValue = static_cast<int32_t>(optionDetails.m_Electivity);
				if (intValue < 0 || intValue > 1)
					optionDetails.m_Electivity = Electivity::Required;
			}

			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
				ImGui::SetTooltip("%d: %s\n%d: %s", 0, "Required", 1, "Optional");
        }

		// 3rd line: InputType
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGui::SetCursorPosX(horizontalAlignment); // align to previous line
			ImGui::Text("InputType");

			ImGui::TableSetColumnIndex(1);

			static_assert(sizeof(InputType) == sizeof(int32_t));
			if (ImGui::InputInt("InputType", (int32_t*)&optionDetails.m_InputType))
			{
				auto intValue = static_cast<int32_t>(optionDetails.m_InputType);
				if (intValue < 0 || intValue > 3)
					optionDetails.m_InputType = InputType::Text;

                // Only ComboBox allows multiple elements in the ValueList
				if (optionDetails.m_InputType != InputType::ComboBox && optionDetails.m_ValueList.size() > 1)
					optionDetails.m_ValueList.resize(1);
			}

			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
				ImGui::SetTooltip("%d: %s\n%d: %s\n%d: %s\n%d: %s", 0, "Text", 1, "OptionalText", 2, "ComboBox", 3, "Fixed");
		}

		// 4th line: Prefix
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGui::SetCursorPosX(horizontalAlignment); // align to previous line
			ImGui::Text("Prefix");

			ImGui::TableSetColumnIndex(1);
            ImGui::InputString("Prefix", optionDetails.m_Prefix);
		}

		// 5th+ lines: Values
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGui::SetCursorPosX(horizontalAlignment); // align to previous line
			ImGui::Text("ValueList");

			ImGui::TableSetColumnIndex(1);
            
            for (int32_t index = 0; index < optionDetails.m_ValueList.size(); index++)
			{
				ImGui::PushID(index);
				ImGui::InputString("Value", optionDetails.m_ValueList[index]);
                ImGui::PopID();
			}

			if (optionDetails.m_InputType == InputType::ComboBox && ImGui::Button("Add Value"))
				optionDetails.m_ValueList.emplace_back("");
		}

        ImGui::PopID();
    }
}
