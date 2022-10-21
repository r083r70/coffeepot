#pragma once

#include "screens.h"
#include <string>

namespace coffeepot
{
	class GlobalOptionScreen : public ScreenWithFooter
	{
	public:
		GlobalOptionScreen() : ScreenWithFooter("Options") {}

		virtual void tickContent() override;
		virtual void tickFooter() override;

	private:
		std::string m_NewOptionName;
	};
}