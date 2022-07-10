#pragma once

#include "actions/action.h"
#include "actions/playlist.h"

#include "yaml-cpp/yaml.h"

#include <vector>

namespace coffeepot
{
	class Serializer
	{
    public:
        static bool loadActionsAndPlaylists(std::vector<Action>& actions, std::vector<Playlist>& playlist);

		static bool saveActions(const std::vector<Action>& actions);
		static bool loadActions(std::vector<Action>& actions);

        static bool savePlaylists(const std::vector<Playlist>& playlists);
		static bool loadPlaylists(const std::vector<Action>& actions, std::vector<Playlist>& playlists);

		static bool saveWindowSize(int32_t width, int32_t height);
		static bool loadWindowSize(int32_t& width, int32_t& height);
	};
}
