
#include "app.h"

#include "actions/actionmanager.h"
#include "events.h"
#include "log.h"
#include "platform.h"
#include "screens/actionscreen.h"
#include "screens/executionscreen.h"
#include "screens/logscreen.h"
#include "screens/playlistscreen.h"
#include "serializer.h"

#include "GLFW/glfw3.h" // TEMP

namespace coffeepot
{
	App* App::s_Instance = nullptr;

	App::App()
		: m_Platform()
        , m_Window()
        , m_ImGuiClient()
        , m_Title("coffeepot")
        , m_Width(720)
        , m_Height(480)
        , b_ShouldClose(false)
    { }

	bool App::init()
    {
		s_Instance = this;

		Log::init();

		m_EventDispacher.init();
		m_EventDispacher.subscribe(this);
        
		m_Platform.init();

		Serializer::loadWindowSize(m_Width, m_Height);
        if (!m_Window.init(m_Title, m_Width, m_Height))
            return false;
        
        if (!m_ImGuiClient.init(m_Window))
            return false;

        m_Screens.push_back(new MainMenuBarScreen());
		m_Screens.push_back(new ActionsScreen());
		m_Screens.push_back(new ExecutionScreen());
		m_Screens.push_back(new LogScreen());
		m_Screens.push_back(new PlaylistScreen());

        Serializer::loadActionsAndPlaylists(m_Actions, m_Playlists);
        ActionsManager::get()->init();

        return true;
    }
    
    void App::run()
    {
        while (!b_ShouldClose)
		{
			m_EventDispacher.tick();

            m_Platform.tick();

			m_Window.tick();
            if (m_Window.IsVisible())
			{
				m_ImGuiClient.preTick(m_Width, m_Height);
				m_ImGuiClient.tick();

				std::for_each(m_Screens.begin(), m_Screens.end(), [](auto Elem) { Elem->tick(); });

				m_ImGuiClient.postTick();
			}

            ActionsManager::get()->tick();
		}

		Serializer::saveWindowSize(m_Width, m_Height);

        ActionsManager::get()->deinit();
        m_Actions.clear();
        m_Playlists.clear();
        
        std::for_each(m_Screens.begin(), m_Screens.end(), [](auto& Elem) { delete Elem; Elem = nullptr; });
        m_Screens.clear();
        
        m_ImGuiClient.deinit();
        m_Window.deinit();

		m_Platform.deinit();
		m_EventDispacher.deinit();
    }
    
    bool App::onEvent(const Event& event)
    {
        switch (event.getType())
		{
		case coffeepot::EventType::Terminate:
        case coffeepot::EventType::WindowClosed:
            b_ShouldClose = true;
            return true;
        case coffeepot::EventType::WindowResized:
            m_Width = event.getData<WindowResizedEventData>()->m_Width;
            m_Height = event.getData<WindowResizedEventData>()->m_Height;
			return true;
        }

		return false;
    }

	void App::refreshActionsAndPlaylists()
	{
		m_Actions.clear();
        m_Playlists.clear();
		Serializer::loadActionsAndPlaylists(m_Actions, m_Playlists);
	}
}
