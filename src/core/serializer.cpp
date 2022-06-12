
#include "serializer.h"

#include "actions/actionmanager.h"
#include "core/log.h"

#include "yaml-cpp/yaml.h"

#include<fstream>

namespace YAML
{
    template<>
    struct convert<coffeepot::InputType>
    {
        static Node encode(const coffeepot::InputType& value)
        {
            int intValue = static_cast<int>(value);
            return Node(intValue);
        }

        static bool decode(const Node& node, coffeepot::InputType& value)
        {
            const int intValue = node.as<int>();
            value = static_cast<coffeepot::InputType>(intValue);
            return true;
        }
    };
}

namespace coffeepot
{
	bool Serializer::saveActions(const std::vector<Action>& actions)
	{
        YAML::Node rootNode;        
        for (const auto& action : actions)
            rootNode["actions"].push_back(createActionNode(action));

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
            actions.push_back(createAction(*it));

        return true;
	}

    YAML::Node Serializer::createActionNode(const Action& action)
    {
        YAML::Node actionNode;
        actionNode["id"] = action.m_ID;
        actionNode["name"] = action.m_Name;
        actionNode["command"] = action.m_Command;

        for (const auto& option : action.m_Options)
            actionNode["options"].push_back(createOptionNode(option));

        return actionNode;
    }

    Action Serializer::createAction(const YAML::Node& actionNode)
    {
        Action action;
        action.m_ID = actionNode["id"].as<int>();
        action.m_Name = actionNode["name"].as<std::string>();
        action.m_Command = actionNode["command"].as<std::string>();

        const auto& optionsNode = actionNode["options"];    
        if (!optionsNode)
            return action;

        action.m_Options.reserve(optionsNode.size());
        for (auto it = optionsNode.begin(); it != optionsNode.end(); ++it)
            action.m_Options.push_back(createOption(*it));

        return action;
    }

    YAML::Node Serializer::createOptionNode(const Option& option)
    {
        YAML::Node optionNode;
        optionNode["id"] = option.m_ID;
        optionNode["name"] = option.m_Name;

        // ValueInfo
        optionNode["inputType"] = option.m_ValueInfo.m_Type;
        optionNode["default"] = option.m_ValueInfo.m_Default;
        for (auto& value : option.m_ValueInfo.m_Choices)
            optionNode["choices"].push_back(value);

        return optionNode;
    }

    Option Serializer::createOption(const YAML::Node& optionNode)
    {
        Option option;
        option.m_ID = optionNode["id"].as<int>();
        option.m_Name = optionNode["name"].as<std::string>();

        // ValueInfo
        option.m_ValueInfo.m_Default = optionNode["default"].as<std::string>();
        if (const auto& choicesNode = optionNode["choices"])
        {
            option.m_ValueInfo.m_Choices.reserve(choicesNode.size());
            for (auto it = choicesNode.begin(); it != choicesNode.end(); ++it)
                option.m_ValueInfo.m_Choices.push_back(it->Scalar());
        }

        const auto& inputTypeNode = optionNode["inputType"];
        if (inputTypeNode)
            option.m_ValueInfo.m_Type = inputTypeNode.as<InputType>();
        else if (option.m_ValueInfo.m_Choices.size() > 0)
            option.m_ValueInfo.m_Type = InputType::ComboBox;
        else
            option.m_ValueInfo.m_Type = InputType::Text;

        option.m_Value = option.m_ValueInfo.m_Default;
        return option;
    }
}
