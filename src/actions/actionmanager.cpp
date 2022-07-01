
#include "actionmanager.h"

#include "core/log.h"
#include "core/serializer.h"

#include "imgui.h"

#include <cassert>
#include <cstring>
#include <mutex>

namespace coffeepot
{
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

		auto& executor = actionManager->m_Executor;
		if (!executor)
		{
			const std::lock_guard<std::mutex> playlistLock(g_PlaylistMutex);
			if (actionManager->m_CurrentPlaylist.hasNextAction())
                actionManager->startNextAction();
		}
        else
        {
			const std::lock_guard<std::mutex> outputLock(g_OutputMutex);
			char* output = actionManager->m_OutputBuffer.data();

            const size_t length = strlen(output);
            const size_t bufferFreeSize = actionManager->m_OutputBuffer.size() - length;

            if (bufferFreeSize > 0 && !executor->update(&output[length], bufferFreeSize))
                executor.reset();
        }

        return true;
	}

	ActionsManager* ActionsManager::m_Instance = nullptr;

	ActionsManager* ActionsManager::get()
    {
        if (!m_Instance)
            m_Instance = new ActionsManager();
        
        return m_Instance;
    }

    bool ActionsManager::init()
    {
        m_ThreadedActionManager = std::thread{ ThreadedActionManager{} };
        return Serializer::loadActions(m_Actions);
    }

    void ActionsManager::deinit()
    {
        b_Terminating = true;
        m_ThreadedActionManager.join();

        m_Actions.clear();
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