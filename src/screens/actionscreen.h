#pragma once

#include "screens.h"

namespace coffeepot
{
    class Action;
    
    class ActionsScreen : public Screen
    {
    public:
        virtual void tick() override;

    private:
        void showAction(const Action* action);
    };
}