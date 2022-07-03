#pragma once

#include "actions/action.h"
#include "actions/playlist.h"
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

        static App* get() { return s_Instance; }

        bool init();
        void run();
        void onEvent(Event& event);

        // Actions
        inline std::vector<Action>& getAllActions() { return m_Actions; }
        inline void addAction(const Action& action) { m_Actions.push_back(action); }
        
        // Playlists
        inline std::vector<Playlist>& getAllPlaylists() { return m_Playlists; }
        inline void addPlaylist(const Playlist& playlist) { m_Playlists.push_back(playlist); }

    private:
        static App* s_Instance;

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