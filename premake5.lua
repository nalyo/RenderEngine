workspace "GameEngine"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Platform specific settings
filter "system:windows"
	system "windows"
	toolset "msc"

filter "system:linux"
	system "linux"
	toolset "gcc"

IncludeDir = {}
IncludeDir["GLFW"] = "GameEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "GameEngine/vendor/Glad/include"
IncludeDir["ImGui"] = "GameEngine/vendor/imgui"
IncludeDir["glm"] = "GameEngine/vendor/glm"
IncludeDir["stb_image"] = "GameEngine/vendor/stb_image"
IncludeDir["entt"] = "GameEngine/vendor/entt"
IncludeDir["yaml_cpp"] = "GameEngine/vendor/yaml_cpp/include"
IncludeDir["ImGuizmo"] = "GameEngine/vendor/ImGuizmo"

include "GameEngine/vendor/GLFW"
include "GameEngine/vendor/Glad"
include "GameEngine/vendor/imgui"
include "GameEngine/vendor/yaml_cpp"


project "GameEngine"
	location "GameEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "gepch.h"
	pchsource "GameEngine/src/gepch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"GameEngine/src",
		"GameEngine/vendor/spdlog-1.x/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}"
	}

	filter "files:GameEngine/vendor/ImGuizmo/**.cpp"
		flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"
		buildoptions { "/utf-8" }

		defines 
		{
			"GE_PLATFORM_WINDOWS",
			"GE_BUILD_DLL",
			"GLFW_INCLUDE_NONE",
			"GE_ENABLE_ASSERTS"
		}
		
		links
		{
			"GLFW",
			"Glad",
			"ImGui",
			"yaml-cpp",
			"opengl32.lib"
		}
	
	filter "system:linux"
		buildoptions { "-std=c++17", "-fPIC" }

		defines 
		{
			"GE_PLATFORM_LINUX",
			"GLFW_INCLUDE_NONE",
			"GE_ENABLE_ASSERTS"
		}
		
		links
		{
			"GLFW",
			"Glad",
			"ImGui",
			"yaml-cpp",
			"GL",
			"pthread",
			"dl"
		}
		
		linkoptions { "-Wl,-rpath,." }
	
	filter "configurations:Debug"
		defines "GE_DEBUG"
		runtime "Debug"
		optimize "on"

	filter "configurations:Release"
		defines "GE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "GE_DST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"GameEngine/vendor/spdlog-1.x/include",
		"GameEngine/src",
		"GameEngine/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}"
	}

	links
	{
		"GameEngine"
	}

	filter "system:windows"
		staticruntime "on"
		systemversion "latest"
		buildoptions { "/utf-8" }

		defines 
		{
			"GE_PLATFORM_WINDOWS"
		}
	
	filter "system:linux"
		buildoptions { "-std=c++17" }

		defines 
		{
			"GE_PLATFORM_LINUX"
		}
	
	filter "configurations:Debug"
		defines "GE_DEBUG"
		runtime "Debug"
		optimize "on"

	filter "configurations:Release"
		defines "GE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "GE_DST"
		runtime "Release"
		optimize "on"