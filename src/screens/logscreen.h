#pragma once

#include "screens.h"
#include <vector>

namespace coffeepot
{
    class LogScreen : public ScreenWithFooter
    {
    public:
        LogScreen();

    protected:
        virtual void tick() override;
        virtual void tickContent() override;
        virtual void tickFooter() override;

    private:
        std::vector<char> m_TextBuffer;
        bool b_AutoScrollDown;
    };
}