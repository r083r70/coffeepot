
#include "serializer.h"

#include "actions/actionmanager.h"
#include "core/log.h"

#include "yaml-cpp/yaml.h"

#include<fstream>

namespace coffeepot
{
	bool Serializer::saveActions()
	{
        YAML::Node rootNode;
        
        const auto& actions = ActionsManager::get()->getAllActions();
        for (const auto& action : actions)
        {
            YAML::Node actionNode;
            actionNode["id"] = action.m_ID;
            actionNode["name"] = action.m_Name;
            actionNode["command"] = action.m_Command;

            for (const auto& option : action.m_Options)
            {
                YAML::Node optionNode;
                optionNode["id"] = option.m_ID;
                optionNode["name"] = option.m_Name;
                optionNode["defaultValue"] = option.m_DefaultValue;

                optionNode["options"].push_back(optionNode);
            }

            rootNode["actions"].push_back(actionNode);
        }

        std::ofstream fout("actions.yaml");
        fout << rootNode << std::endl;
        return true;
	}

	bool Serializer::loadActions(std::vector<Action>& actions)
	{
        YAML::Node rootNode = YAML::LoadFile("actions.yaml");
        const auto& actionsNode = rootNode["actions"];

        actions.clear();
        actions.reserve(actionsNode.size());
        CP_TRACE("Found {} actions", actionsNode.size());

        for (auto it = actionsNode.begin(); it != actionsNode.end(); ++it)
        {
            const auto& actionNode = *it;
            Action& action = actions.emplace_back();
            action.m_ID = actionNode["id"].as<int>();
            action.m_Name = actionNode["name"].as<std::string>();
            action.m_Command = actionNode["command"].as<std::string>();
            
            if (!actionNode["options"])
                continue;

            const auto& optionsNode = actionNode["options"];
            action.m_Options.reserve(optionsNode.size());

            for (auto yt = optionsNode.begin(); yt != optionsNode.end(); ++yt)
            {
                const auto& optionNode = *yt;
                Option& option = action.m_Options.emplace_back();
                option.m_ID = optionNode["id"].as<int>();
                option.m_Name = optionNode["name"].as<std::string>();
                option.m_DefaultValue = optionNode["defaultValue"].as<std::string>();
                option.m_Value = option.m_DefaultValue;
            }
        }

        return true;
	}
}
