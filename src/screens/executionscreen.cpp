
#include "executionscreen.h"

#include "core/actionmanager.h"
#include "fa_icons.h"
#include "utils/imgui_helper.h"

#include "imgui.h"
#include <cassert>

namespace coffeepot
{
	void ExecutionScreen::tickContent()
	{
		const bool bIsExecuting = ActionsManager::get()->isExecuting();

		const ExecutionPlaylist& playlist = ActionsManager::get()->getExecutionPlaylist();
		const int32_t lastCompletedActionIndex = playlist.getLastCompletedActionIndex();
		const int32_t activeActionIndex = playlist.getActiveActionIndex();

		const auto& actions = playlist.getActions();

		for (int32_t index = 0; index < actions.size(); index++)
		{
			ActionState actionState = ActionState::None;

			if (index <= lastCompletedActionIndex)
				actionState = ActionState::Done;
			else if (index == activeActionIndex)
				actionState = ActionState::Running;
			else if (bIsExecuting)
				actionState = ActionState::Waiting;
			else
				actionState = ActionState::Skipped;

			listAction(actions[index], index, actionState, bIsExecuting);
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
		case ActionState::Done:			ImGui::Text(ICON_FA_CIRCLE_CHECK); break;
		case ActionState::Running:		ImGui::Text(ICON_FA_BOLT); break;
		case ActionState::Waiting:		ImGui::Text(ICON_FA_HOURGLASS_EMPTY); break;
		case ActionState::Skipped:		ImGui::Text(ICON_FA_BAN); break;
		default:						assert(false);
		}

		ImGui::SameLine();
		ImGui::Text(action.m_Name.c_str());

		const bool bCanRemoveAction = actionState == ActionState::Waiting || !bIsExecutionActive;
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
