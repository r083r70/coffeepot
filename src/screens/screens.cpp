
#include "screens.h"

#include "core/serializer.h"
#include <imgui.h>

void coffeepot::MainMenuBarScreen::tick()
{
    if (!ImGui::BeginMainMenuBar())
        return;
    
    ImGui::EndMainMenuBar();
}
