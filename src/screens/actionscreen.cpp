
#include "actionscreen.h"

#include "actions/actionmanager.h"
#include "core/log.h"

#include "imgui.h"
#include "imgui_internal.h"

namespace coffeepot
{
	int inputTextCallback(ImGuiInputTextCallbackData* data)
	{
        if (data->EventFlag != ImGuiInputTextFlags_CallbackResize)
            return 0;

		auto option = static_cast<Option*>(data->UserData);
		option->m_Value.resize(data->BufTextLen);
		data->Buf = const_cast<char*>(option->m_Value.c_str());
		return 1;
	}

    void ActionsScreen::tick()
    {
        if (!ImGui::Begin("Commands"))
            return;
        
        if (!ImGui::BeginTable("", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
            return;
    
        auto& actions = ActionsManager::get()->getAllActions();
        std::for_each(actions.begin(), actions.end(), [this](auto& elem) { showAction(elem); });

        ImGui::EndTable();
        ImGui::End();
    }

    void ActionsScreen::showAction(Action& action)
    {
        ImGui::PushID(&action);
        const std::string& actionName = action.m_Name;

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        
        bool bTreeNode = false;
        if (action.m_Options.size() != 0)
            bTreeNode = ImGui::TreeNode("Action", actionName.c_str());
        else
            ImGui::BulletText(actionName.c_str());

        ImGui::TableSetColumnIndex(1);
        if (ImGui::Button("Run"))
        {
            const bool bIsActionStarted = ActionsManager::get()->startAction(action);
            CP_DEBUG("[{}] Start => {}", actionName, bIsActionStarted);
        }

        if (bTreeNode)
        {
            auto& options = action.m_Options;
			std::for_each(options.begin(), options.end(), [this](auto& elem) { showOption(elem); });

            ImGui::TreePop();
        }

        ImGui::PopID();
        ImGui::Separator();
    }

	void ActionsScreen::showOption(Option& option)
	{
		ImGui::PushID(&option);
		ImGui::TableNextRow();

		// > Column 0
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx("OptionName", treeNodeFlags, option.m_Name.c_str());

		// > Column 1
		ImGui::TableSetColumnIndex(1);

		// Set ItemWidth to leave space for the Button
		const float buttonSize = ImGui::GetFrameHeight();
		ImGui::SetNextItemWidth(-buttonSize);

		// Remove ItemSpacing
		const float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
		ImGui::GetStyle().ItemSpacing.x = 0;

		// Draw InputText - PushID to avoid Label
		ImGui::PushID("OptionValue");
		ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CallbackResize;
		ImGui::InputText("", option.m_Value.data(), option.m_Value.size() + 1, inputTextFlags, &inputTextCallback, &option);
		ImGui::PopID();

		// Draw Button
		ImGui::SameLine();
		if (ImGui::Button("", ImVec2(buttonSize, buttonSize)))
			option.m_Value = option.m_DefaultValue;

		// Restore ItemSpacing
		ImGui::GetStyle().ItemSpacing.x = itemSpacing;

		ImGui::NextColumn();
		ImGui::PopID();
	}
}