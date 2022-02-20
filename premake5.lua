workspace "coffeepot"
    architecture "x86_64"
    configurations { "Debug", "Release" }
    startproject "coffeepot"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "coffeepot"
    kind "ConsoleApp"
    language "C++"
    cppdialect "c++17"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    includedirs {
        "thirdparty/spdlog/include"
    }

    files { "src/**.h", "src/**.cpp" }

    filter "system:windows"
        systemversion "latest"

    filter "system:linux"
        systemversion "latest"
        links { "pthread", "dl" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

