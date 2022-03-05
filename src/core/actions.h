#pragma once

#include <array>
#include <string>

namespace coffeepot
{
    class Action
    {
    public:
        Action() = default;
        virtual ~Action() { abort(); }

        virtual bool isRunning() const { return false; }
        virtual std::string getName() const { return "InvalidAction"; }

        virtual bool start() { return false; }
        virtual bool update() = 0;

        virtual void abort() {}
    };

    class CmdAction : public Action
    {
    public:
        CmdAction(const std::string& command);
        
        virtual bool isRunning() const override;
        virtual std::string getName() const override;

        virtual bool start() override;
        virtual bool update() override;

        virtual void abort() override;

    protected:
        void stop();

    private:
        std::string m_Command;
        
        std::array<char, 128> m_Buffer;
        FILE* m_Pipe;
    };
}