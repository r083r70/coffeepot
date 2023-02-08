
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

#include "imgui.h"

#include <vector>
#include <algorithm>

namespace coffeepot
{
	class UILayer : public kettle::Layer
	{
	public:
		UILayer()
		{
			m_Screens.push_back(new MainMenuBarScreen());
			m_Screens.push_back(new ActionsScreen());
			m_Screens.push_back(new ExecutionScreen());
			m_Screens.push_back(new LogScreen());
			m_Screens.push_back(new PlaylistScreen());
			m_Screens.push_back(new GlobalOptionScreen());
		}

		virtual void tick() override
		{
			std::for_each(m_Screens.begin(), m_Screens.end(), [](auto Elem) { Elem->tick(); });
		}

	private:
		std::vector<Screen*> m_Screens;
	};

	int main()
	{
		int32_t width, height;
		Serializer::loadWindowSize(width, height);

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

		kettle::App app{ "Coffeepot", width, height };
		app.pushLayer<Platform>();
		app.pushLayer<ActionsManager>();
		app.pushLayer<UILayer>();
		app.run();

		Serializer::saveWindowSize(width, height);
		return 0;
	}
}

#ifdef CP_DIST
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	return coffeepot::main();
}
#else
int main()
{
	return coffeepot::main();
}
#endif
