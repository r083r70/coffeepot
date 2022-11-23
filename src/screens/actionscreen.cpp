
#include "actionscreen.h"

#include "core/actionmanager.h"
#include "fa_icons.h"
#include "utils/utils.h"

#include "imgui.h"

#include <algorithm>
#include <random>

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
		if (b_CreatingAction)
		{
			if (ImGui::Button("Save Action"))
			{
				// Finalize options
				for (auto& option : m_ActionTemplate.m_Options)
				{
					option.m_Value = option.m_Details.m_ValueList[0];
					option.b_Active = option.m_Details.m_Electivity == Electivity::Required;
				}

				ActionsManager::get()->Actions.push_back(m_ActionTemplate);
				b_CreatingAction = false;
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
				b_CreatingAction = false;
		}
		else
		{
			if (ImGui::Button("Create new Action"))
			{
				static std::random_device randomDevice;
				static std::mt19937 engine{ randomDevice() };
				static std::uniform_int_distribution<int32_t> UniformDistribution{};

				// Init a Action
				m_ActionTemplate = Action{};
				m_ActionTemplate.m_ID = UniformDistribution(engine);

				b_CreatingAction = true;
			}

			// #TODO
			// Create from template
		}
	}
    
    void ActionsScreen::renderActions()
    {
        if (!ImGui::BeginTable("", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
            return;
    
		auto& actions = ActionsManager::get()->Actions;

		int32_t swapIndex1 = -1, swapIndex2 = -1;
		const auto DrawIconButtonEx = [](const char* label, bool bEnabled) -> bool
		{
			ImGui::BeginDisabled(!bEnabled);
			const bool bResult = ImGui::IconButton(label);
			ImGui::EndDisabled();
			return bResult;
		};


		for (int32_t i = 0; i < actions.size(); i++)
		{
			Action& action = actions[i];
			const bool bIsFirst = i == 0;
			const bool bIsLast = i == actions.size() - 1;

			ImGui::PushID(&action);
			const std::string& actionName = action.m_Name;

			ImGui::TableNextRow();
			bool bTreeNode = false;

			// First Row
			{
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();

				if (action.m_Options.size() != 0)
					bTreeNode = ImGui::TreeNode("Action", actionName.c_str());
				else
					ImGui::BulletText(actionName.c_str());

				ImGui::TableSetColumnIndex(1);

				if (m_RenamingAction == &action)
				{
					if (ImGui::IconButton(ICON_FA_PEN_TO_SQUARE))
					{
						action.m_Name = m_NewActionName;
						m_RenamingAction = nullptr;
					}

					ImGui::SameLine();
					ImGui::SetNextItemWidth(-FLT_MIN);
					ImGui::InputString("NewName", m_NewActionName);
				}
				else if (m_DeletingAction == &action)
				{
					if (ImGui::IconButton(ICON_FA_CHECK))
						b_DeleteConfirmed = true;

					ImGui::SameLine(0.f, 3);
					if (ImGui::IconButton(ICON_FA_XMARK))
						m_DeletingAction = nullptr;
				}
				else
				{
					// Run
					if (ImGui::IconButton(ICON_FA_PLAY))
						ActionsManager::get()->executeAction(action);

					// Move Up
					ImGui::SameLine(0.f, 3);
					if (DrawIconButtonEx(ICON_FA_ARROW_UP, !bIsFirst))
					{
						swapIndex1 = i;
						swapIndex2 = i - 1;
					}

					// Move Down
					ImGui::SameLine(0.f, 3);
					if (DrawIconButtonEx(ICON_FA_ARROW_DOWN, !bIsLast))
					{
						swapIndex1 = i;
						swapIndex2 = i + 1;
					}

					// Rename
					ImGui::SameLine(0.f, 3);
					if (ImGui::IconButton(ICON_FA_PEN))
					{
						m_RenamingAction = &action;
						m_NewActionName = actionName;
					}

					// Delete
					ImGui::SameLine(0.f, 3);
					if (ImGui::IconButton(ICON_FA_TRASH))
					{
						m_DeletingAction = &action;
						b_DeleteConfirmed = false;
					}
				}
			}

			// Show Options
			if (bTreeNode)
			{
				auto& options = action.m_Options;
				std::for_each(options.begin(), options.end(), [](auto& elem) { ImGui::OptionRow(elem); });

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		ImGui::EndTable();

		// Swap if needed
		if (swapIndex1 != -1 && swapIndex2 != -1)
		{
			const auto tmp = actions[swapIndex1];
			actions[swapIndex1] = actions[swapIndex2];
			actions[swapIndex2] = tmp;
		}

		// Perform Delete at the end of tick
		if (m_DeletingAction != nullptr && b_DeleteConfirmed)
		{
			auto actionToErase = std::remove_if(actions.begin(), actions.end(), [this](auto& elem) { return &elem == m_DeletingAction; });
			actions.erase(actionToErase);

			// Reset
			m_DeletingAction = nullptr;
			b_DeleteConfirmed = false;
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
