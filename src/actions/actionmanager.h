
#include "action.h"

#include <array>
#include <memory>
#include <vector>

namespace coffeepot
{
    class ActionsManager
    {
    public:
        static ActionsManager* get();

        bool init();
        void deinit();

        void tick();

        std::vector<Action>& getAllActions() { return m_Actions; }
        void addAction(Action& action) { m_Actions.push_back(action); }

        bool startAction(Action& action);

        const char* readOutput();

    protected:
        ActionsManager() = default;

    private:
        static ActionsManager* m_Instance;

        std::unique_ptr<ActionExecutor> m_Executor;
        std::vector<Action> m_Actions;

        std::array<char, 256> m_OutputBuffer;
    };
}