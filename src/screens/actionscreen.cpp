
#include "actionscreen.h"

#include "core/actionmanager.h"
#include "fa_icons.h"
#include "utils/utils.h"

#include "imgui.h"

#include <algorithm>
#include <map>
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
			if (ImGui::IconButton(ICON_FA_CHECK))
			{
				// Set a Name if not was given
				if (m_ActionTemplate.m_Name.empty())
					m_ActionTemplate.m_Name = "New Action";

				// Finalize Options
				for (auto& option : m_ActionTemplate.m_Options)
				{
					option.m_Value = option.m_Details.m_ValueList[0];
					option.b_Active = option.m_Details.m_Electivity == Electivity::Required;
				}

				ActionsManager::get()->Actions.push_back(m_ActionTemplate);
				b_CreatingAction = false;
			}

			ImGui::SameLine();
			if (ImGui::IconButton(ICON_FA_XMARK))
				b_CreatingAction = false;
		}
		else
		{
			ImGui::ComboBoxActions("SelectTemplateAction", m_ActionTemplate, ActionsManager::get()->Actions, /*bShowEmpty =*/ true);

			ImGui::SameLine();
			if (ImGui::IconButton(ICON_FA_PLUS))
			{
				static std::random_device randomDevice;
				static std::mt19937 engine{ randomDevice() };
				static std::uniform_int_distribution<int32_t> UniformDistribution{};

				// Init a Action
				m_ActionTemplate.m_ID = UniformDistribution(engine);
				m_ActionTemplate.m_Name = "New Action";
				b_CreatingAction = true;
			}
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

			// Action Name
			{
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();

				if (action.m_Options.size() != 0)
					bTreeNode = ImGui::TreeNode("Action", actionName.c_str());
				else
					ImGui::BulletText(actionName.c_str());

				ImGui::TableSetColumnIndex(1);
				ImGui::AlignTextToFramePadding();

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

					ImGui::SameLine(0.f, 2);
					ImGui::Text("|");

					// Move Up
					ImGui::SameLine(0.f, 2);
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
        
		// Name
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Name");

			ImGui::TableSetColumnIndex(1);
			ImGui::AlignTextToFramePadding();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::InputString("ActionName", m_ActionTemplate.m_Name);
		}

		// Command
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Command");

			ImGui::TableSetColumnIndex(1);
			ImGui::AlignTextToFramePadding();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::InputString("ActionCommand", m_ActionTemplate.m_Command);
		}

		// Show Options
        if (m_ActionTemplate.m_Options.size() != 0)
		{
			auto& options = m_ActionTemplate.m_Options;
            std::for_each(options.begin(), options.end(), [this](auto& elem) { renderOptionBuilder(elem.m_Details); });
		}

		// Manage Options
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - 43);
			if (ImGui::IconButton(ICON_FA_CIRCLE_PLUS))
			{
				Option& newOption = m_ActionTemplate.m_Options.emplace_back();
				newOption.m_Details.m_ID = static_cast<int32_t>(m_ActionTemplate.m_Options.size());
				newOption.m_Details.m_Name = "New Option";
				newOption.m_Details.m_ValueList.emplace_back(""); // Add a Value as the Default one
			}

			ImGui::SameLine(0.f, 2);
			ImGui::BeginDisabled(m_ActionTemplate.m_Options.empty());
			if (ImGui::IconButton(ICON_FA_CIRCLE_MINUS))
				m_ActionTemplate.m_Options.pop_back();
			ImGui::EndDisabled();
		}

        ImGui::EndTable();
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
			ImGui::Text("Option Name");

			ImGui::TableSetColumnIndex(1);
			ImGui::AlignTextToFramePadding();
			ImGui::SetNextItemWidth(-FLT_MIN);
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
			ImGui::AlignTextToFramePadding();
			ImGui::SetNextItemWidth(-FLT_MIN);
			const std::map<Electivity, const char*> Electivities = { { Electivity::Required, "Required" }, { Electivity::Optional, "Optional" } };
			ImGui::ComboBoxEnums("Electivity", optionDetails.m_Electivity, Electivities);
		}

		// 3rd line: InputType
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGui::SetCursorPosX(horizontalAlignment); // align to previous line
			ImGui::Text("Type");

			ImGui::TableSetColumnIndex(1);
			ImGui::AlignTextToFramePadding();
			ImGui::SetNextItemWidth(-FLT_MIN);
			const std::map<InputType, const char*> InputTypes = { { InputType::Text, "Text" }, { InputType::MultiInput, "MultiInput" }, { InputType::ComboBox, "ComboBox" }, { InputType::Fixed, "Fixed" } };
			if (ImGui::ComboBoxEnums("InputText", optionDetails.m_InputType, InputTypes))
			{
				if (optionDetails.m_InputType != InputType::ComboBox && optionDetails.m_ValueList.size() > 1)
					optionDetails.m_ValueList.resize(1);
			}
		}

		// 4th line: Prefix
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGui::SetCursorPosX(horizontalAlignment); // align to previous line
			ImGui::Text("Prefix");

			ImGui::TableSetColumnIndex(1);
			ImGui::AlignTextToFramePadding();
			ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputString("Prefix", optionDetails.m_Prefix);
		}

		// 5th+ lines: Values
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGui::SetCursorPosX(horizontalAlignment); // align to previous line

			if (optionDetails.m_InputType == InputType::ComboBox)
				ImGui::Text("Values");
			else
				ImGui::Text("Default Value");

			ImGui::TableSetColumnIndex(1);
			ImGui::AlignTextToFramePadding();
            
            for (int32_t index = 0; index < optionDetails.m_ValueList.size(); index++)
			{
				ImGui::PushID(index);
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputString("Value", optionDetails.m_ValueList[index]);
                ImGui::PopID();
			}

			if (optionDetails.m_InputType == InputType::ComboBox)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - 43);

				if (ImGui::IconButton(ICON_FA_SQUARE_PLUS))
					optionDetails.m_ValueList.emplace_back("");

				ImGui::SameLine(0.f, 2);
				ImGui::BeginDisabled(optionDetails.m_ValueList.size() == 1);
				if (ImGui::IconButton(ICON_FA_SQUARE_MINUS))
					optionDetails.m_ValueList.pop_back();
				ImGui::EndDisabled();
			}
		}

        ImGui::PopID();
    }
}
