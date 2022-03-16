#pragma once

#include "screens.h"

namespace coffeepot
{
	class Action;
	struct Option;
    
    class ActionsScreen : public Screen
    {
    public:
        virtual void tick() override;

    private:
		void showAction(Action* action);
		void showOption(Option& option);
    };
}