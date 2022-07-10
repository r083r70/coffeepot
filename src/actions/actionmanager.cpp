
#include "actionmanager.h"

#include "core/log.h"
#include "core/serializer.h"

#include "imgui.h"

#include <cassert>
#include <cstring>
#include <mutex>

namespace coffeepot
{
	std::mutex g_ExecutorMutex;
	std::mutex g_PlaylistMutex;
	std::mutex g_OutputMutex;

    void ThreadedActionManager::operator()()
	{
        while (update()) ;
    }

	bool ThreadedActionManager::update()
	{
		ActionsManager* actionManager = ActionsManager::get();
        if (actionManager->b_Terminating) // just reading > no lock
            return false;

        const std::lock_guard<std::mutex> executorLock(g_ExecutorMutex);
		auto& executor = actionManager->m_Executor;
		if (!executor)
		{
			const std::lock_guard<std::mutex> playlistLock(g_PlaylistMutex);
			if (actionManager->m_CurrentPlaylist.hasNextAction())
                actionManager->startNextAction();
		}
        else
        {
            size_t bufferFreeSize = 0; // Comput Buffer free size
            {
			    const std::lock_guard<std::mutex> outputLock(g_OutputMutex);
                
                const auto outputBuffer = actionManager->m_OutputBuffer; 
                const size_t outputLength = strlen(outputBuffer.data());
                bufferFreeSize = outputBuffer.size() - outputLength;
            }

            if (bufferFreeSize <= 0) // Buffer is full, retry later
                return true;

            // Update Executor - Gather output
			char* tmpOutput = m_OutputBuffer.data();
            if (executor->update(tmpOutput, bufferFreeSize))
            {
                // Copy output to ActionsManager's Buffer
			    const std::lock_guard<std::mutex> outputLock(g_OutputMutex);

                char* output = actionManager->m_OutputBuffer.data(); 
                const size_t outputLength = strlen(output);
                memcpy(&output[outputLength], tmpOutput, bufferFreeSize);
            }
            else
            {
                executor.reset(); // Invalidate Executor
            }
        }

        return true;
	}

	ActionsManager* ActionsManager::s_Instance = nullptr;

	ActionsManager* ActionsManager::get()
    {
        if (!s_Instance)
            s_Instance = new ActionsManager();
        
        return s_Instance;
    }

    bool ActionsManager::init()
    {
        m_ThreadedActionManager = std::thread{ ThreadedActionManager{} };
        return true;
    }

    void ActionsManager::deinit()
    {
        b_Terminating = true;
        m_ThreadedActionManager.join();
    }

    void ActionsManager::tick() {}

    bool ActionsManager::executeAction(const Action& action)
	{
		const std::lock_guard<std::mutex> playlistLock(g_PlaylistMutex);

        if (!m_Executor)
            m_CurrentPlaylist.removeAllAction(); // reset

        m_CurrentPlaylist.addAction(action);
        return true;
    }

    bool ActionsManager::executePlaylist(const Playlist& playlist)
    {
        const std::lock_guard<std::mutex> executorLock(g_ExecutorMutex);
		const std::lock_guard<std::mutex> playlistLock(g_PlaylistMutex);

        if (m_Executor)
        {
            m_Executor->stop();
            m_Executor.reset();
        }

        m_CurrentPlaylist = playlist;
        return m_CurrentPlaylist.startExecution();
    }

	void ActionsManager::readOutput(ImGuiTextBuffer& textOutput)
    {
		const std::lock_guard<std::mutex> outputLock(g_OutputMutex);
        
        char* output = m_OutputBuffer.data();
        textOutput.append(output); // memcpy the data

		memset(output, 0, strlen(output)); // clean the data
    }

	bool ActionsManager::startNextAction()
    {
        const Action& nextAction = m_CurrentPlaylist.getNextAction();
        m_Executor = std::make_unique<ActionExecutor>(nextAction);
        
        if (m_Executor->start())
            return true;
        
        CP_ERROR("Couldnt start action {}", nextAction.m_Name);
        m_Executor.reset();
        return false;
    }
}