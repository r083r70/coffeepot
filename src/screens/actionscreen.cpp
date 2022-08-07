
#include "actionscreen.h"

#include "actions/actionmanager.h"
#include "core/app.h"
#include "core/log.h"
#include "core/utils.h"

#include "imgui.h"


namespace coffeepot
{
     ActionsScreen::ActionsScreen()
        : ScreenWithFooter("Actions")
        , b_CreatingAction(false)
        , m_ActionTemplate()
     {}

    void ActionsScreen::tickContent()
    {
        if (b_CreatingAction)
            renderActionBuilder();
        else
            renderActions();
    }
    
    void ActionsScreen::tickFooter()
    {
        switch (ImGui::BuilderFooter("Action", b_CreatingAction))
        {
            case ImGui::BuilderFooterResult::Save:
                App::get()->addAction(m_ActionTemplate);
                break;
            case ImGui::BuilderFooterResult::Start:
                m_ActionTemplate = Action{};
                break;
        }
    }
    
    void ActionsScreen::renderActions()
    {
        if (!ImGui::BeginTable("", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
            return;
    
        auto& actions = App::get()->getAllActions();
        std::for_each(actions.begin(), actions.end(), [this](auto& elem) { renderAction(elem); });

        ImGui::EndTable();
    }
    
    void ActionsScreen::renderAction(Action& action)
    {
        if (ImGui::ActionTree(action, /*bCanRun =*/ true))
        {
            const bool bResult = ActionsManager::get()->executeAction(action);
            CP_DEBUG("{0} => {1}", action.m_Name, bResult);
        }
    }

    void ActionsScreen::renderActionBuilder()
    {
        ImGui::PushID(&m_ActionTemplate);
        if (!ImGui::BeginTable("", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
            return;
        
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
        ImGui::Text("Name");
        ImGui::TableSetColumnIndex(1);
		ImGui::InputString("ActionName", m_ActionTemplate.m_Name);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
        ImGui::Text("Command");
        ImGui::TableSetColumnIndex(1);
		ImGui::InputString("ActionCommand", m_ActionTemplate.m_Command);

        if (m_ActionTemplate.m_Options.size() != 0)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
		    ImGui::AlignTextToFramePadding();
            ImGui::Text("Options");

            auto& options = m_ActionTemplate.m_Options;
            std::for_each(options.begin(), options.end(), [this](auto& elem) { renderOptionBuilder(elem); });
        }

        ImGui::EndTable();
        ImGui::Separator();

        if (ImGui::Button("Add Option"))
        {
            Option& newOption = m_ActionTemplate.m_Options.emplace_back();
            newOption.m_ID = static_cast<int32_t>(m_ActionTemplate.m_Options.size());
            newOption.m_ValueInfo.m_Type = InputType::Text;
        }

        ImGui::PopID();
    }

    void ActionsScreen::renderOptionBuilder(Option& option)
    {
        ImGui::PushID(&option);

        // First line
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
        ImGui::Bullet(); ImGui::SameLine();
        const float cursonPosX = ImGui::GetCursorPosX(); // for alignment
        ImGui::Text("Name");

        ImGui::TableSetColumnIndex(1);
        ImGui::InputString("OptionName", option.m_Name);

		// Second line
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
        ImGui::SetCursorPosX(cursonPosX); // align to previous line
        ImGui::Text("DefaultValue");

        ImGui::TableSetColumnIndex(1);
        ImGui::InputString("OptionDefaultValue", option.m_ValueInfo.m_Default);

        ImGui::PopID();
    }
}