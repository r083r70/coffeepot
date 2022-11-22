
#include "playlistscreen.h"

#include "core/actionmanager.h"
#include "fa_icons.h"
#include "utils/utils.h"

#include "imgui.h"

#include <algorithm>

namespace coffeepot
{
    PlaylistScreen::PlaylistScreen()
        : ScreenWithFooter("Playlists")
        , b_CreatingPlaylist(false)
        , m_PlaylistTemplate()
    {}

    void PlaylistScreen::tickContent()
    {
        if (b_CreatingPlaylist)
            renderPlaylistBuilder();
        else
            renderPlaylists();
    }

    void PlaylistScreen::tickFooter()
    {
        switch (ImGui::BuilderFooter("Playlist", b_CreatingPlaylist))
        {
            case ImGui::BuilderFooterResult::Save:
                ActionsManager::get()->Playlists.push_back(m_PlaylistTemplate);
                break;
            case ImGui::BuilderFooterResult::Start:
                m_PlaylistTemplate = Playlist{};
                break;
        }
    }
    
    void PlaylistScreen::renderPlaylists()
    {
        if (!ImGui::BeginTable("", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
            return;
    
        auto& playlists = ActionsManager::get()->Playlists;
        std::for_each(playlists.begin(), playlists.end(), [this](auto& elem) { renderPlaylist(elem); });

		ImGui::EndTable();

        // Perform Delete at the end of tick
		if (m_DeletingPlaylist != nullptr)
		{
            auto elemToErase = std::remove_if(playlists.begin(), playlists.end(), [this](auto& elem) { return &elem == m_DeletingPlaylist; });
            playlists.erase(elemToErase);
			m_DeletingPlaylist = nullptr;
		}
    }

    void PlaylistScreen::renderPlaylist(Playlist& playlist)
	{
		ImGui::PushID(&playlist);
		const std::string& playlistName = playlist.m_Name;

		ImGui::TableNextRow();
        bool bTreeNode = false;

        // First Row
		{
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			bTreeNode = ImGui::TreeNode("Playlist", playlistName.c_str());

			ImGui::TableSetColumnIndex(1);

            if (m_RenamingPlaylist == &playlist)
            {
				if (ImGui::IconButton(ICON_FA_PEN_TO_SQUARE))
				{
                    playlist.m_Name = m_NewPlaylistName;
                    m_RenamingPlaylist = nullptr;
				}

				ImGui::SameLine();
				ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::InputString("NewName", m_NewPlaylistName);
            }
            else
			{
				if (ImGui::IconButton(ICON_FA_PLAY))
					ActionsManager::get()->executePlaylist(playlist);

				ImGui::SameLine();
				if (ImGui::IconButton(ICON_FA_PEN))
				{
					m_RenamingPlaylist = &playlist;
                    m_NewPlaylistName = playlistName;
				}

				ImGui::SameLine();
                if (ImGui::IconButton(ICON_FA_TRASH))
                    m_DeletingPlaylist = &playlist;
			}
		}

		// Show Actions
		if (bTreeNode)
		{
			auto& actions = playlist.getActions();
			std::for_each(actions.begin(), actions.end(), [this](auto& elem) { renderAction(elem); });

			ImGui::TreePop();
		}

		ImGui::PopID();
    }

	void PlaylistScreen::renderAction(Action& action)
	{
		ImGui::PushID(&action);
		const std::string& actionName = action.m_Name;

		ImGui::TableNextRow();
		bool bTreeNode = false;

		// First Row
        {
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();

			if (action.m_Options.size() != 0)
				bTreeNode = ImGui::TreeNode("Action", actionName.c_str());
			else
				ImGui::BulletText(actionName.c_str());

			ImGui::TableSetColumnIndex(1);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 13);
			if (ImGui::IconButton(ICON_FA_PLAY))
				ActionsManager::get()->executeAction(action);
		}

        // Show Options
		if (bTreeNode)
		{
			auto& options = action.m_Options;
			std::for_each(options.begin(), options.end(), [](auto& elem) { ImGui::OptionRow(elem); });

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	void PlaylistScreen::renderPlaylistBuilder()
    {
        ImGui::PushID(&m_PlaylistTemplate);
        
        ImGui::Text("Name: ");
        ImGui::SameLine();
        ImGui::InputString("ActionName", m_PlaylistTemplate.m_Name);

        const auto& actions = m_PlaylistTemplate.getActions();
        std::for_each(actions.begin(), actions.end(), [](const auto& Elem) { ImGui::BulletText(Elem.m_Name.c_str()); });

        ImGui::Separator();
        renderActionSelector();

        ImGui::PopID();
    }

    void PlaylistScreen::renderActionSelector()
    {
        if (!ImGui::BeginCombo("", "Add Action"))
            return;

        for (const auto& action : ActionsManager::get()->Actions)
        {
            if (ImGui::Selectable(action.m_Name.c_str(), false))
            {
                m_PlaylistTemplate.addAction(action);
            }
        }

        ImGui::EndCombo();
    }
}