
#include "core/actionmanager.h"
#include "core/app.h"
#include "core/layer.h"
#include "core/platform.h"
#include "fa_icons.h"

#include "screens/actionscreen.h"
#include "screens/executionscreen.h"
#include "screens/globaloptionscreen.h"
#include "screens/logscreen.h"
#include "screens/playlistscreen.h"

#include "utils/serializer.h"

#include <vector>
#include <algorithm>

class UILayer : public kettle::Layer
{
public:
	UILayer()
	{
        m_Screens.push_back(new coffeepot::MainMenuBarScreen());
		m_Screens.push_back(new coffeepot::ActionsScreen());
		m_Screens.push_back(new coffeepot::ExecutionScreen());
		m_Screens.push_back(new coffeepot::LogScreen());
		m_Screens.push_back(new coffeepot::PlaylistScreen());
		m_Screens.push_back(new coffeepot::GlobalOptionScreen());
	}

	virtual void tick() override
	{
		std::for_each(m_Screens.begin(), m_Screens.end(), [](auto Elem) { Elem->tick(); });
	}

private:
	std::vector<coffeepot::Screen*> m_Screens;
};


int main()
{
	int32_t width, height;
	coffeepot::Serializer::loadWindowSize(width, height);

	// Setup Fonts
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.GlyphMinAdvanceX = 16.0f;
		icons_config.PixelSnapH = true;
		io.Fonts->AddFontDefault();
		io.Fonts->AddFontFromFileTTF("res/fa-solid-900.otf", 11.0f, &icons_config, icons_ranges);
		io.Fonts->Build();
	}

	kettle::App app{ "Title", width, height };
	app.pushLayer<coffeepot::Platform>();
	app.pushLayer<coffeepot::ActionsManager>();
    app.pushLayer<UILayer>();
	app.run();

	coffeepot::Serializer::saveWindowSize(width, height);
}
