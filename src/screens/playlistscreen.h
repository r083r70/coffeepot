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

        void renderPlaylistExpansion();
    
    private:
        // Rename
        Playlist* m_RenamingPlaylist = nullptr;
        std::string m_NewPlaylistName;

        Action* m_RenamingAction = nullptr;
		std::string m_NewActionName;

		// Delete
        Playlist* m_DeletingPlaylist = nullptr;
        bool b_DeleteConfirmed = false;

		// Expanding
		Playlist* m_ExpandingPlaylist = nullptr;
		int32_t m_ExpansionIndex = -1;
    };
}