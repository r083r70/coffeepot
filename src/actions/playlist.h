#pragma once

#include "action.h"
#include <vector>

namespace coffeepot
{
    class Playlist
    {
    public:
        void addAction(const Action& action);
        void removeAllAction();

        bool startExecution();

        bool hasNextAction() const;
        const Action& getNextAction();

    private:
        std::vector<Action> m_Actions;
        size_t m_NextActionIndex;
    };
}