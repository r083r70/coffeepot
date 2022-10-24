#include "globaloptionscreen.h"

#include "core/actionmanager.h"
#include "utils/utils.h"

#include "imgui.h"

void coffeepot::GlobalOptionScreen::tickContent()
{
	ImGui::BeginTable("App", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable);

	for (auto& [name, value] : ActionsManager::get()->GlobalOptions)
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
		auto& globalOptions = ActionsManager::get()->GlobalOptions;
		if (globalOptions.find(m_NewOptionName) == globalOptions.end())
			globalOptions[m_NewOptionName] = "";
	}
}
