#pragma once

#include "actions/action.h"
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
		void renderAction(Action& action);
		void renderOption(Option& option);

        void renderActionBuilder();
        void renderOptionBuilder(Option& option);

    private:
        bool b_CreatingAction = false;
        Action m_ActionTemplate;
    };
}