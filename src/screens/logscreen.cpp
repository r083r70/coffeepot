
#include "logscreen.h"

#include "actions/actionmanager.h"
#include "core/log.h"

namespace coffeepot
{
    void LogScreen::tick()
    {
        if (!ImGui::Begin("Log"))
            return;

        const float itemSpacing = ImGui::GetStyle().ItemSpacing.y;
        const float footerHeight = ImGui::GetFrameHeight() + itemSpacing;
        if (ImGui::BeginChild("Actions", ImVec2(0, -footerHeight)))
        {
            renderLogger();
            ImGui::EndChild();
        }

        renderFooter();
        ImGui::End();
    }

    void LogScreen::renderLogger()
    {
        ActionsManager::get()->readOutput(m_TextBuffer);
        ImGui::TextUnformatted(m_TextBuffer.c_str());

        if (b_AutoScrollDown)
            ImGui::SetScrollHereY(1.f);
    }

    void LogScreen::renderFooter()
    {
        ImGui::Separator();

        if (ImGui::Button("Clear"))
            m_TextBuffer.clear();

        ImGui::SameLine();
        ImGui::Checkbox("AutoScroll", &b_AutoScrollDown);
    }
}
