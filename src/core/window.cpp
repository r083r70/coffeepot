
#include "window.h"

#include "events.h"
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
        
        glfwSetWindowUserPointer(m_Handle, &m_User);
        glfwMakeContextCurrent(m_Handle);
        setupEventCallbacks();

        const int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        return status;
    }

    void Window::deinit()
    {
        // Remove callbacks
        glfwSetCharCallback(m_Handle, nullptr);
        glfwSetWindowSizeCallback(m_Handle, nullptr);
        glfwSetWindowCloseCallback(m_Handle, nullptr);
        
        glfwSetWindowUserPointer(m_Handle, nullptr);
        glfwDestroyWindow(m_Handle);
        m_Handle = nullptr;

        glfwTerminate();
    }

    void Window::tick()
    {
        glfwPollEvents();
        glfwSwapBuffers(m_Handle);
    }

    void Window::getSize(int& width, int& height) const
    {
        glfwGetWindowSize(m_Handle, &width, &height);
    }

    void Window::setupEventCallbacks()
    {
        glfwSetWindowCloseCallback(getHandle(), [](GLFWwindow* window)
        {
            User& user = *static_cast<User*>(glfwGetWindowUserPointer(window));
            WindowClosedEvent event{};
            user.onEvent(event);
        });

        glfwSetWindowSizeCallback(getHandle(), [](GLFWwindow* window, int width, int height)
        {
            User& user = *static_cast<User*>(glfwGetWindowUserPointer(window));
            WindowResizedEvent event{width, height};
            user.onEvent(event);
        });

        glfwSetCharCallback(getHandle(), [](GLFWwindow* window, unsigned int codepoint)
        {
            User& user = *static_cast<User*>(glfwGetWindowUserPointer(window));
            CharEvent event{codepoint};
            user.onEvent(event);
        });
    }
}