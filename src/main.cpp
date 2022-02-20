#include "core/log.h"
#include "core/imguibase.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

int main(void)
{
    coffeepot::log::init();
    CP_DEBUG("log::init");

    glfwInit();
    CP_DEBUG("glfwInit");

    auto tt = "Hello world!";
    auto window = glfwCreateWindow(500, 500, tt, nullptr, nullptr);
    CP_DEBUG("glfwCreateWindow");

    glfwMakeContextCurrent(window);
    CP_DEBUG("glfwMakeContextCurrent");

    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    CP_DEBUG("gladLoadGLLoader: {0}", status);

    coffeepot::imguibase::init(window);
    CP_DEBUG("imguibase::init");

    while (true)
    {
        glfwPollEvents();
        glfwSwapBuffers(window);

        coffeepot::imguibase::tick();
    }

    return 0;
}