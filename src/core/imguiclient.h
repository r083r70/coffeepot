#pragma once

#include "window.h"

namespace coffeepot
{
    class ImGuiClient
    {
    public:
        bool init(const Window& window);
        void deinit();

        void preTick(int32_t width, int32_t height);
        void tick();
        void postTick();

    private:
        time_t m_PrevTickTime;
    };
};