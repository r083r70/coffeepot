
#include "app.h"

#include "events.h"
#include "log.h"
#include "screens/actionscreen.h"
#include "screens/logscreen.h"
#include "screens/executionscreen.h"
#include "serializer.h"

namespace coffeepot
{
    App::App()
		: m_Window()
        , m_ImGuiClient()
        , m_Title("coffeepot")
        , m_Width(720)
        , m_Height(480)
        , b_ShouldClose(false)
    {}

    bool App::init()
    {
        Log::init();
        Serializer::loadWindowSize(m_Width, m_Height);

        m_Window.setEventCallback(std::bind(&App::onEvent, this, std::placeholders::_1));
        if (!m_Window.init(m_Title, m_Width, m_Height))
            return false;
        
        if (!m_ImGuiClient.init(m_Window))
            return false;

        m_Screens.push_back(new MainMenuBarScreen());
		m_Screens.push_back(new LogScreen());
		m_Screens.push_back(new ActionsScreen());
		m_Screens.push_back(new ExecutionScreen());
        
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

		Serializer::saveWindowSize(m_Width, m_Height);

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
            const auto& windowResizedEvent = static_cast<coffeepot::WindowResizedEvent&>(event);
            m_Width = windowResizedEvent.m_Width;
            m_Height = windowResizedEvent.m_Height;
		} break;
        }
    }
}
