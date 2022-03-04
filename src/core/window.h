#pragma once

#include "events.h"

struct GLFWwindow;

namespace coffeepot
{
    class Window
    {
    public:
        bool init(const char* title, int width, int height);
        void deinit();

        void tick();

        inline GLFWwindow* getHandle() const { return m_Handle; }
        inline void setEventCallback(EventCallbackFn callback) { m_User.onEvent = callback; }
        
        void getSize(int& width, int& height) const;
    
    private:
        void setupEventCallbacks();

    private:
        struct User {
            EventCallbackFn onEvent;
        };

        GLFWwindow *m_Handle;
        User m_User;
    };
}