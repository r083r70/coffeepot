#pragma once

#include <string>
#include <vector>

namespace coffeepot
{
    struct Option
    {
    public:
        Option(const std::string& name, int32_t id, const std::string& value)
            : m_Name(name), m_ID(id) , m_DefaultValue(value) , m_Value(value)
        {}

        std::string m_Name;
        int32_t m_ID;

        std::string m_DefaultValue;
        std::string m_Value;
    };

    class Action
    {
    public:
        static Action* Create(const std::string& command);
        static Action* CreateDemo();

        ~Action() { abort(); }

        inline const std::vector<Option>& getOptions() const { return m_Options; }
        inline size_t getOptionsCount() const { return m_Options.size(); }

        std::string getName() const;
        bool isRunning() const;

        bool start();
        bool update();

        void abort();

    protected:
        Action(const std::string& command);

        std::string createFullCommand() const;
        const std::string& getOptionValueByID(int32_t id) const;

        void stop();

    private:
        std::string m_Command;
        std::vector<Option> m_Options;

        FILE* m_Pipe;
    };
}
