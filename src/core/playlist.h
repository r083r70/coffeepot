#pragma once

#include "action.h"
#include <vector>

namespace coffeepot
{
    class Playlist
    {
	public:
        Playlist() = default;

		const std::string& getName() const { return m_Name; }
		void setName(const std::string& name) { m_Name = name; }

        void addAction(const Action& action);
		void removeAction(int32_t actionIndex);
		void removeAllActions();

		const size_t getActionsCount() const { return m_Actions.size(); }
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
		void addAction(const Action& action) { m_Playlist.addAction(action); }

		void removeAction(int32_t actionIndex);
		void removeAllActions();

		void restart();
		void stop();

		void advance();

		bool isValidForExecution() const;
		bool isActive() const;

		int32_t getActiveActionIndex() const { return m_ActiveActionIndex; }
		int32_t getLastCompletedActionIndex() const { return m_LastCompletedActionIndex; }

		const Action& getActiveAction() const;

	protected:
		Playlist m_Playlist;

		int32_t m_ActiveActionIndex = 0; // This will always be Positive [0, m_Actions.size]
		int32_t m_LastCompletedActionIndex = -1; // -1 when no Action has been completed yet
	};
}