
#include "playlist.h"

#include <array>
#include <memory>
#include <thread>
#include <vector>

struct ImGuiTextBuffer;

namespace coffeepot
{
    class ThreadedActionManager
    {
    public:
        ThreadedActionManager() = default;
        void operator()();

    private:
        bool update();

    private:
        std::array<char, 2048> m_OutputBuffer;
    };

    class ActionsManager
    {
    public:
        static ActionsManager* get();

        bool init();
        void deinit();

        void tick();

        bool executeAction(const Action& action);
        bool executePlaylist(const Playlist& playlist);

        const Playlist& getCurrentPlaylist() const { return m_CurrentPlaylist; }

        void readOutput(ImGuiTextBuffer& textOutput);

    protected:
        ActionsManager() = default;

        bool startNextAction();

    private:
        static ActionsManager* s_Instance;

        std::unique_ptr<ActionExecutor> m_Executor;
        Playlist m_CurrentPlaylist;

        std::array<char, 2048> m_OutputBuffer;

		std::thread m_ThreadedActionManager;
		bool b_Terminating;

        friend class ThreadedActionManager;
    };
}