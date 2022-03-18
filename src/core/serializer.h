#pragma once

#include <vector>

namespace coffeepot
{
	class Action;

	class Serializer
	{
		static bool saveActions(const std::vector<Action*>& actions);
		static bool loadActions(std::vector<Action*>& actions);
	};
}
