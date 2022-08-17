require "vscode"

workspace "coffeepot"
    architecture "x86_64"
    configurations { "Debug", "Release" }
    startproject "coffeepot"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "thirdparty/glfw"
include "thirdparty/glad"
include "thirdparty/imgui"
include "thirdparty/yaml-cpp"

project "coffeepot"
    filter "Debug"
        kind "ConsoleApp"

    filter "Release"
        kind "WindowedApp" -- Applicable only to Windows

    language "C++"
    cppdialect "C++17"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    includedirs {
        "src/",
        "thirdparty/glad/include",
        "thirdparty/glfw/include",
        "thirdparty/imgui",
        "thirdparty/spdlog/include",
        "thirdparty/yaml-cpp/include"
    }

    links {
        "glfw",
        "glad",
        "imgui",
		"yaml-cpp"
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

    filter "Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "Release"
        defines { "NDEBUG" }
        optimize "On"

    filter "system:windows"
        systemversion "latest"
        defines { "CP_WINDOWS", "YAML_CPP_STATIC_DEFINE" }

        includedirs { "res/" }
        files {
            "res/coffeepot.rc",
            "res/resources.h",
            'res/**.ico'
        }

    filter "system:linux"
        systemversion "latest"
        links { "pthread", "dl" }
        defines { "CP_LINUX" }
