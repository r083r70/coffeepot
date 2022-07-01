#pragma once

#include "actions/action.h"
#include "screens.h"

namespace coffeepot
{
	class ExecutionScreen : public Screen
	{
	public:
		virtual void tick() override;

	private:
		void listAction(const Action& action, bool bExecuted);
	};
}

