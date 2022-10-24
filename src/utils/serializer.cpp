
#include "serializer.h"

#include "yaml-cpp/yaml.h"

#include <cassert>
#include <filesystem>
#include <fstream>

namespace YAML
{
    template<>
    struct convert<coffeepot::InputType>
    {
        static Node encode(const coffeepot::InputType& value)
		{
            return Node(coffeepot::InputTypeToString(value));
        }

        static bool decode(const Node& node, coffeepot::InputType& value)
		{
            value = coffeepot::StringToInputType(node.as<std::string>());
            return true;
        }
	};

	template<>
	struct convert<coffeepot::Option>
	{
		static Node encode(const coffeepot::Option& value)
		{
			YAML::Node node;
			node["id"] = value.m_Details.m_ID;
			node["name"] = value.m_Details.m_Name;
			node["inputType"] = value.m_Details.m_Type;

			for (auto& value : value.m_Details.m_ValueList)
				node["valueList"].push_back(value);

			return node;
		}

        static bool decode(const Node& node, coffeepot::Option& value)
		{
			value.m_Details.m_ID = node["id"].as<int>();
			value.m_Details.m_Name = node["name"].as<std::string>();
            value.m_Details.m_Type = node["inputType"].as<coffeepot::InputType>();

			if (const auto& choicesNode = node["valueList"])
			{
				value.m_Details.m_ValueList.reserve(choicesNode.size());
				for (auto it = choicesNode.begin(); it != choicesNode.end(); ++it)
					value.m_Details.m_ValueList.push_back(it->Scalar());
			}

            // Init CheckboxValue
            if (value.m_Details.m_Type == coffeepot::InputType::Checkbox && !value.m_Details.m_ValueList.empty())
                value.m_Value = value.m_Details.m_ValueList[0];

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
	static std::string ActionsFile = "actions.yaml";
	static std::string PlaylistFile = "playlists.yaml";
	static std::string GlobalOptionsFile = "globalOptions.yaml";
	static std::string CoffeepotFile = "coffeepot.yaml";

    bool Serializer::loadActionsAndPlaylists(std::vector<Action>& actions, std::vector<Playlist>& playlist)
    {
        return loadActions(actions) && loadPlaylists(actions, playlist);
    }

	bool Serializer::saveActions(const std::vector<Action>& actions)
	{
        YAML::Node rootNode;        
        for (const auto& action : actions)
            rootNode["actions"].push_back(YAML::Node(action));

        std::ofstream fout(ActionsFile);
        fout << rootNode << std::endl;
        return true;
	}

	bool Serializer::loadActions(std::vector<Action>& actions)
	{
		if (!std::filesystem::exists(ActionsFile))
			return false;

        const YAML::Node rootNode = YAML::LoadFile(ActionsFile);
        const auto& actionsNode = rootNode["actions"];

        actions.clear();
        actions.reserve(actionsNode.size());

        for (auto it = actionsNode.begin(); it != actionsNode.end(); ++it)
		{
			const auto action = it->as<coffeepot::Action>();
			actions.push_back(action);
		}

        return true;
	}

    bool Serializer::savePlaylists(const std::vector<Playlist>& playlists)
    {
        YAML::Node rootNode;

        const auto createOptionNode = [](const Option& option) -> YAML::Node
        {
            YAML::Node optionNode;
            optionNode["id"] = option.m_Details.m_ID;
            optionNode["value"] = option.m_Value;
            return optionNode;
        };

        const auto createActionNode = [&createOptionNode](const Action& action) -> YAML::Node
        {
            YAML::Node actionNode;
            actionNode["id"] = action.m_ID;

            for (auto& option : action.m_Options)
                actionNode["options"].push_back(createOptionNode(option));

            return actionNode;
        };

        const auto createPlaylistNode = [&createActionNode](const Playlist& playlist) -> YAML::Node
        {
            YAML::Node playlistNode;
            playlistNode["name"] = playlist.m_Name;

            for (auto& action : playlist.getActions())
                playlistNode["actions"].push_back(createActionNode(action));

            return playlistNode;
        };

        for (const auto& playlist : playlists)
            rootNode["playlists"].push_back(createPlaylistNode(playlist));

        std::ofstream fout(PlaylistFile);
        fout << rootNode << std::endl;
        return true;
    }

    bool Serializer::loadPlaylists(const std::vector<Action>& actions, std::vector<Playlist>& playlists)
	{
		if (!std::filesystem::exists(PlaylistFile))
			return false;

        const YAML::Node rootNode = YAML::LoadFile(PlaylistFile);
        const auto& playlistsNode = rootNode["playlists"];

        playlists.clear();
        playlists.reserve(playlistsNode.size());

        const auto createAction = [&actions](const YAML::Node& actionNode) -> Action
        {
            const int actionId = actionNode["id"].as<int>();
            auto baseAction = std::find_if(actions.begin(), actions.end(), [actionId](const auto& elem) { return elem.m_ID == actionId; });

            assert(baseAction != actions.end());
            Action action = *baseAction;

			const auto& optionsNode = actionNode["options"];
            if (!optionsNode)
                return action;

            auto& options = action.m_Options;
            for (auto it = optionsNode.begin(); it != optionsNode.end(); ++it)
            {
                const YAML::Node& optionNode = *it;
                const int optionId = optionNode["id"].as<int>();
                auto option = std::find_if(options.begin(), options.end(), [optionId](const auto& elem) { return elem.m_Details.m_ID == optionId; });

                assert(option != options.end());
                option->m_Value = optionNode["value"].as<std::string>();
            }

            return action;
        };

        for (auto it = playlistsNode.begin(); it != playlistsNode.end(); ++it)
		{
            const YAML::Node playlistNode = *it;

            Playlist playlist;
            playlist.m_Name = playlistNode["name"].as<std::string>();

			const auto& actionsNode = playlistNode["actions"];
			if (!actionsNode) // Empty Playlist > Ignore
                continue;
                
            for (auto yt = actionsNode.begin(); yt != actionsNode.end(); ++yt)
                playlist.addAction(createAction(*yt));

			playlists.push_back(playlist);
		}

        return true;
    }

	bool Serializer::saveGlobalOptions(const std::map<std::string, std::string>& globalOptions)
	{
		YAML::Node rootNode;
		for (auto& [key, value] : globalOptions)
		{
			YAML::Node option;
			option["key"] = key;
			option["value"] = value;
			rootNode["options"].push_back(option);
		}

		std::ofstream fout(GlobalOptionsFile);
		fout << rootNode << std::endl;
		return true;
	}

	bool Serializer::loadGlobalOptions(std::map<std::string, std::string>& globalOptions)
	{
		if (!std::filesystem::exists(GlobalOptionsFile))
			return false;

		const YAML::Node rootNode = YAML::LoadFile(GlobalOptionsFile);
		const auto& optionsNode = rootNode["options"];
		if (!optionsNode)
			return false;

		globalOptions.clear();
		//globalOptions.resize(optionsNode.size());

		for (auto it = optionsNode.begin(); it != optionsNode.end(); ++it)
		{
			const auto& option = *it;
			const auto key = option["key"].as<std::string>();
			const auto value = option["value"].as<std::string>();
			globalOptions[key] = value;
		}

		return true;
	}

	bool Serializer::saveWindowSize(int32_t width, int32_t height)
	{
		YAML::Node rootNode;
		rootNode["width"] = width;
		rootNode["height"] = height;

		std::ofstream fout(CoffeepotFile);
		fout << rootNode << std::endl;
		return true;
	}

	bool Serializer::loadWindowSize(int32_t& width, int32_t& height)
	{
		if (!std::filesystem::exists(CoffeepotFile))
			return false;

		const YAML::Node rootNode = YAML::LoadFile(CoffeepotFile);
		width = rootNode["width"].as<int32_t>();
		height = rootNode["height"].as<int32_t>();
		return true;
	}
}
