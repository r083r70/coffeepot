
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
    {}

    void PlaylistScreen::tickContent()
    {
		renderPlaylists();
    }

    void PlaylistScreen::tickFooter()
	{
		if (ImGui::Button("Create new Playlist"))
			ActionsManager::get()->Playlists.emplace_back();
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

        bool bPlaylistTreeNode = false;

		ImGui::TableNextRow();

        // Playlist Name
		{
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			bPlaylistTreeNode = ImGui::TreeNode("Playlist", playlistName.c_str());

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
				// Run
				if (ImGui::IconButton(ICON_FA_PLAY))
					ActionsManager::get()->executePlaylist(playlist);

				// Add Action
				ImGui::SameLine(0.f, 3);
				if (ImGui::IconButton(ICON_FA_CIRCLE_PLUS))
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
                if (ImGui::IconButton(ICON_FA_TRASH))
                    m_DeletingPlaylist = &playlist;
			}
		}

		// Actions
		if (bPlaylistTreeNode)
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
				Action& action = actions[i];
				const bool bIsFirst = i == 0;
				const bool bIsLast = i == actions.size() - 1;

				ImGui::PushID(&action);
				const std::string& actionName = action.m_Name;

				ImGui::TableNextRow();
				bool bActionTreeNode = false;

				// Action Name
				{
					// First Column
					ImGui::TableSetColumnIndex(0);
					ImGui::AlignTextToFramePadding();

					if (action.m_Options.size() != 0)
						bActionTreeNode = ImGui::TreeNode("Action", actionName.c_str());
					else
						ImGui::BulletText(actionName.c_str());

					// Second Column
					ImGui::TableSetColumnIndex(1);

					// Add
					if (ImGui::IconButton(ICON_FA_SQUARE_PLUS))
					{
						m_ExpandingPlaylist = &playlist;
						m_ExpansionIndex = i + 1;
					}

					// Move Up
					ImGui::SameLine(0.f, 3);
					if (DrawIconButtonEx(ICON_FA_ARROW_UP, !bIsFirst))
					{
						swapIndex1 = i;
						swapIndex2 = i - 1;
					}

					// Move Down
					ImGui::SameLine(0.f, 3);
					if (DrawIconButtonEx(ICON_FA_ARROW_DOWN, !bIsLast))
					{
						swapIndex1 = i;
						swapIndex2 = i + 1;
					}

					// Remove
					ImGui::SameLine(0.f, 3);
					if (ImGui::IconButton(ICON_FA_XMARK))
						removeIndex = i;

					// Execute
					ImGui::SameLine(0.f, 3);
					if (ImGui::IconButton(ICON_FA_PLAY))
					{
						ActionsManager::get()->executeAction(action);
					}
				}

				// Show Options
				if (bActionTreeNode)
				{
					auto& options = action.m_Options;
					std::for_each(options.begin(), options.end(), [](auto& elem) { ImGui::OptionRow(elem); });

					ImGui::TreePop(); // ActionTree
				}

				ImGui::PopID();
			}

			ImGui::TreePop(); // PlaylistTree

			// Swap if needed
			if (swapIndex1 != -1 && swapIndex2 != -1)
			{
				const auto tmp = actions[swapIndex1];
				actions[swapIndex1] = actions[swapIndex2];
				actions[swapIndex2] = tmp;
			}

			// Remove if needed
			if (removeIndex != -1)
				actions.erase(actions.begin() + removeIndex);
		}

		// Add Action
		if (m_ExpandingPlaylist == &playlist)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Choose Action:");

			ImGui::TableSetColumnIndex(1);

			if (ImGui::IconButton(ICON_FA_DELETE_LEFT))
			{
				m_ExpandingPlaylist = nullptr;
				m_ExpansionIndex = -1;
			}

			Action action;
			ImGui::SameLine();
			if (renderActionSelector(action))
			{
				auto& actions = playlist.getActions();
				if (m_ExpansionIndex != -1)
					actions.insert(actions.begin() + m_ExpansionIndex, action);
				else
					actions.push_back(action);

				m_ExpandingPlaylist = nullptr;
				m_ExpansionIndex = -1;
			}
		}

		ImGui::PopID();
    }

    bool PlaylistScreen::renderActionSelector(Action& outAction)
	{
		if (!ImGui::BeginCombo("", "Select.."))
			return false;

		bool bSelected = false;
        for (const auto& action : ActionsManager::get()->Actions)
        {
            if (ImGui::Selectable(action.m_Name.c_str(), false))
            {
				outAction = action;
				bSelected = true;
            }
        }

        ImGui::EndCombo();
		return bSelected;
    }
}