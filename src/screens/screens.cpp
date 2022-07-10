
#include "screens.h"

#include "core/app.h"
#include "core/log.h"

#include "imgui.h"

void coffeepot::MainMenuBarScreen::tick()
{
    if (!ImGui::BeginMainMenuBar())
        return;

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
