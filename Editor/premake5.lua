project "Editor"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++20"
	floatingpoint "fast"
	staticruntime "off"
	exceptionhandling "off"
	characterset "ASCII"

	targetdir (outputdir .. "/bin/%{prj.name}")
	objdir (outputdir .. "/obj/%{prj.name}")
	
	files
	{
		"src/**.h",
		"src/**.cpp",
		"src/**.ixx",
		"third_party/**.h",
		"third_party/**.cpp",
	}

	includedirs
	{
		"src",
		"third_party",
		"%{wks.location}/Engine/src",
		"%{wks.location}/Engine/third_party",
		"%{wks.location}/Engine/assets/code",
	}

	libdirs { "../lib/**" }

	linkoptions { "/DEBUG:FULL" }

	postbuildcommands 
	{ 
		"xcopy \"%{wks.location}/Engine/assets\" \"%{cfg.targetdir}/assets\" /siy",
	}

	defines { "WIN32", "SPDLOG_NO_EXCEPTIONS" }

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		defines { "_DEBUG", "_WINDOWS", "ENGINE_DEBUG", "EDITOR" }
		linkoptions { "-IGNORE:4099" } -- assimp link warning
		links
		{
			"d3d11",
			"dxgi",
			"dxguid",
			"d3dcompiler",
			"IrrXMLd",
			"zlibstaticd",
			"assimp-vc142-mtd",
			"Engine",
		}

	filter "configurations:Single-Thread Debug"
		runtime "Debug"
		symbols "on"
		defines { "_DEBUG", "_WINDOWS", "ENGINE_DEBUG", "ENGINE_SINGLE_THREAD", "EDITOR" }
		linkoptions { "-IGNORE:4099" } -- assimp link warning
		links
		{
			"d3d11",
			"dxgi",
			"dxguid",
			"d3dcompiler",
			"IrrXMLd",
			"zlibstaticd",
			"assimp-vc142-mtd",
			"Engine",
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		defines { "NDEBUG", "_WINDOWS", "ENGINE_RELEASE", "EDITOR" }
		links
		{
			"d3d11",
			"dxgi",
			"d3dcompiler",
			"IrrXML",
			"zlibstatic",
			"assimp-vc142-mt",
			"Engine",
		}

