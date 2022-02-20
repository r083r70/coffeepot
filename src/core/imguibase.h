#pragma once

class GLFWwindow;

namespace coffeepot
{
    class imguibase
    {
    public:
        static void init(GLFWwindow* w);
        static void tick();
    };
};