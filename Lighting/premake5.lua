project "Lighting"
    kind "ConsoleApp"
    language "C++"
	cppdialect "C++20"
	targetdir ("../bin/%{prj.name}")
	objdir ("../obj/%{prj.name}")
	files {
		"src/*.h",
		"src/*.cpp",
		"../vendors/glad/src/glad.c",
		"res/*.glsl",
		"res/*.png",
        "res/*.jpg",
	}
	includedirs {
        "%{IncludeDirs.GLFW}",
		"%{IncludeDirs.GLAD}",
		"%{IncludeDirs.STB}",
		"%{IncludeDirs.GLM}",
        "../include",
	}
	vpaths {
		["Source Files"] = { "**.cpp", "**.c" },
		["Header Files"] = "**.h",
        ["Resource Files"] = { "**.glsl", "**.png", "**.jpg" },
	}
	libdirs {
		"../bin/libs",
	}
	links {
		"glfw",
	}
