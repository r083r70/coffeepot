#pragma once

#include "core/action.h"
#include "screens.h"

namespace coffeepot
{
	enum class ActionState;

	class ExecutionScreen : public ScreenWithFooter
	{
    public:
        ExecutionScreen() : ScreenWithFooter("Execution") {}

    protected:
        virtual void tickContent() override;
        virtual void tickFooter() override;

	private:
		void listAction(const Action& action, int32_t actionIndex, ActionState actionState, bool bIsExecutionActive);
	};
}

