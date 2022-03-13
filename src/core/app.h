#pragma once

#include "actions/actionmanager.h"
#include "imguiclient.h"
#include "screens/screens.h"
#include "window.h"

#include <vector>

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

        std::vector<Screen*> m_Screens;

        const char* m_Title;
        int32_t m_Width;
        int32_t m_Height;

        bool b_ShouldClose;
    };
}