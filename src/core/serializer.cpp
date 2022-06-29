
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

	template<>
	struct convert<coffeepot::Option>
	{
		static Node encode(const coffeepot::Option& value)
		{
			YAML::Node node;
			node["id"] = value.m_ID;
			node["name"] = value.m_Name;

			// ValueInfo
			node["inputType"] = value.m_ValueInfo.m_Type;
			node["default"] = value.m_ValueInfo.m_Default;
			for (auto& value : value.m_ValueInfo.m_Choices)
				node["choices"].push_back(value);

			return node;
		}

        static bool decode(const Node& node, coffeepot::Option& value)
		{
			value.m_ID = node["id"].as<int>();
			value.m_Name = node["name"].as<std::string>();

			// ValueInfo
			value.m_ValueInfo.m_Default = node["default"].as<std::string>();
			if (const auto& choicesNode = node["choices"])
			{
				value.m_ValueInfo.m_Choices.reserve(choicesNode.size());
				for (auto it = choicesNode.begin(); it != choicesNode.end(); ++it)
					value.m_ValueInfo.m_Choices.push_back(it->Scalar());
			}

			const auto& inputTypeNode = node["inputType"];
			if (inputTypeNode)
				value.m_ValueInfo.m_Type = inputTypeNode.as<coffeepot::InputType>();
			else if (value.m_ValueInfo.m_Choices.size() > 0)
				value.m_ValueInfo.m_Type = coffeepot::InputType::ComboBox;
			else
				value.m_ValueInfo.m_Type = coffeepot::InputType::Text;

			value.m_Value = value.m_ValueInfo.m_Default;
			return true;
        }
	};

	template<>
    struct convert<coffeepot::Action>
	{
		static Node encode(const coffeepot::Action& value)
		{
			YAML::Node node;
			node["id"] = value.m_ID;
			node["name"] = value.m_Name;
			node["command"] = value.m_Command;

			for (const auto& option : value.m_Options)
				node["options"].push_back(option);

			return node;
		}

		static bool decode(const Node& node, coffeepot::Action& value)
		{
            value.m_ID = node["id"].as<int>();
            value.m_Name = node["name"].as<std::string>();
            value.m_Command = node["command"].as<std::string>();

			const auto& optionsNode = node["options"];
			if (!optionsNode)
				return true;

            value.m_Options.reserve(optionsNode.size());
			for (auto it = optionsNode.begin(); it != optionsNode.end(); ++it)
			{
                const auto option = it->as<coffeepot::Option>();
				value.m_Options.push_back(option);
			}

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
            rootNode["actions"].push_back(YAML::Node(action));

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
			const auto action = it->as<coffeepot::Action>();
			actions.push_back(action);
		}

        return true;
	}

	bool Serializer::saveWindowSize(int32_t width, int32_t height)
	{
		YAML::Node rootNode;
		rootNode["width"] = width;
		rootNode["height"] = height;

		std::ofstream fout("coffeepot.yaml");
		fout << rootNode << std::endl;
		return true;
	}

	bool Serializer::loadWindowSize(int32_t& width, int32_t& height)
	{
		YAML::Node rootNode = YAML::LoadFile("coffeepot.yaml");
		width = rootNode["width"].as<int32_t>();
		height = rootNode["height"].as<int32_t>();
		return true;
	}
}
