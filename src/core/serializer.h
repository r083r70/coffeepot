#pragma once

#include "actions/action.h"
#include "yaml-cpp/yaml.h"

#include <vector>

namespace coffeepot
{
	class Serializer
	{
    public:
		static bool saveActions(const std::vector<Action>& actions);
		static bool loadActions(std::vector<Action>& actions);

		static bool saveWindowSize(int32_t width, int32_t height);
		static bool loadWindowSize(int32_t& width, int32_t& height);
	};
}
