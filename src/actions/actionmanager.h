
#include "playlist.h"

#include <array>
#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#if CP_WINDOWS
#include <wtypes.h>
#endif

struct ImGuiTextBuffer;

namespace coffeepot
{
    struct ExecutionState
    {
#if CP_WINDOWS
		PROCESS_INFORMATION m_ProcessInformation;
		HANDLE m_ProcessOutput;
		HANDLE m_Job;
#elif CP_LINUX
		pid_t m_PID = -1;
		FILE* m_Pipe = nullptr;
#endif

        std::atomic<bool> b_Running = false;
    };

    class ActionsManager
    {
    public:
        static ActionsManager* get();

        bool init();
        void deinit();
        void tick() {}

        const Playlist& getExecutionPlaylist() const { return m_ExecutionPlaylist; }

        bool executeAction(const Action& action);
        bool executePlaylist(const Playlist& playlist);

        bool isExecutingAction() const { return m_ExecutionState.b_Running; }
        void killExecution();

        void readOutput(ImGuiTextBuffer& textOutput);

    protected:
        ActionsManager() = default;

    private:
        void threadedTick();
        
        bool maybeStartNextAction();
        bool startAction(const Action& action);

        void emptyExecutionPlaylist();
        void stopCurrentAction();

        void killAction();

    private:
        static ActionsManager* s_Instance;
        Playlist m_ExecutionPlaylist;

        ExecutionState m_ExecutionState;
        std::array<char, 2048> m_OutputBuffer;

        std::thread m_Thread;
		std::atomic<bool> b_Ending;
    };
}