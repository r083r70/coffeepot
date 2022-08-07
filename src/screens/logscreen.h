#pragma once

#include "screens.h"
#include "imgui.h"

namespace coffeepot
{
    class LogScreen : public ScreenWithFooter
    {
    public:
        LogScreen();

    protected:
        virtual void tickContent() override;
        virtual void tickFooter() override;

    private:
        ImGuiTextBuffer m_TextBuffer;
        bool b_AutoScrollDown;
    };
}