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

    class ScreenWithFooter : public Screen
    {
    public:
        virtual void tick();
    protected:
        ScreenWithFooter(const char* name) : m_Name(name) {}
        virtual void tickContent() = 0;
        virtual void tickFooter() = 0;

    private:
        const char* m_Name;
    };
}