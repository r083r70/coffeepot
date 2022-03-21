#pragma once

#include "actions/action.h"
#include "screens.h"

#include <string>

namespace coffeepot
{
    class ActionsScreen : public Screen
    {
    public:
        virtual void tick() override;

    private:
        void renderActions();
		void renderAction(Action& action);
		void renderOption(Option& option);

        void renderActionBuilder();
        void renderOptionBuilder(Option& option);

        void renderFooter();

    private:
        bool m_CreatingAction = false;
        Action m_ActionTemplate;
    };
}