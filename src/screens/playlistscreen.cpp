
#include "playlistscreen.h"

#include "core/actionmanager.h"
#include "fa_icons.h"
#include "utils/imgui_helper.h"

#include "imgui.h"

#include <algorithm>
#include <cmath>

namespace coffeepot
{
    PlaylistScreen::PlaylistScreen()
        : ScreenWithFooter("Playlists")
    {}

    void PlaylistScreen::tickContent()
    {
		renderPlaylists();
    }

    void PlaylistScreen::tickFooter()
	{
		if (ImGui::IconButton(ICON_FA_PLUS))
		{
			auto& playlist = ActionsManager::get()->Playlists.emplace_back();
			playlist.setName("New Playlist");
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
		if (m_DeletingPlaylist != nullptr && b_DeleteConfirmed)
		{
            auto playlistToErase = std::remove_if(playlists.begin(), playlists.end(), [this](auto& elem) { return &elem == m_DeletingPlaylist; });
            playlists.erase(playlistToErase);

			// Reset
			m_DeletingPlaylist = nullptr;
			b_DeleteConfirmed = false;
		}
    }

    void PlaylistScreen::renderPlaylist(Playlist& playlist)
	{
		ImGui::PushID(&playlist);
		const std::string& playlistName = playlist.getName();

        bool bShowActions = false;

		ImGui::TableNextRow();

        // Playlist Name
		{
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			bShowActions = ImGui::TreeNode("Playlist", playlistName.c_str());

			ImGui::TableSetColumnIndex(1);
			ImGui::AlignTextToFramePadding();
            if (m_RenamingPlaylist == &playlist)
            {
				if (ImGui::IconButton(ICON_FA_PEN_TO_SQUARE))
				{
                    playlist.setName(m_NewPlaylistName);
                    m_RenamingPlaylist = nullptr;
				}

				ImGui::SameLine();
				ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::InputString("NewName", m_NewPlaylistName);
            }
			else if (m_DeletingPlaylist == &playlist)
			{
				if (ImGui::IconButton(ICON_FA_CHECK))
					b_DeleteConfirmed = true;

				ImGui::SameLine(0.f, 3);
				if (ImGui::IconButton(ICON_FA_XMARK))
					m_DeletingPlaylist = nullptr;
			}
            else
			{
				// Run
				if (ImGui::IconButton(ICON_FA_PLAY))
					ActionsManager::get()->executePlaylist(playlist);

				ImGui::SameLine(0.f, 3);
				ImGui::Text("|");

				// Add Action
				ImGui::SameLine(0.f, 3);
				if (ImGui::IconButton(ICON_FA_PLUS))
				{
					m_ExpandingPlaylist = &playlist;
					m_ExpansionIndex = -1;
				}

				// Rename
				ImGui::SameLine(0.f, 3);
				if (ImGui::IconButton(ICON_FA_PEN))
				{
					m_RenamingPlaylist = &playlist;
                    m_NewPlaylistName = playlistName;
				}

				// Delete
				ImGui::SameLine(0.f, 3);
                if (ImGui::IconButton(ICON_FA_TRASH_CAN))
				{
					m_DeletingPlaylist = &playlist;
					b_DeleteConfirmed = false;
				}
			}
		}

		// Show Actions
		if (bShowActions)
		{
			auto& actions = playlist.getActions();

			int32_t swapIndex1 = -1, swapIndex2 = -1, removeIndex = -1;
			const auto DrawIconButtonEx = [](const char* label, bool bEnabled) -> bool
			{
				ImGui::BeginDisabled(!bEnabled);
				const bool bResult = ImGui::IconButton(label);
				ImGui::EndDisabled();
				return bResult;
			};

			for (int32_t i = 0; i < actions.size(); i++)
			{
				// Add Action at Index
				if (m_ExpandingPlaylist == &playlist && m_ExpansionIndex == i)
					renderPlaylistExpansion();

				Action& action = actions[i];
				const bool bIsFirst = i == 0;
				const bool bIsLast = i == actions.size() - 1;

				ImGui::PushID(&action);
				const std::string& actionName = action.m_Name;

				ImGui::TableNextRow();
				bool bShowOptions = false;

				// Action
				{
					// First Column
					ImGui::TableSetColumnIndex(0);
					ImGui::AlignTextToFramePadding();

					if (action.m_Options.size() != 0)
						bShowOptions = ImGui::TreeNode("Action", actionName.c_str());
					else
						ImGui::BulletText(actionName.c_str());

					// Second Column
					ImGui::TableSetColumnIndex(1);
					ImGui::AlignTextToFramePadding();

					if (m_RenamingAction == &action)
					{
						if (ImGui::IconButton(ICON_FA_PEN_TO_SQUARE))
						{
							action.m_Name = m_NewActionName;
							m_RenamingAction = nullptr;
						}

						ImGui::SameLine();
						ImGui::SetNextItemWidth(-FLT_MIN);
						ImGui::InputString("NewName", m_NewActionName);
					}
					else
					{
						// Execute
						if (ImGui::IconButton(ICON_FA_CIRCLE_PLAY))
						{
							ActionsManager::get()->executeAction(action);
						}

						ImGui::SameLine(0.f, 3);
						ImGui::Text("|");

						// Add
						ImGui::SameLine(0.f, 3);
						if (ImGui::IconButton(ICON_FA_CIRCLE_PLUS))
						{
							m_ExpandingPlaylist = &playlist;
							m_ExpansionIndex = i;
						}

						// Move Down
						ImGui::SameLine(0.f, 3);
						if (DrawIconButtonEx(ICON_FA_ARROW_DOWN, !bIsLast))
						{
							swapIndex1 = i;
							swapIndex2 = i + 1;
						}

						// Move Up
						ImGui::SameLine(0.f, 3);
						if (DrawIconButtonEx(ICON_FA_ARROW_UP, !bIsFirst))
						{
							swapIndex1 = i;
							swapIndex2 = i - 1;
						}

						// Rename
						ImGui::SameLine(0.f, 3);
						if (ImGui::IconButton(ICON_FA_PEN))
						{
							m_RenamingAction = &action;
							m_NewActionName = action.m_Name;
						}

						// Remove
						ImGui::SameLine(0.f, 3);
						if (ImGui::IconButton(ICON_FA_XMARK))
							removeIndex = i;
					}
				}

				// Options
				if (bShowOptions)
				{
					auto& options = action.m_Options;
					std::for_each(options.begin(), options.end(), [](auto& elem) { ImGui::OptionRow(elem); });

					ImGui::TreePop(); // ShowOptions|End
				}

				ImGui::PopID();
			}

			// Add Action at Last
			if (m_ExpandingPlaylist == &playlist && m_ExpansionIndex == actions.size())
				renderPlaylistExpansion();

			// Swap if needed
			if (swapIndex1 != -1 && swapIndex2 != -1)
			{
				const auto tmp = actions[swapIndex1];
				actions[swapIndex1] = actions[swapIndex2];
				actions[swapIndex2] = tmp;
			}

			// Remove if needed
			if (removeIndex != -1)
			{
				actions.erase(actions.begin() + removeIndex);

				if (m_ExpansionIndex > removeIndex)
					m_ExpansionIndex--;
			}

			ImGui::TreePop(); // ShowActions|End
		}

		// Add Action (no index)
		if (m_ExpandingPlaylist == &playlist && m_ExpansionIndex == -1)
			renderPlaylistExpansion();

		ImGui::PopID();
    }

	void PlaylistScreen::renderPlaylistExpansion()
	{
		assert(m_ExpandingPlaylist != nullptr);
		Playlist& playlist = *m_ExpandingPlaylist;

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::SetNextItemWidth(-FLT_MIN);

		auto& allActions = ActionsManager::get()->Actions;
		auto& playlistActions = playlist.getActions();
		Action action;

		if (ImGui::ComboBoxActions("SelectAction", action, allActions))
		{
			if (m_ExpansionIndex != -1)
				playlistActions.insert(playlistActions.begin() + m_ExpansionIndex, action);
			else
				playlistActions.push_back(action);

			m_ExpandingPlaylist = nullptr;
			m_ExpansionIndex = -1;
		}

		ImGui::TableSetColumnIndex(1);
		ImGui::AlignTextToFramePadding();

		if (m_ExpansionIndex != -1)
		{
			const bool bIsFirst = m_ExpansionIndex == 0;
			const bool bIsLast = m_ExpansionIndex == playlistActions.size();
			const auto DrawIconButtonEx = [](const char* label, bool bEnabled) -> bool
			{
				ImGui::BeginDisabled(!bEnabled);
				const bool bResult = ImGui::IconButton(label);
				ImGui::EndDisabled();
				return bResult;
			};

			// Move Down
			if (DrawIconButtonEx(ICON_FA_ARROW_DOWN, !bIsLast))
				m_ExpansionIndex++;

			// Move Up
			ImGui::SameLine(0.f, 3);
			if (DrawIconButtonEx(ICON_FA_ARROW_UP, !bIsFirst))
				m_ExpansionIndex--;

			ImGui::SameLine(0.f, 3);
		}

		if (ImGui::IconButton(ICON_FA_XMARK))
		{
			m_ExpandingPlaylist = nullptr;
			m_ExpansionIndex = -1;
		}
	}
}