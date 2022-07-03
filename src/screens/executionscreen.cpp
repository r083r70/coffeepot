
#include "executionscreen.h"

#include "actions/actionmanager.h"
#include "imgui.h"

namespace coffeepot
{
	void ExecutionScreen::tick()
	{
		if (!ImGui::Begin("Execution"))
			return;

		const Playlist& playlist = ActionsManager::get()->getCurrentPlaylist();
		size_t nextActionIndex = playlist.getNextActionIndex();

		auto& actions = playlist.getActions();
		for (size_t i = 0; i < actions.size(); i++)
		{
			const bool bExecuted = i < nextActionIndex;
			listAction(actions[i], bExecuted);
		}

		ImGui::End();
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
