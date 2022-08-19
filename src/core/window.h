#pragma once

#include "eventdispatcher.h"

struct GLFWwindow;

namespace coffeepot
{
    class Window : public EventSubscriber
    {
    public:
        bool init(const char* title, int width, int height);
        void deinit();

        void tick();

        inline GLFWwindow* getHandle() const { return m_Handle; }
        void getSize(int& width, int& height) const;

		virtual bool onEvent(const Event& event) override;
    
    private:
        void setupEventCallbacks();

		void hide();
		void show();

    private:
        GLFWwindow *m_Handle;
    };
}