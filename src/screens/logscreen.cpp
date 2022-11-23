
#include "logscreen.h"

#include "core/actionmanager.h"
#include "fa_icons.h"
#include "utils/utils.h"

namespace coffeepot
{   
    LogScreen::LogScreen()
        : ScreenWithFooter("Log")
        , m_TextBuffer()
        , b_AutoScrollDown(false)
	{
		m_TextBuffer.clear();
	}

    void LogScreen::tickContent()
    {
        ActionsManager::get()->readOutput(m_TextBuffer);
        ImGui::TextUnformatted(m_TextBuffer.c_str());

        if (b_AutoScrollDown)
            ImGui::SetScrollHereY(1.f);
    }

    void LogScreen::tickFooter()
	{
		if (ImGui::IconButton(ICON_FA_ERASER))
			m_TextBuffer.clear();

		ImGui::SameLine(0.f, 2);
		ImGui::Text("|");

		ImGui::SameLine(0.f, 2);
        if (b_AutoScrollDown)
		    ImGui::Checkbox(ICON_FA_ANGLES_DOWN, &b_AutoScrollDown);
		else
			ImGui::Checkbox(ICON_FA_EQUALS, &b_AutoScrollDown);
    }
}
