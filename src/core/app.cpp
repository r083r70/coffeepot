
#include "app.h"

#include "screens/actionscreen.h"
#include "events.h"
#include "log.h"

namespace coffeepot
{
    App::App()
        : m_Title("Coffeepot")
        , m_Width(640)
        , m_Height(360)
        , b_ShouldClose(false)
    {}

    bool App::init()
    {
        log::init();

        m_Window.setEventCallback(std::bind(&App::onEvent, this, std::placeholders::_1));
        if (!m_Window.init(m_Title, m_Width, m_Height))
            return false;
        
        if (!m_ImGuiClient.init(m_Window))
            return false;

        m_Screens.push_back(new MainMenuBarScreen());
        m_Screens.push_back(new ActionsScreen());
        
        ActionsManager::get()->init();

        return true;
    }
    
    void App::run()
    {
        while (!b_ShouldClose)
        {
            m_Window.tick();

            m_ImGuiClient.preTick(m_Width, m_Height);
            m_ImGuiClient.tick();

            std::for_each(m_Screens.begin(), m_Screens.end(), [](auto Elem) { Elem->tick(); });
            
            m_ImGuiClient.postTick();

            ActionsManager::get()->tick();
        }

        ActionsManager::get()->deinit();
        
        std::for_each(m_Screens.begin(), m_Screens.end(), [](auto& Elem) { delete Elem; Elem = nullptr; });
        m_Screens.clear();
        
        m_ImGuiClient.deinit();
        m_Window.deinit();
    }
    
    void App::onEvent(Event& event)
    {
        switch (event.getType())
        {
        case coffeepot::EventType::WindowClosed:
            b_ShouldClose = true;
            break;
        case coffeepot::EventType::WindowResized: {
            auto windowResizedEvent = static_cast<coffeepot::WindowResizedEvent&>(event);
            m_Width = windowResizedEvent.m_Width;
            m_Height = windowResizedEvent.m_Height;
        } break;
        case coffeepot::EventType::Char:
            CP_DEBUG("Char");
            break;
        }
    }
}