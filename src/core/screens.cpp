
#include "screens.h"

#include "actions.h"
#include "imgui.h"
#include "log.h"

void coffeepot::MainMenuBarScreen::tick()
{
    if (!ImGui::BeginMainMenuBar())
        return;

    // #todo MainMenuBar
    ImGui::EndMainMenuBar();
}

void coffeepot::CmdListScreen::tick()
{
    if (!ImGui::Begin("Commands"))
        return;
    
    const auto& actions = ActionsManager::get()->getAllActions();
    for (auto it = actions.begin(); it != actions.end(); it++)
    {
        Action* const action = *it;
        if (action == nullptr)
            continue;

        auto actionName = action->getName();
        if (ImGui::Button(actionName.c_str()) && !action->isRunning())
        {
            const bool bIsActionStarted = action->start();
            CP_DEBUG("[{}] Start => {}", actionName, bIsActionStarted);
        }
    }

    ImGui::End();
}
