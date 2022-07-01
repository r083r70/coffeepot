#pragma once

#include "action.h"
#include <vector>

namespace coffeepot
{
    class Playlist
    {
	public:
        Playlist() = default;

        const std::vector<Action>& getActions() const { return m_Actions; }
        void addAction(const Action& action);
        void removeAllAction();

		size_t getNextActionIndex() const { return m_NextActionIndex; }
        bool hasNextAction() const;
		const Action& getNextAction();

		bool startExecution();

    private:
        std::vector<Action> m_Actions;
        size_t m_NextActionIndex;
    };
}