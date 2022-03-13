
#include "actionscreen.h"

#include "actions/actionmanager.h"
#include "core/log.h"

#include "imgui.h"

namespace coffeepot
{
    void ActionsScreen::tick()
    {
        if (!ImGui::Begin("Commands"))
            return;
        
        if (!ImGui::BeginTable("", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
            return;
    
        const auto& actions = ActionsManager::get()->getAllActions();
        for (auto it = actions.begin(); it != actions.end(); it++)
        {
            Action* const action = *it;
            if (action != nullptr)
                showAction(action);
        }

        ImGui::EndTable();
        ImGui::End();
    }

    void ActionsScreen::showAction(const Action* action)
    {
        ImGui::PushID(action);
        const auto actionName = action->getName();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        
        bool bTreeNode = false;
        if (action->getOptionsCount() != 0)
            bTreeNode = ImGui::TreeNode("Action", actionName.c_str());
        else
            ImGui::Text(actionName.c_str());

        ImGui::TableSetColumnIndex(1);
        if (ImGui::Button("Run") && !action->isRunning())
        {
            const bool bIsActionStarted = false;//action->start();
            CP_DEBUG("[{}] Start => {}", actionName, bIsActionStarted);
        }

        if (bTreeNode)
        {
            auto options = action->getOptions();
            for (auto it = options.begin(); it != options.end(); ++it)
            {
                auto& option = *it;
                ImGui::PushID(&option);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
                ImGui::TreeNodeEx("Option", flags, option.m_Name.c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::InputText(option.m_Name.c_str(), option.m_Value.data(), option.m_Value.size());
                
                ImGui::NextColumn();
                ImGui::PopID();
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
        ImGui::Separator();
    }
}