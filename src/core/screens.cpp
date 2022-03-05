
#include "screens.h"

#include "imgui.h"
#include "log.h"

void coffeepot::MainMenuBarScreen::tick()
{
    if (ImGui::BeginMainMenuBar())
    {
        ImGui::EndMainMenuBar();
    }
}

coffeepot::CmdListScreen::CmdListScreen()
    : m_Actions()
{
    m_Actions.push_back(new CmdAction{"git status"});
    m_Actions.push_back(new CmdAction("git --version"));
    m_Actions.push_back(new CmdAction("ls -lX"));
}

void coffeepot::CmdListScreen::tick()
{
    if (ImGui::Begin("Commands"))
    {
        for (auto it = m_Actions.begin(); it != m_Actions.end(); it++)
        {
            Action* const action = *it;
            if (ImGui::Button(action->getName().c_str()) && !action->isRunning())
            {
                const bool bIsActionStarted = action->start();
                CP_DEBUG("bIsActionStarted = {0}", bIsActionStarted);
            }
        }

        ImGui::End();
    }

    // Update actions -- someone else should do it
    for (auto it = m_Actions.begin(); it != m_Actions.end(); it++)
    {
        Action* const action = *it;
        if (action->isRunning())
            action->update();
    }
}

    