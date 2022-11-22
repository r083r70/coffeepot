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

        bool renderActionSelector(Action& outAction);
    
    private:
        // Rename
        Playlist* m_RenamingPlaylist = nullptr;
        std::string m_NewPlaylistName;

		// Delete
        Playlist* m_DeletingPlaylist = nullptr;

		// Expanding
		Playlist* m_ExpandingPlaylist = nullptr;
		int32_t m_ExpansionIndex = -1;
    };
}