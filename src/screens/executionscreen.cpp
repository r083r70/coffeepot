
#include "executionscreen.h"

#include <cassert>

#include "imgui.h"
#include "imgui_internal.h"

#include "core/actionmanager.h"
#include "fa_icons.h"
#include "utils/imgui_helper.h"

namespace coffeepot
{
	void ExecutionScreen::tickContent()
	{
		const bool bIsExecuting = ActionsManager::get()->isExecuting();

		const ExecutionPlaylist& playlist = ActionsManager::get()->getExecutionPlaylist();

		const auto& actions = playlist.getActions();
		const auto& actionStates = playlist.getActionStates();
		assert(actions.size() == actionStates.size());

		for (int32_t index = 0; index < actions.size(); index++)
		{
			listAction(actions[index], index, actionStates[index], bIsExecuting);
		}
    }

	void ExecutionScreen::tickFooter()
	{
		const bool bIsExecuting = ActionsManager::get()->isExecuting();

		ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x * 0.5f - 25);

		// ExecutionActive - Commands
		ImGui::BeginDisabled(!bIsExecuting);
		{
			if (ImGui::IconButton(ICON_FA_SKULL))
				ActionsManager::get()->killExecution();
		}
		ImGui::EndDisabled();

		// ExecutionInactive - Commands
		ImGui::BeginDisabled(bIsExecuting);
		{
			ImGui::SameLine(0.f, 3);
			if (ImGui::IconButton(ICON_FA_TRASH_CAN))
				ActionsManager::get()->clearExecutionPlaylist();

			ImGui::SameLine(0.f, 3);
			if (ImGui::IconButton(ICON_FA_ROTATE_RIGHT))
				ActionsManager::get()->restartExecutionPlaylist();
		}
		ImGui::EndDisabled();
	}

	void ExecutionScreen::listAction(const Action& action, int32_t actionIndex, ActionState actionState, bool bIsExecutionActive)
	{
		ImGui::PushID(&action);

		const float ContentRegionAvail = ImGui::GetContentRegionAvail().x;

		switch (actionState)
		{
		case ActionState::Ready:		ImGui::Text(ICON_FA_HOURGLASS_EMPTY); break;
		case ActionState::Running:		ImGui::Text(ICON_FA_BOLT); break;
		case ActionState::Success:		ImGui::Text(ICON_FA_CHECK); break;
		case ActionState::Fail:			ImGui::Text(ICON_FA_SKULL_CROSSBONES); break;
		case ActionState::Aborted:		ImGui::Text(ICON_FA_BAN); break;
		default:						assert(false);
		}

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 0.8f);
		ImGui::SameLine();
		ImGui::Text(action.m_Name.c_str());

		const bool bCanRemoveAction = actionState == ActionState::Ready || !bIsExecutionActive;
		ImGui::BeginDisabled(!bCanRemoveAction);
		{
			ImGui::SameLine(ContentRegionAvail - 25.f);
			if (ImGui::IconButton(ICON_FA_XMARK))
				ActionsManager::get()->removeAction(actionIndex);
		}
		ImGui::EndDisabled();

		ImGui::PopID();
	}
}
