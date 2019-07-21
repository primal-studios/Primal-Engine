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
        "Glad",
        "Engine",
    }

    files {
        "include/**.h",
        "src/**.cpp"
    }

    includedirs {
        "include",
        "%{sln.location}\\projects\\Engine\\include",
        "%{IncludeDir.catch}",
        "%{IncludeDir.fxgltf}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.json}",
        "%{IncludeDir.phonon}",
        "%{IncludeDir.physx}",
        "%{IncludeDir.physxinternal}",
        "%{IncludeDir.pxshared}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.tbb}",
        "%{IncludeDir.Glad}",
    }

    libdirs {
        "%{LibDir.phonon}",
        "%{LibDir.tbb}",
		targetDirectory
    }

    bindirs {
        "%{BinDir.phonon}",
        "%{BinDir.tbb}",
    }

    links {
        "Engine",
        "GLFW",
        "Glad",
        "phonon",
        "PhysXTask_static_64",
		"PhysX_64",
		"PhysXCommon_64",
		"PhysXExtensions_static_64",
		"PhysXPvdSDK_static_64",
		"PhysXCooking_64",
		"PhysXCharacterKinematic_static_64",
		"SceneQuery_static_64",
		"SimulationController_static_64",
		"PhysXFoundation_64",
    }

    defines {
    	"GLFW_INCLUDE_NONE",
        "GLM_FORCE_DEPTH_ZERO_TO_ONE"
	}

	disablewarnings {
		"4005"
    }
    
    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"
        staticruntime "Off"

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
        staticruntime "Off"
    
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

        libdirs {
            "%{LibDir.physx}" .. "/debug/"
        }

        bindirs {
            "%{BinDir.physx}" .. "/debug/"
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
            "PRIMAL_RELEASE",
			"NDEBUG"
        }

        libdirs {
            "%{LibDir.physx}" .. "/release/"
        }

        bindirs {
            "%{BinDir.physx}" .. "/release/"
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
            "PRIMAL_DIST",
			"NDEBUG"
        }

        libdirs {
            "%{LibDir.physx}" .. "/release/"
        }

        bindirs {
            "%{BinDir.physx}" .. "/release/"
        }

        staticruntime "Off"
        runtime "Release"

        optimize "On"

        links {
            "tbb",
            "tbbmalloc",
            "tbbmalloc_proxy"
        }