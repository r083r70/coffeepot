
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
		const bool bIsExecutionPlaylistValid = ActionsManager::get()->isExecutionPlaylistValid();
		const bool bIsExecuting = ActionsManager::get()->isExecuting();

		const Playlist& playlist = ActionsManager::get()->getExecutionPlaylist();
		const size_t currentActionIndex = playlist.getCurrentActionIndex();
		const auto& actions = playlist.getActions();

		for (size_t index = 0; index < actions.size(); index++)
		{
			ActionState actionState = ActionState::None;
			if (index < currentActionIndex || !bIsExecutionPlaylistValid)
				actionState = ActionState::Done;
			else if (index == currentActionIndex && bIsExecuting)
				actionState = ActionState::Running;
			else
				actionState = ActionState::Waiting;

			listAction(actions[index], index, actionState);
		}
    }

	void ExecutionScreen::tickFooter()
	{
		ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x * 0.5f - 25);
		if (ImGui::IconButton(ICON_FA_SKULL))
			ActionsManager::get()->killExecution();

		ImGui::SameLine(0.f, 3);
		const bool bIsExecutionPlaylistValid = ActionsManager::get()->isExecutionPlaylistValid();
		ImGui::BeginDisabled(bIsExecutionPlaylistValid);

		ImGui::SameLine(0.f, 3);
		if (ImGui::IconButton(ICON_FA_TRASH_CAN))
		{
			//ActionsManager::get()->clearExecution();
		}

		ImGui::SameLine(0.f, 3);
		if (ImGui::IconButton(ICON_FA_ROTATE_RIGHT))
		{
			// Reset execution Playlist to make it valid and start
			Playlist playlist = ActionsManager::get()->getExecutionPlaylist();
			ActionsManager::get()->executePlaylist(playlist);
		}
		
		ImGui::EndDisabled();
	}

	void ExecutionScreen::listAction(const Action& action, size_t actionIndex, ActionState actionState)
	{
		ImGui::PushID(&action);

		const float ContentRegionAvail = ImGui::GetContentRegionAvail().x;

		switch (actionState)
		{
		case ActionState::Done:			ImGui::Text(ICON_FA_CIRCLE_CHECK); break;
		case ActionState::Running:		ImGui::Text(ICON_FA_BOLT); break;
		case ActionState::Waiting:		ImGui::Text(ICON_FA_HOURGLASS_EMPTY); break;
		default:						assert(false);
		}

		ImGui::SameLine();
		ImGui::Text(action.m_Name.c_str());

		ImGui::BeginDisabled(actionState != ActionState::Waiting);
		{
			ImGui::SameLine(ContentRegionAvail - 25.f);
			if (ImGui::IconButton(ICON_FA_XMARK))
				ActionsManager::get()->removeAction(actionIndex);
		}
		ImGui::EndDisabled();

		ImGui::PopID();
	}
}
