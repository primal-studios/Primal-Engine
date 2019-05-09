-- Game Premake
project "Tests"
    kind "ConsoleApp"
    language "C++"

    local targetDirectory = "%{sln.location}\\bin"
    local objectDirectory = "%{sln.location}\\bin-int\\"

    targetdir(targetDirectory)
    objdir(objectDirectory)
    debugdir("%{sln.location}")

    dependson {
        "GLFW",
        "Engine",
        "vma"
    }

    files {
        "include/**.h",
        "src/**.cpp"
    }

    includedirs {
        "include",
        "%{sln.location}\\projects\\Engine\\include",
        "%{IncludeDir.assimp}",
        "%{IncludeDir.catch}",
        "%{IncludeDir.Eigen}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.phonon}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.tbb}",
        "%{IncludeDir.vma}",
        "%{IncludeDir.vulkan}",
    }

    libdirs {
        "%{LibDir.assimp}",
        "%{LibDir.phonon}",
        "%{LibDir.tbb}",
        "%{LibDir.vulkan}"
    }

    bindirs {
        "%{BinDir.assimp}",
        "%{BinDir.phonon}",
        "%{BinDir.tbb}",
        "%{BinDir.vulkan}"
    }

    links {
        "Engine",
        "assimp-vc140-mt",
        "GLFW",
        "phonon",
        "vma",
        "vulkan-1"
    }

    defines {
    	"GLFW_INCLUDE_NONE"
	}

	disablewarnings {
		"4005"
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