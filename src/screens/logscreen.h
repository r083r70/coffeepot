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
        ImGuiTextBuffer m_Buffer;
    };
}