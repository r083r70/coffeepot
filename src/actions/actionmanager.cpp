
#include "actionmanager.h"

#include "core/log.h"
#include "core/serializer.h"

#include "imgui.h"

#include <cassert>
#include <cstring>
#include <mutex>

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#if CP_WIN
#include "process.h"
#endif

namespace coffeepot
{
    ActionsManager* ActionsManager::s_Instance = nullptr;
    std::mutex g_ActionMutex;
    std::mutex g_OutputMutex;

	ActionsManager* ActionsManager::get()
    {
        if (!s_Instance)
            s_Instance = new ActionsManager();
        return s_Instance;
    }

    bool ActionsManager::init()
    {
        b_Ending = false;
        m_Thread = std::thread(&ActionsManager::threadedTick, this);
        return true;
    }

    void ActionsManager::deinit()
    {
        b_Ending = true;
        m_Thread.join();
    }

    bool ActionsManager::executeAction(const Action& action)
	{
		const std::lock_guard<std::mutex> playlistLock(g_ActionMutex);

        if (!m_ExecutionState.b_Running)
            m_ExecutionPlaylist.removeAllAction(); // Clean Playlist

        m_ExecutionPlaylist.addAction(action);
        return true;
    }

    bool ActionsManager::executePlaylist(const Playlist& playlist)
    {
		const std::lock_guard<std::mutex> playlistLock(g_ActionMutex);

        if (!m_ExecutionPlaylist.hasNextAction())
            m_ExecutionPlaylist.removeAllAction(); // It resets the nextActionIndex

        for (auto& action : playlist.getActions())
            m_ExecutionPlaylist.addAction(action);

        return true;
    }

    void ActionsManager::killExecution()
    {
        emptyExecutionPlaylist();
        stopCurrentAction();
    }

	void ActionsManager::readOutput(ImGuiTextBuffer& textOutput)
    {
        if (!g_OutputMutex.try_lock())
            return;
        
        char* output = m_OutputBuffer.data();
        textOutput.append(output); // memcpy the data
		memset(output, 0, strlen(output)); // clean the data

        g_OutputMutex.unlock();
    }

    void ActionsManager::threadedTick()
    {
        std::array<char, 2048> localBuffer;
        while (!b_Ending)
        {
            if (!m_ExecutionState.b_Running)
                maybeStartNextAction();
            
            if (!m_ExecutionState.b_Running)
                continue;

            size_t outputFreeSize = 0; // Check Output freeSize
            {
                const std::lock_guard<std::mutex> outputLock(g_OutputMutex);

                const size_t outputLength = strlen(m_OutputBuffer.data());
                outputFreeSize = m_OutputBuffer.size() - outputLength;
            }

            if (outputFreeSize <= 0) // Output is full, retry later
                continue;

            // Read in the LocalBuffer and later copy in the Output
            if (fgets(localBuffer.data(), outputFreeSize, m_ExecutionState.m_Pipe))
            {
			    const std::lock_guard<std::mutex> outputLock(g_OutputMutex);

                char* output = m_OutputBuffer.data();
                const size_t outputLength = strlen(output);
                memcpy(&output[outputLength], localBuffer.data(), outputFreeSize);
            }
            // Read failed > Action is completed
            else
            {
                fclose(m_ExecutionState.m_Pipe);
                stopCurrentAction();
            }
        }
    }

	bool ActionsManager::maybeStartNextAction()
    {
        const std::lock_guard<std::mutex> lock(g_ActionMutex);
        if (!m_ExecutionPlaylist.hasNextAction())
            return false;
        
        const Action& nextAction = m_ExecutionPlaylist.getNextAction();
        return startAction(nextAction);
    }

	bool ActionsManager::startAction(const Action& action)
    {
        constexpr int READ = 0;
        constexpr int WRITE = 1;

        int fd[2];
        if (pipe(fd) != 0)
            return false;

        m_ExecutionState.m_PID = fork();
        if (m_ExecutionState.m_PID == -1)
            return false;

        if (m_ExecutionState.m_PID == 0)
        {
            dup2(fd[WRITE], STDOUT_FILENO);
            dup2(fd[WRITE], STDERR_FILENO);
            
            close(fd[READ]);
            close(fd[WRITE]);

            setpgid(m_ExecutionState.m_PID, m_ExecutionState.m_PID);

            const std::string cmd = action.createFullCommand();

#if CP_WIN
            _execl("C::\\WINDOWS\\SYSTEM32\\CMD.EXE", "cmd.exe", "/c", cmd.c_str(), nullptr);
#elif CP_LINUX
            execl("/bin/sh", "/bin/sh", "-c", cmd.c_str(), nullptr);
#endif

            exit(1);
        }
        else
        {
            close(fd[WRITE]);
        }

        m_ExecutionState.m_Pipe = fdopen(fd[READ], "r");
        m_ExecutionState.b_Running = true;
        return true;
    }

    void ActionsManager::emptyExecutionPlaylist()
    {
        const std::lock_guard<std::mutex> playlistLock(g_ActionMutex);
        m_ExecutionPlaylist.removeAllAction();
    }

    void ActionsManager::stopCurrentAction()
    {
        if (!m_ExecutionState.b_Running)
            return;

        const std::lock_guard<std::mutex> lock(g_ActionMutex);

        CP_DEBUG("Killing {}", m_ExecutionState.m_PID);

        m_ExecutionState.b_Running = false;
        kill(-m_ExecutionState.m_PID, SIGKILL);
        waitpid(m_ExecutionState.m_PID, nullptr, 0);
    }
}