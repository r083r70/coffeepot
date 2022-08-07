
#include "logscreen.h"

#include "actions/actionmanager.h"
#include "core/log.h"

namespace coffeepot
{   
    LogScreen::LogScreen()
        : ScreenWithFooter("Log")
        , m_TextBuffer()
        , b_AutoScrollDown(false)
    {}

    void LogScreen::tickContent()
    {
        ActionsManager::get()->readOutput(m_TextBuffer);
        ImGui::TextUnformatted(m_TextBuffer.c_str());

        if (b_AutoScrollDown)
            ImGui::SetScrollHereY(1.f);
    }

    void LogScreen::tickFooter()
    {
        if (ImGui::Button("Clear"))
            m_TextBuffer.clear();

        ImGui::SameLine();
        ImGui::Checkbox("AutoScroll", &b_AutoScrollDown);
    }
}
