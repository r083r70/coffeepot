
#include "logscreen.h"

#include "actions/actionmanager.h"
#include "core/log.h"

namespace coffeepot
{
    void LogScreen::tick()
    {
        if (!ImGui::Begin("Log"))
            return;

        ActionsManager::get()->readOutput(m_TextBuffer);
        ImGui::TextUnformatted(m_TextBuffer.c_str());
        ImGui::SetScrollHereY(1.f);

        ImGui::End();
    }
}
