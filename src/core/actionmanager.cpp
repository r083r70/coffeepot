
#include "actionmanager.h"

#include "utils/serializer.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstring>
#include <mutex>

#if CP_WINDOWS
#include <atlbase.h>
#include <atlconv.h>
#include <windows.h>
#elif CP_LINUX
#include <csignal>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace coffeepot
{
    ActionsManager* ActionsManager::s_Instance = nullptr;
    std::mutex g_ActionMutex;
    std::mutex g_OutputMutex;

	ActionsManager* ActionsManager::get()
    {
        return s_Instance;
    }

	ActionsManager::ActionsManager()
        : m_ExecutionState()
        , m_Buffer()
        , b_Ending(false)
	{
		s_Instance = this;

		Serializer::loadGlobalOptions(GlobalOptions);
		Serializer::loadActionsAndPlaylists(Actions, Playlists);
	}

    void ActionsManager::start()
    {
        b_Ending = false;
        m_Thread = std::thread(&ActionsManager::threadedTick, this);
    }

    void ActionsManager::stop()
    {
        b_Ending = true;
        m_Thread.join();
    }

	void ActionsManager::reloadAll()
	{
		Serializer::loadGlobalOptions(GlobalOptions);
		Serializer::loadActionsAndPlaylists(Actions, Playlists);
	}

	void ActionsManager::saveAll()
	{
		Serializer::saveGlobalOptions(GlobalOptions);
		Serializer::saveActions(Actions);
		Serializer::savePlaylists(Playlists);
	}

	//--------------------------------------------------//
	//--------------- ExecutionPlaylist ----------------//
	//--------------------------------------------------//

	bool ActionsManager::isExecuting() const
	{
		return m_ExecutionState.b_Running || m_ExecutionPlaylist.isActive();
	}

	bool ActionsManager::clearExecutionPlaylist()
	{
		const std::lock_guard<std::mutex> playlistLock(g_ActionMutex);
		m_ExecutionPlaylist.removeAllActions();
		return true;
	}

	bool ActionsManager::restartExecutionPlaylist()
	{
		const std::lock_guard<std::mutex> playlistLock(g_ActionMutex);
		m_ExecutionPlaylist.restart();
		return true;
	}

	bool ActionsManager::executePlaylist(const Playlist& playlist)
	{
		const std::lock_guard<std::mutex> playlistLock(g_ActionMutex);

		if (!m_ExecutionPlaylist.isActive())
		{
			m_ExecutionPlaylist.removeAllActions(); // Clear all Actions
			m_ExecutionPlaylist.restart(); // Reset ActiveActionIndex
		}

		// Add all Actions
		for (auto& action : playlist.getActions())
			m_ExecutionPlaylist.addAction(action);

		return true;
	}

	bool ActionsManager::executeAction(const Action& action)
	{
		const std::lock_guard<std::mutex> playlistLock(g_ActionMutex);

		if (!m_ExecutionPlaylist.isActive())
		{
			m_ExecutionPlaylist.removeAllActions(); // Clear all Actions
			m_ExecutionPlaylist.restart(); // Reset ActiveActionIndex
		}

		// Add Action
		m_ExecutionPlaylist.addAction(action);
        return true;
    }

	bool ActionsManager::removeAction(int32_t actionIndex)
	{
		const std::lock_guard<std::mutex> playlistLock(g_ActionMutex);
		m_ExecutionPlaylist.removeAction(actionIndex);
		return true;
	}

    void ActionsManager::killExecution()
    {
		{
			const std::lock_guard<std::mutex> playlistLock(g_ActionMutex);
			m_ExecutionPlaylist.stop(); // Invalidate Playlist b4 stopping the ActiveAction
		}

        stopActiveAction();
	}

	//--------------------------------------------------//
	//----------------- ExecutionOutput ----------------//
	//--------------------------------------------------//

	void ActionsManager::moveBuffer(std::vector<char>& destination)
    {
        if (!g_OutputMutex.try_lock())
            return;

        // Create terminator
        if (destination.empty())
            destination.push_back(0);
        
        const char* bufferRaw = m_Buffer.data();
        size_t bufferSize = strlen(bufferRaw);

        // Insert data into the Destination before the terminator
		destination.insert(destination.end() - 1, bufferRaw, bufferRaw + bufferSize);

		// Clean the data from the Buffer
		std::fill(m_Buffer.begin(), m_Buffer.end(), 0);

        g_OutputMutex.unlock();
    }

	//--------------------------------------------------//
	//----------------- ActionExecution ----------------//
	//--------------------------------------------------//

	void ActionsManager::threadedTick()
    {
        while (!b_Ending)
		{
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Sleep half a second to avoid overusing the Locks

            if (!m_ExecutionState.b_Running)
                maybeStartNextAction();
            
            if (m_ExecutionState.b_Running)
                executeAction();
        }
	}

	void ActionsManager::executeAction()
	{
		std::array<char, 2048> localBuffer;
        for (;;)
		{
			size_t bufferFreeSize = 0; // Check Buffer freeSize
			{
				const std::lock_guard<std::mutex> outputLock(g_OutputMutex);

				const size_t bufferLength = strlen(m_Buffer.data());
				bufferFreeSize = m_Buffer.size() - bufferLength;
			}

            if (bufferFreeSize <= 0) // Buffer is full, retry later
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

			// Read in the LocalBuffer and later copy in the Output
#if CP_WINDOWS
			DWORD readBytes;
			if (ReadFile(m_ExecutionState.m_ProcessOutput, localBuffer.data(), bufferFreeSize - 1, &readBytes, nullptr))
			{
				localBuffer[readBytes] = '\0'; // ReadFile doesnt set Termination char
#elif CP_LINUX
			if (fgets(localBuffer.data(), bufferFreeSize, m_ExecutionState.m_Pipe))
			{
#else
			if (false)
			{
#endif
				const std::lock_guard<std::mutex> outputLock(g_OutputMutex);

				char* bufferRaw = m_Buffer.data();
				const size_t bufferLength = strlen(bufferRaw);
				memcpy(&bufferRaw[bufferLength], localBuffer.data(), bufferFreeSize);
			}
			// Read failed > Action is completed
			else
			{
#if CP_WINDOWS
				CloseHandle(m_ExecutionState.m_ProcessOutput);
#elif CP_LINUX
				fclose(m_ExecutionState.m_Pipe);
#endif

				stopActiveAction();
                break;
			}
		}
	}

	bool ActionsManager::maybeStartNextAction()
    {
        const std::lock_guard<std::mutex> lock(g_ActionMutex);

		if (!m_ExecutionPlaylist.isActive()) // Inactive - Need Restart
			return false;

		m_ExecutionPlaylist.advance();
		if (!m_ExecutionPlaylist.hasActiveAction()) // Execution Completed
            return false;
        
        const Action& nextAction = m_ExecutionPlaylist.getActiveAction();
        return startAction(nextAction);
    }

	bool ActionsManager::startAction(const Action& action)
	{
#if CP_WINDOWS
		// Setup Pipes
		bool bSuccess = true;
		SECURITY_ATTRIBUTES securityAttributes;
		securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
		securityAttributes.bInheritHandle = true;
		securityAttributes.lpSecurityDescriptor = nullptr;

		HANDLE& outRead = m_ExecutionState.m_ProcessOutput = nullptr;
        HANDLE outWrite = nullptr;
		bSuccess = CreatePipe(&outRead, &outWrite, &securityAttributes, 0) && SetHandleInformation(outRead, HANDLE_FLAG_INHERIT, 0);
		if (!bSuccess)
			return false;

		PROCESS_INFORMATION& processInformation = m_ExecutionState.m_ProcessInformation;
		memset(&processInformation, 0, sizeof(PROCESS_INFORMATION));

		STARTUPINFO startupInfo;
		memset(&startupInfo, 0, sizeof(STARTUPINFO));
		startupInfo.cb = sizeof(STARTUPINFO);
		startupInfo.hStdError = outWrite;
		startupInfo.hStdOutput = outWrite;
		startupInfo.dwFlags |= STARTF_USESTDHANDLES;

        // Create Job
		m_ExecutionState.m_Job = CreateJobObject(nullptr, L"coffeepot_job");
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobObjectInformation{};
        jobObjectInformation.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
		SetInformationJobObject(m_ExecutionState.m_Job, JobObjectExtendedLimitInformation, &jobObjectInformation, sizeof(jobObjectInformation));

		// Create suspended Process
		USES_CONVERSION;
        const std::string cmd = action.createFullCommand();
		bSuccess = CreateProcess(nullptr, ATL::CA2W(cmd.data()), nullptr, nullptr, true, CREATE_NO_WINDOW | CREATE_SUSPENDED, nullptr, nullptr, &startupInfo, &processInformation);
		if (!bSuccess)
			return false;

		// Assign process to Job and Resume it
		AssignProcessToJobObject(m_ExecutionState.m_Job, processInformation.hProcess);
		ResumeThread(processInformation.hThread);

		CloseHandle(outWrite);

		m_ExecutionState.b_Running = true;
		return true;
#elif CP_LINUX
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
            execl("/bin/sh", "/bin/sh", "-c", cmd.c_str(), nullptr);
            exit(1);
        }
        else
        {
            close(fd[WRITE]);
        }

        m_ExecutionState.m_Pipe = fdopen(fd[READ], "r");
        m_ExecutionState.b_Running = true;
		return true;
#endif
    }

    void ActionsManager::stopActiveAction()
    {
        if (!m_ExecutionState.b_Running)
            return;

        const std::lock_guard<std::mutex> lock(g_ActionMutex);

#if CP_WINDOWS
		m_ExecutionState.b_Running = false;

		TerminateProcess(m_ExecutionState.m_ProcessInformation.hProcess, 0);
		WaitForSingleObject(m_ExecutionState.m_ProcessInformation.hProcess, INFINITE);

		CloseHandle(m_ExecutionState.m_ProcessInformation.hProcess);
		CloseHandle(m_ExecutionState.m_ProcessInformation.hThread);
		CloseHandle(m_ExecutionState.m_Job);
#elif CP_LINUX
		m_ExecutionState.b_Running = false;

        kill(-m_ExecutionState.m_PID, SIGKILL);
        waitpid(m_ExecutionState.m_PID, nullptr, 0);
#endif
    }
}