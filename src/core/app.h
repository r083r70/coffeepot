#pragma once

#include "actions/action.h"
#include "actions/playlist.h"
#include "imguiclient.h"
#include "eventdispatcher.h"
#include "platform.h"
#include "screens/screens.h"
#include "window.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace coffeepot
{
	class Event;
	using GlobalOptionsMap = std::unordered_map<std::string, std::string>;

    class App : public EventSubscriber
    {
    public:
        App();

        static App* get() { return s_Instance; }

        // LifeCycle
        bool init();
        void run();

        virtual bool onEvent(const Event& event) override;

        // Data
        inline std::vector<Action>& getAllActions() { return m_Actions; }
		inline void addAction(const Action& action) { m_Actions.push_back(action); }

		inline std::vector<Playlist>& getAllPlaylists() { return m_Playlists; }
        inline void addPlaylist(const Playlist& playlist) { m_Playlists.push_back(playlist); }

		inline GlobalOptionsMap& getGlobalOptions() { return m_GlobalOptions; }
        
        void refreshActionsAndPlaylists();
        void saveAll();

    private:
        static App* s_Instance;

        Platform m_Platform;

        EventDispatcher m_EventDispacher;
        Window m_Window;
        ImGuiClient m_ImGuiClient;

        std::vector<Action> m_Actions;
        std::vector<Playlist> m_Playlists;
        GlobalOptionsMap m_GlobalOptions;

        std::vector<Screen*> m_Screens;

        const char* m_Title;
        int32_t m_Width;
        int32_t m_Height;

        bool b_ShouldClose;
    };
}