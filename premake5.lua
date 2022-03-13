require "vscode"

workspace "coffeepot"
    architecture "x86_64"
    configurations { "Debug", "Release" }
    startproject "coffeepot"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "thirdparty/glfw"
include "thirdparty/glad"
include "thirdparty/imgui"

project "coffeepot"
    kind "ConsoleApp"
    language "C++"
    cppdialect "c++17"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    includedirs {
        "thirdparty/spdlog/include",
        "thirdparty/glfw/include",
        "thirdparty/glad/include",
        "thirdparty/imgui",
        "src/"
    }

    links {
        "glfw",
        "glad",
        "imgui"
    }

    files {
        "src/**.h",
        "src/**.cpp",

        -- ImGui files
        "thirdparty/imgui/backends/imgui_impl_glfw.h",
        "thirdparty/imgui/backends/imgui_impl_glfw.cpp",
        "thirdparty/imgui/backends/imgui_impl_opengl3.h",
        "thirdparty/imgui/backends/imgui_impl_opengl3.cpp"
    }

    filter "system:windows"
        systemversion "latest"
        defines { "CP_WIN" }

    filter "system:linux"
        systemversion "latest"
        links { "pthread", "dl" }
        defines { "CP_LINUX" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

