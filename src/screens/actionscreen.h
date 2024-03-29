#pragma once

#include "core/action.h"
#include "screens.h"

#include <string>

namespace coffeepot
{
    class ActionsScreen : public ScreenWithFooter
    {
    public:
        ActionsScreen();

    protected:
        virtual void tickContent() override;
        virtual void tickFooter() override;

    private:
        void renderActions();

        void renderTemplateActionSelector(Action& outAction);
        void renderActionBuilder();
        void renderOptionBuilder(OptionDetails& optionDetails);

    private:
        bool b_CreatingAction = false;
        Action m_ActionTemplate;

        // Rename
		Action* m_RenamingAction = nullptr;
		std::string m_NewActionName;

		// Delete
        Action* m_DeletingAction = nullptr;
        bool b_DeleteConfirmed = false;
    };
}