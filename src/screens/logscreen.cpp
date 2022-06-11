
#include "logscreen.h"

#include "actions/actionmanager.h"
#include "core/log.h"

namespace coffeepot
{
    void LogScreen::tick()
    {
        if (!ImGui::Begin("Log"))
            return;

        if (const char* output = ActionsManager::get()->readOutput())
            m_Buffer.append(output);

        ImGui::TextUnformatted(m_Buffer.c_str());
        ImGui::SetScrollHereY(1.f);

        ImGui::End();
    }
}
