
#include "playlistscreen.h"

#include "core/actionmanager.h"
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
    }

    void PlaylistScreen::renderPlaylist(Playlist& playlist)
    {
        if (ImGui::PlaylistTree(playlist, /*bCanRun =*/ true))
        {
            const bool bResult = ActionsManager::get()->executePlaylist(playlist);
        }
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