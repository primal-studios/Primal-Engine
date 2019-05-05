-- Engine Premake
project "Engine"
    kind "StaticLib"
    language "C++"

    local targetDirectory = "%{sln.location}\\bin"
    local objectDirectory = "%{sln.location}\\bin-int\\"

    targetdir(targetDirectory)
    objdir(objectDirectory)

    files {
        "include/**.h",
        "src/**.cpp"
    }

    includedirs {
        "include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.tbb}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.assimp}",
        "%{IncludeDir.FreeImage}",
        "%{IncludeDir.catch}"
    }

    libdirs {
        "%{LibDir.assimp}",
        "%{LibDir.FreeImage}",
        "%{LibDir.tbb}"
    }

    bindirs {
        "%{BinDir.assimp}",
        "%{BinDir.FreeImage}"
    }

    links {
        "GLFW",
        "Glad",
        "assimp",
        "freeimage"
    }

    defines {
        "GLFW_INCLUDE_NONE"
    }

    disablewarnings {
		"4005"
    }

    postbuildcommands {
        "{COPY} %{sln.location}/dependencies/FreeImage/bin/FreeImage.dll " .. targetDirectory
    }

    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"

        ignoredefaultlibraries {
            "LIBCMT",
            "LIBCMTD"
        }

        linkoptions {
            "/ignore:4006",
            "/ignore:4221",
            "/ignore:4099",
            "/ignore:4075"
        }

        defines {
            "PRIMAL_PLATFORM_WINDOWS"
        }

        links {
            "opengl32.lib"
        }

    filter "system:linux"
        buildoptions {
            "-std=c++17",
            "-fPIC"
        }

        defines {
            "PRIMAL_PLATFORM_LINUX"
        }

        libdirs {
            "/usr/lib/x86_64-linux-gnu"
        }

        links {
            "GL"
        }

    filter "configurations:Debug"
        defines { 
            "PRIMAL_DEBUG",
            "PRIMAL_ENABLE_ASSERTS"
        }

        staticruntime "Off"
        runtime "Debug"

        symbols "On"

        links {
            "tbb_debug",
            "tbbmalloc_debug",
            "tbbmalloc_proxy_debug"
        }

    filter "configurations:Release"
        defines { 
            "PRIMAL_RELEASE" 
        }

        staticruntime "Off"
        runtime "Release"

        optimize "On"

        links {
            "tbb",
            "tbbmalloc",
            "tbbmalloc_proxy"
        }

    filter "configurations:Dist"
        defines {
            "PRIMAL_DIST"
        }

        staticruntime "Off"
        runtime "Release"

        optimize "On"

        links {
            "tbb",
            "tbbmalloc",
            "tbbmalloc_proxy"
        }