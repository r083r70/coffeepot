#pragma once

#include "actions/action.h"
#include "actions/playlist.h"
#include "imguiclient.h"
#include "eventdispatcher.h"
#include "screens/screens.h"
#include "window.h"

#include <vector>

namespace coffeepot
{
	class BasePlatform;
    class Event;

    class App : public EventSubscriber
    {
    public:
        App(BasePlatform* platform);

        static App* get() { return s_Instance; }

        // Life cycle
        bool init();
        void run();

        virtual bool onEvent(const Event& event) override;

        // Actions
        inline std::vector<Action>& getAllActions() { return m_Actions; }
        inline void addAction(const Action& action) { m_Actions.push_back(action); }
        
        void saveActions();
        
        // Playlists
        inline std::vector<Playlist>& getAllPlaylists() { return m_Playlists; }
        inline void addPlaylist(const Playlist& playlist) { m_Playlists.push_back(playlist); }

        void savePlaylists();

    private:
        static App* s_Instance;

        BasePlatform* m_Platform;

        EventDispatcher m_EventDispacher;
        Window m_Window;
        ImGuiClient m_ImGuiClient;

        std::vector<Action> m_Actions;
        std::vector<Playlist> m_Playlists;
        std::vector<Screen*> m_Screens;

        const char* m_Title;
        int32_t m_Width;
        int32_t m_Height;

        bool b_ShouldClose;
    };
}