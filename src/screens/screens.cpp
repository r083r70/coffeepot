
#include "screens.h"

#include "core/app.h"
#include "core/eventdispatcher.h"
#include "core/log.h"

#include "imgui.h"

void coffeepot::MainMenuBarScreen::tick()
{
    if (!ImGui::BeginMainMenuBar())
        return;

    if (ImGui::BeginMenu("Coffeepot"))
	{
        if (ImGui::MenuItem("Exit"))
            EventDispatcher::get()->createEvent(EventType::Terminate);

		ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Actions"))
    {
        if (ImGui::MenuItem("Save"))
            App::get()->saveActions();

        if (ImGui::MenuItem("Reload"))
            CP_DEBUG("Not implemented");

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Playlists"))
    {
        if (ImGui::MenuItem("Save"))
            App::get()->savePlaylists();

        if (ImGui::MenuItem("Reload"))
            CP_DEBUG("Not implemented");

        ImGui::EndMenu();
    }
    
    ImGui::EndMainMenuBar();
}

void coffeepot::ScreenWithFooter::tick()
{
    if (!ImGui::Begin(m_Name, nullptr, ImGuiWindowFlags_NoScrollbar))
        return;

    const float itemSpacing = ImGui::GetStyle().ItemSpacing.y;
    const float footerHeight = ImGui::GetFrameHeight() + itemSpacing;
    if (ImGui::BeginChild("Content", ImVec2(0, -footerHeight)))
    {
        tickContent();
        ImGui::EndChild();
    }

    ImGui::Separator();
    tickFooter();
    
    ImGui::End();
}
