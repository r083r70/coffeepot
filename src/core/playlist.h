#pragma once

#include "action.h"
#include <vector>

namespace coffeepot
{
    class Playlist
    {
	public:
        Playlist() = default;

        // Actions
        void addAction(const Action& action);
		void removeAction(size_t actionIndex);
		void removeAllActions();

		const std::vector<Action>& getActions() const { return m_Actions; }
		std::vector<Action>& getActions() { return m_Actions; }

		// Execution
		bool startExecution();
		void stopExecution();

        size_t getCurrentActionIndex() const;
        bool hasNextActionForExecution() const;
		const Action& getNextActionForExecution();

    public:
        std::string m_Name;

    private:
        std::vector<Action> m_Actions;
        size_t m_NextActionIndex;
    };
}