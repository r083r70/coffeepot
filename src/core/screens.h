#pragma once

#include "actions.h"
#include <vector>

namespace coffeepot
{
    class Screen
    {
    public:
        virtual void tick() = 0;
    };

    class MainMenuBarScreen : public Screen
    {
    public:
        virtual void tick() override;
    };

    class CmdListScreen : public Screen
    {
    public:
        CmdListScreen();
        virtual void tick() override;

    private:
        std::vector<Action*> m_Actions;
    };
}