
workspace "RUNSTOP"
	architecture "x86_64"
	startproject "Editor"

	configurations
	{
		"Debug",
		"Single-Thread Debug",
		"Release"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{wks.location}/build/%{cfg.buildcfg}"

include "Engine"
include "Editor"

