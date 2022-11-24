
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
		const Playlist& playlist = ActionsManager::get()->getExecutionPlaylist();
		const auto& actions = playlist.getActions();

		const bool isRunningAction = ActionsManager::get()->isExecutingAction();
		const size_t currentActionIndex = playlist.getNextActionIndex() - (isRunningAction ? 1 : 0);

		for (size_t index = 0; index < actions.size(); index++)
		{
			ActionState actionState = ActionState::None;
			if (index < currentActionIndex)
				actionState = ActionState::Done;
			else if (index == currentActionIndex)
				actionState = ActionState::Running;
			else
				actionState = ActionState::Waiting;

			listAction(actions[index], actionState);
		}
    }

    void ExecutionScreen::tickFooter()
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() * 0.5f - 10);
        if (ImGui::IconButton(ICON_FA_SKULL))
            ActionsManager::get()->killExecution();
	}

	void ExecutionScreen::listAction(const Action& action, ActionState actionState)
	{
		ImGui::PushID("ActionState");

		assert(actionState != ActionState::None);
		switch (actionState)
		{
		case ActionState::Done:			ImGui::Text(ICON_FA_CIRCLE_CHECK); break;
		case ActionState::Running:		ImGui::Text(ICON_FA_BOLT); break;
		case ActionState::Waiting:		ImGui::Text(ICON_FA_HOURGLASS_EMPTY); break;
		}

		ImGui::SameLine();
		ImGui::Text(action.m_Name.c_str());

		ImGui::PopID();
	}
}
