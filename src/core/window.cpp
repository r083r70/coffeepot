
#include "window.h"

#include "log.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace coffeepot
{
    bool Window::init(const char* title, int width, int height)
    {
        glfwInit();

        m_Handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (m_Handle == nullptr)
            return false;

		m_Visible = true;

		glfwSetWindowUserPointer(m_Handle, this);
        glfwMakeContextCurrent(m_Handle);
        setupEventCallbacks();

        EventDispatcher::get()->subscribe(this);

        const int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        return status;
    }

    void Window::deinit()
    {
        // Remove callbacks
        glfwSetCharCallback(m_Handle, nullptr);
		glfwSetWindowSizeCallback(m_Handle, nullptr);
		glfwSetWindowCloseCallback(m_Handle, nullptr);
		glfwSetWindowIconifyCallback(m_Handle, nullptr);
        
        glfwDestroyWindow(m_Handle);
        m_Handle = nullptr;

        glfwTerminate();
    }

    void Window::tick()
    {
        glfwPollEvents();

        if (m_Visible)
            glfwSwapBuffers(m_Handle);
    }

    void Window::getSize(int& width, int& height) const
    {
        glfwGetWindowSize(m_Handle, &width, &height);
    }

    bool Window::onEvent(const Event& event)
	{
		switch (event.getType())
		{
		case coffeepot::EventType::WindowClosed:
			hide(); break;
		case coffeepot::EventType::NotifyIconInteracted:
            show(); break;
		}

		return false; // Dont consume the Events
	}

	void Window::hide()
	{
		glfwHideWindow(m_Handle);
        m_Visible = false;
	}

	void Window::show()
	{
		glfwShowWindow(m_Handle);
		m_Visible = true;
	}

	void Window::setupEventCallbacks()
	{
		glfwSetWindowIconifyCallback(m_Handle, [](GLFWwindow* window, int iconified)
		{
			static_cast<Window*>(glfwGetWindowUserPointer(window))->m_Visible = iconified != GLFW_TRUE;
		});

		glfwSetWindowCloseCallback(m_Handle, [](GLFWwindow* window)
		{
			EventDispatcher::get()->createEvent(EventType::WindowClosed);
		});

        glfwSetWindowSizeCallback(m_Handle, [](GLFWwindow* window, int width, int height)
		{
			auto eventData = new WindowResizedEventData{ width, height };
            EventDispatcher::get()->createEvent(EventType::WindowResized, eventData);
        });
    }
}