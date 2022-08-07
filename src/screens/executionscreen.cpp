
#include "executionscreen.h"

#include "actions/actionmanager.h"
#include "imgui.h"

namespace coffeepot
{
	void ExecutionScreen::tickContent()
	{
		const Playlist& playlist = ActionsManager::get()->getExecutionPlaylist();
		size_t nextActionIndex = playlist.getNextActionIndex();

		auto& actions = playlist.getActions();
		for (size_t i = 0; i < actions.size(); i++)
		{
			const bool bExecuted = i < nextActionIndex;
			listAction(actions[i], bExecuted);
		}
    }

    void ExecutionScreen::tickFooter()
    {
        if (ImGui::Button("Kill All"))
            ActionsManager::get()->killExecution();
	}

	void ExecutionScreen::listAction(const Action& action, bool bExecuted)
	{
		ImGui::PushID("bExecuted");
		ImGui::Checkbox("", &bExecuted);
		ImGui::SameLine();
		ImGui::Text(action.m_Name.c_str());
		ImGui::PopID();
	}
}
