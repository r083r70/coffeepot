#pragma once

#include "core/layer.h"
#include "playlist.h"

#include <array>
#include <atomic>
#include <map>
#include <memory>
#include <thread>
#include <vector>

#if CP_WINDOWS
#include <wtypes.h>
#endif

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

    class ActionsManager : public kettle::Layer
    {
    public:
        static ActionsManager* get();
        ActionsManager();

        virtual void start() override;
        virtual void stop() override;
        virtual void tick() override {}

		void reloadAll();
		void saveAll();

		const ExecutionPlaylist& getExecutionPlaylist() const { return m_ExecutionPlaylist; }

		bool isExecuting() const;
        bool clearExecutionPlaylist();
		bool restartExecutionPlaylist();

		bool executePlaylist(const Playlist& playlist);
        bool executeAction(const Action& action);
        bool removeAction(int32_t actionIndex);

		void killExecution();

        void moveBuffer(std::vector<char>& destination);

	private:
		void threadedTick();
		void executeAction();
        
        bool maybeStartNextAction();
		bool startAction(const Action& action);

		void stopActiveAction(bool bForcedTerminate = false);
	
	public:
		std::vector<Action> Actions;
		std::vector<Playlist> Playlists;
		std::map<std::string, std::string> GlobalOptions;

    private:
        static ActionsManager* s_Instance;
        ExecutionPlaylist m_ExecutionPlaylist;

        ExecutionState m_ExecutionState;
        std::array<char, 2048> m_Buffer;

        std::thread m_Thread;
		std::atomic<bool> b_Ending;
    };
}