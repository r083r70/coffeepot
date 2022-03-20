
#include "action.h"

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
        bool startAction(Action& action);

    protected:
        ActionsManager() = default;

    private:
        static ActionsManager* m_Instance;

        std::unique_ptr<ActionExecutor> m_Executor;
        std::vector<Action> m_Actions;
    };
}