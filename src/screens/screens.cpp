
#include "screens.h"

#include "core/actionmanager.h"
#include "core/app.h"

#include "imgui.h"

void coffeepot::MainMenuBarScreen::tick()
{
    if (!ImGui::BeginMainMenuBar())
        return;

    if (ImGui::BeginMenu("Coffeepot"))
	{
        if (ImGui::MenuItem("Refresh"))
            ActionsManager::get()->reloadAll();

        if (ImGui::MenuItem("SaveAll"))
            ActionsManager::get()->saveAll();

		if (ImGui::MenuItem("Exit"))
			kettle::App::get()->terminate();

		ImGui::EndMenu();
    }
    
    ImGui::EndMainMenuBar();
}

void coffeepot::ScreenWithFooter::tick()
{
    if (!ImGui::Begin(m_Name, nullptr, ImGuiWindowFlags_NoScrollbar))
        return;

	const float itemSpacing = ImGui::GetStyle().ItemSpacing.y;
    const float footerHeight = ImGui::GetFrameHeight() + itemSpacing * 2;
    if (ImGui::BeginChild("Content", ImVec2(0, -footerHeight)))
    {
        tickContent();
        ImGui::EndChild();
    }

    ImGui::Separator();
    tickFooter();
    
    ImGui::End();
}
