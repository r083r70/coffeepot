
#include "playlist.h"

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

        bool executeAction(const Action& action);

        const char* readOutput();

    protected:
        ActionsManager() = default;

        bool startNextAction();

    private:
        static ActionsManager* m_Instance;
        std::vector<Action> m_Actions;

        std::unique_ptr<ActionExecutor> m_Executor;
        Playlist m_CurrentPlaylist;

        std::array<char, 256> m_OutputBuffer;
    };
}