#pragma once

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
}