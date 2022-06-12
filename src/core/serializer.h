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

    protected:
        static YAML::Node createActionNode(const Action& action);
        static Action createAction(const YAML::Node& actionNode);

		static YAML::Node createOptionNode(const Option& option);
        static Option createOption(const YAML::Node& optionNode);
	};
}
