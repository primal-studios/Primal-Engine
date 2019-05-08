-- vma Premake
project "vma"
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
        "%{IncludeDir.vulkan}",
    }

    libdirs {
        "%{LibDir.vulkan}"
    }

    bindirs {
        "%{BinDir.vulkan}"
    }

    links {
        "vulkan-1"
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

    filter "configurations:Release"
        defines { 
            "PRIMAL_RELEASE" 
        }

        staticruntime "Off"
        runtime "Release"

        optimize "On"

    filter "configurations:Dist"
        defines {
            "PRIMAL_DIST"
        }

        staticruntime "Off"
        runtime "Release"

        optimize "On"