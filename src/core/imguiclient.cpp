
#include "imguiclient.h"

#include "log.h"
#include "window.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

#include <array>
#include <cstdio>
#include <string>

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
        ImGui::DockSpaceOverViewport();

        if (ImGui::BeginMainMenuBar())
        {
            if (bShowDemo && ImGui::MenuItem("Exec"))
            {
                std::array<char, 128> buffer;
                FILE* pipe = popen("git status", "r");
                while (fgets(buffer.data(), 128, pipe) != nullptr)
                {
                    const size_t eolIndex = std::strcspn(buffer.data(), "\r\n\0");
                    buffer[eolIndex] = '\0';

                    CP_INFO(buffer.data());
                }
                pclose(pipe);
            }

            ImGui::EndMainMenuBar();
        }
    }
    
    void ImGuiClient::postTick()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        const ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backupCurrentContext = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backupCurrentContext);
        }
    }
}
