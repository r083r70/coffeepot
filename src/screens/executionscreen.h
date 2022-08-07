#pragma once

#include "actions/action.h"
#include "screens.h"

namespace coffeepot
{
	class ExecutionScreen : public ScreenWithFooter
	{
    public:
        ExecutionScreen() : ScreenWithFooter("Execution") {}

    protected:
        virtual void tickContent() override;
        virtual void tickFooter() override;

	private:
		void listAction(const Action& action, bool bExecuted);
	};
}

