
#include "screens.h"

#include <imgui.h>

void coffeepot::MainMenuBarScreen::tick()
{
    if (!ImGui::BeginMainMenuBar())
        return;

    // #todo MainMenuBar
    ImGui::EndMainMenuBar();
}
