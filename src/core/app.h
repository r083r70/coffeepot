#pragma once

#include "imguiclient.h"
#include "screens.h"
#include "window.h"

namespace coffeepot
{
    class Event;

    class App
    {
    public:
        App();
        bool init();
        void run();
        void onEvent(Event& event);

    private:
        Window m_Window;
        ImGuiClient m_ImGuiClient;

        MainMenuBarScreen MainMenuBar;
        CmdListScreen CmdList;

        const char* m_Title;
        int32_t m_Width;
        int32_t m_Height;

        bool b_ShouldClose;
    };
}