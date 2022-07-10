#pragma once

#include "screens.h"
#include "imgui.h"

namespace coffeepot
{
    class LogScreen : public Screen
    {
    public:
        virtual void tick() override;

    private:
        void renderLogger();
        void renderFooter();

    private:
        ImGuiTextBuffer m_TextBuffer;
        bool b_AutoScrollDown;
    };
}