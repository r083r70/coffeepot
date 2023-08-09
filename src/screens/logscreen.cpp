
#include "logscreen.h"

#include "core/actionmanager.h"
#include "fa_icons.h"
#include "utils/imgui_helper.h"

#include <algorithm>

namespace coffeepot
{   
    LogScreen::LogScreen()
        : ScreenWithFooter("Log")
        , m_TextBuffer()
        , b_AutoScrollDown(false)
	{
		m_TextBuffer.clear();
	}

	void LogScreen::tick()
	{
		ScreenWithFooter::tick();
		ActionsManager::get()->moveBuffer(m_TextBuffer); // Keep updating the Buffer to free it
	}

	void LogScreen::tickContent()
	{
        ImGui::TextUnformatted(m_TextBuffer.data());
        if (b_AutoScrollDown)
            ImGui::SetScrollHereY(1.f);
    }

    void LogScreen::tickFooter()
	{
        if (ImGui::IconButton(ICON_FA_ERASER))
		{
			std::fill(m_TextBuffer.begin(), m_TextBuffer.end(), 0);
            m_TextBuffer.clear(); // Doesnt resize
		}

		ImGui::SameLine(0.f, 3);
		ImGui::Text("|");

		ImGui::SameLine(0.f, 3);
        if (b_AutoScrollDown)
		    ImGui::Checkbox(ICON_FA_ANGLES_DOWN, &b_AutoScrollDown);
		else
			ImGui::Checkbox(ICON_FA_EQUALS, &b_AutoScrollDown);
    }
}
