#pragma once

#include <vector>

namespace coffeepot
{
	class Action;

	class Serializer
	{
    public:
		static bool saveActions();
		static bool loadActions(std::vector<Action>& actions);
	};
}
