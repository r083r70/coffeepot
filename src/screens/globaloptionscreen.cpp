#include "globaloptionscreen.h"

#include "core/app.h"
#include "core/utils.h"

#include "imgui.h"

void coffeepot::GlobalOptionScreen::tickContent()
{
	ImGui::BeginTable("App", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable);

	for (auto& [name, value] : App::get()->getGlobalOptions())
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text(name.c_str());
		ImGui::TableSetColumnIndex(1);
		ImGui::InputString("Option", value, ImGuiInputTextFlags_CharsNoBlank);
	}

	ImGui::EndTable();
}

void coffeepot::GlobalOptionScreen::tickFooter()
{
	ImGui::InputString("NewOption", m_NewOptionName, ImGuiInputTextFlags_CharsNoBlank);
	ImGui::SameLine();

	if (ImGui::Button("AddOption"))
	{
		auto& globalOptions = App::get()->getGlobalOptions();
		if (globalOptions.find(m_NewOptionName) == globalOptions.end())
			globalOptions[m_NewOptionName] = "";
	}
}
