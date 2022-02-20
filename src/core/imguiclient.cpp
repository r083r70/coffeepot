
#include "imguiclient.h"

#include "log.h"
#include "window.h"

#include "imgui.h"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

#include <chrono>

namespace coffeepot
{
    bool ImGuiClient::init(const Window& window)
    {
        ImGui::CreateContext();
        ImGui::StyleColorsClassic();

        ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        int32_t x, y;
        window.getSize(x, y);
        io.DisplaySize = ImVec2(x, y);

        int status = ImGui_ImplGlfw_InitForOpenGL(window.getHandle(), true);
        status &= ImGui_ImplOpenGL3_Init("#version 300 es");

        m_PrevTickTime = 0;

        return status;
    }

    void ImGuiClient::deinit()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiClient::preTick(int32_t width, int32_t height)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = width;
        io.DisplaySize.y = height;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiClient::tick(bool bShowDemo /*= false*/)
    {
        ImGui::ShowDemoWindow(&bShowDemo);
    }
    
    void ImGuiClient::postTick()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        const ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            //GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            //glfwMakeContextCurrent(backup_current_context);
        }
    }
}
