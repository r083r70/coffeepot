#pragma once

#include "imguiclient.h"
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

        const char* m_Title;
        int32_t m_Width;
        int32_t m_Height;

        bool b_ShouldClose;
        bool b_ShowDemo;
    };
}