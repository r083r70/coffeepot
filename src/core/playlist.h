#pragma once

#include "action.h"
#include <vector>

namespace coffeepot
{
	enum class ActionState
	{
		Ready,
		Running,
		Success,
		Fail,
		Aborted
	};

    class Playlist
    {
	public:
        Playlist() = default;

		const std::string& getName() const { return m_Name; }
		void setName(const std::string& name) { m_Name = name; }

        void addAction(const Action& action);
		void removeAction(int32_t actionIndex);
		void removeAllActions();

		const int32_t getActionsCount() const { return (int32_t)m_Actions.size(); }
		const std::vector<Action>& getActions() const { return m_Actions; }
		std::vector<Action>& getActions() { return m_Actions; }

	protected:
        std::string m_Name;
        std::vector<Action> m_Actions;
    };

	class ExecutionPlaylist
	{
	public:
		const std::vector<Action>& getActions() const { return m_Playlist.getActions(); }
		const std::vector<ActionState>& getActionStates() const { return m_ActionStates; }

		void addAction(const Action& action);

		void removeAction(int32_t actionIndex);
		void removeAllActions();

		void restart();
		void stop();

		void advance();
		void completeAction(bool bSuccess);

		bool isActive() const;

		bool hasActiveAction() const;
		const Action& getActiveAction() const;

	protected:
		Playlist m_Playlist;
		std::vector<ActionState> m_ActionStates;

		int32_t m_ActiveActionIndex = -1;
	};
}