
#include "imguibase.h"
#include "log.h"

#include "imgui.h"
#include "GLFW/glfw3.h"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

void coffeepot::imguibase::init(GLFWwindow* w)
{
    ImGui::CreateContext();
    ImGui::StyleColorsClassic();

    ImGuiIO& IO = ImGui::GetIO();
    IO.DisplaySize.x = 500;
    IO.DisplaySize.y = 500;

    int r = ImGui_ImplGlfw_InitForOpenGL(w, true);
    CP_DEBUG("ImGui_ImplGlfw_InitForOpenGL {0}", r);

    r = ImGui_ImplOpenGL3_Init("#version 300 es");
    CP_DEBUG("ImGui_ImplOpenGL3_Init {0}", r);
}

void coffeepot::imguibase::tick()
{
    ImGuiIO &IO = ImGui::GetIO();
    IO.DisplaySize.x = 500;
    IO.DisplaySize.y = 500;
    IO.DeltaTime = 1.0f / 60.0f;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    bool bShow = true;
    ImGui::ShowDemoWindow(&bShow);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}
