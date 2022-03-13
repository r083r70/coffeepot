
#include "action.h"
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

        const std::vector<Action*>& getAllActions() const { return m_Actions; }
    
    protected:
        ActionsManager() = default;

    private:
        static ActionsManager* m_Instance;
        std::vector<Action*> m_Actions;
    };
}