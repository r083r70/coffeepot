#pragma once

#include "actions/playlist.h"
#include "screens.h"

namespace coffeepot
{
    class PlaylistScreen : public Screen
    {
    public:
        virtual void tick() override;

    private:
        void renderPlaylists();
        void renderPlaylist(Playlist& playlist);
        void renderAction(Action& action);
        void renderOption(Option& option);

        void renderPlaylistBuilder();
        void renderActionSelector();

        void renderFooter();
    
    private:
        bool b_CreatingPlaylist = false;
        Playlist m_PlaylistTemplate;
    };
}