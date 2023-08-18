#pragma once

#include "core/action.h"
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
		enum class ActionState { None, Done, Running, Waiting, Skipped };
		void listAction(const Action& action, int32_t actionIndex, ActionState actionState, bool bIsExecutionActive);
	};
}

