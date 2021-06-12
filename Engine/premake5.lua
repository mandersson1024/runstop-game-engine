project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	floatingpoint "fast"
	staticruntime "off"
	exceptionhandling "off"
	characterset "ASCII"

	targetdir (outputdir .. "/bin/%{prj.name}")
	objdir (outputdir .. "/obj/%{prj.name}")

	pchheader "pch.h"
	pchsource "src/pch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"src/**.ixx",
		"third_party/**.h",
		"third_party/**.hpp",
		"third_party/**.cpp",
		"third_party/**.inl",
		"assets/code/**.cpp",
	}

	includedirs
	{
		"src",
		"third_party",
		"assets/code",
	}

	libdirs 
	{ 
		"../lib"
	}

	links
	{
	}

	defines { "WIN32", "SPDLOG_NO_EXCEPTIONS" }

	filter "files:src/**.ixx"
		flags { "NoPCH" }

	filter "files:third_party/**.cpp"
		flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines { "_DEBUG", "_WINDOWS", "ENGINE_DEBUG" }
		runtime "Debug"
		symbols "on"

	filter "configurations:Single-Thread Debug"
		defines { "_DEBUG", "_WINDOWS", "ENGINE_DEBUG", "ENGINE_SINGLE_THREAD" }
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines { "NDEBUG", "_WINDOWS", "ENGINE_RELEASE" }
		runtime "Release"
		optimize "on"

