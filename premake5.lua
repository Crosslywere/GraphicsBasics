workspace "Graphics Basics"
	platforms "x64"
	configurations { "Debug", "Release" }

    IncludeDirs = {}
    IncludeDirs["GLFW"] = "%{wks.location}/vendors/glfw/include"
    IncludeDirs["GLAD"] = "%{wks.location}/vendors/glad/include"
    IncludeDirs["STB"] = "%{wks.location}/vendors/stb"
    IncludeDirs["GLM"] = "%{wks.location}/vendors/glm"

	include "GettingStarted"
    include "Lighting"

    project "glfw"
        kind "StaticLib"
        language "C"
        cdialect "C17"
        targetdir "bin/libs"
        objdir "obj/%{prj.name}"
        files {
            "vendors/glfw/src/**.c",
            "vendors/glfw/include/**.h"
        }
        vpaths {
            ["Source Files"] = "**.c",
            ["Header Files"] = "**.h"
        }
        filter "system:windows"
            defines {
                "_GLFW_WIN32",
                "_CRT_SECURE_NO_WARNINGS"
            }
        filter "configurations:Debug"
            defines "DEBUG"
            symbols "On"
        filter "configurations:Release"
            defines "NDEBUG"
            optimize "On"
