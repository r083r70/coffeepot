#pragma once

#include "core/playlist.h"
#include "screens.h"

namespace coffeepot
{
    class PlaylistScreen : public ScreenWithFooter
    {
    public:
        PlaylistScreen();

    protected:
        virtual void tickContent() override;
        virtual void tickFooter() override;

    private:
        void renderPlaylists();
        void renderPlaylist(Playlist& playlist);
        void renderAction(Action& action);
        void renderOption(Option& option);

        void renderPlaylistBuilder();
        void renderActionSelector();
    
    private:
        bool b_CreatingPlaylist = false;
        Playlist m_PlaylistTemplate;

        Playlist* m_RenamingPlaylist = nullptr;
        std::string m_NewPlaylistName;

        Playlist* m_DeletingPlaylist = nullptr;
    };
}