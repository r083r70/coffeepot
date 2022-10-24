workspace "coffeepot"
	architecture "x86_64"
	configurations { "Debug", "Release" }
	startproject "coffeepot"

include "kettle/kettleCore"
include "thirdparty/yaml-cpp"

project "coffeepot"
	kind "ConsoleApp"

	language "C++"
	cppdialect "C++17"

	targetdir ("bin/%{cfg.buildcfg}")
	objdir ("obj/%{cfg.buildcfg}" )

	includedirs {
		"res/",
		"src/",
		"kettle/kettle/src",
		"kettle/thirdparty/imgui",
		"thirdparty/yaml-cpp/include"
	}

	links {
		"kettle",
		"yaml-cpp"
	}

	files {
		"src/**.h",
		"src/**.cpp",
		"res/fa_icons.h", -- FontIcons
	}

	filter "Debug"
		symbols "On"

	filter "Release"
		optimize "On"

	filter "system:windows"
		systemversion "latest"
		defines { "CP_WINDOWS" }
		includedirs { "res/" }
		files {
			"res/coffeepot.rc",
			"res/resources.h",
			'res/**.ico'
		}

	filter "system:linux"
		systemversion "latest"
		links { "pthread", "dl" }
		links { "glad", "GLFW", "imgui" }
		defines { "CP_LINUX" }
