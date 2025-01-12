project "GettingStarted"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	targetdir ("../bin/%{prj.name}")
	objdir ("../obj/%{prj.name}")
	files {
		"src/*.h",
		"src/*.cpp",
		"../vendor/glad/src/glad.c",
		"res/*.glsl",
		"res/*.png",
	}
	includedirs {
		"../vendor/glfw/include",
		"../vendor/glad/include",
		"../vendor/stb",
		"../vendor/glm",
	}
	vpaths {
		["Source Files"] = { "**.cpp", "**.c" },
		["Header Files"] = "**.h",
	}
	libdirs {
		"../vendor/glfw/lib-vc2022",
	}
	links {
		"glfw3",
	}