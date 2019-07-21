-- Main Premake
workspace "Primal"
    configurations {
        "Debug",
        "Release",
        "Dist"
    }

    architecture "x64"

    sln = solution()
    outputdir = "%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}"
    
    startproject "Tests"

    -- Projects
    include "projects/Engine"
    include "projects/Tests"

    -- Dependencies
    include "dependencies/Glad"
    include "dependencies/GLFW"

    -- Directories
    IncludeDir = {}
    IncludeDir["catch"] = "%{sln.location}/dependencies/catch2/include"
    IncludeDir["fxgltf"] = "%{sln.location}/dependencies/fxgltf/include"
    IncludeDir["GLFW"] = "%{sln.location}/dependencies/GLFW/include"
    IncludeDir["glm"] = "%{sln.location}/dependencies/glm/include"
    IncludeDir["json"] = "%{sln.location}/dependencies/json/include"
    IncludeDir["phonon"] = "%{sln.location}/dependencies/phonon/include"
    IncludeDir["physx"] = "%{sln.location}/dependencies/physx/include"
    IncludeDir["physxinternal"] = "%{sln.location}/dependencies/physx/include/physx"
    IncludeDir["pxshared"] = "%{sln.location}/dependencies/physx/pxshared/include"
    IncludeDir["spdlog"] = "%{sln.location}/dependencies/spdlog/include"
    IncludeDir["stb"] = "%{sln.location}/dependencies/stb/include"
    IncludeDir["tbb"] = "%{sln.location}/dependencies/tbb/include"
    IncludeDir["Glad"] = "%{sln.location}/dependencies/Glad/include"

    LibDir = {}
    LibDir["phonon"] = "%{sln.location}/dependencies/phonon/lib"
    LibDir["physx"] = "%{sln.location}/dependencies/physx/lib"
    LibDir["tbb"] = "%{sln.location}/dependencies/tbb/lib"

    BinDir = {}
    BinDir["phonon"] = "%{sln.location}/dependencies/phonon/bin"
    BinDir["physx"] = "%{sln.location}/dependencies/physx/bin"
    BinDir["tbb"] = "%{sln.location}/dependencies/tbb/bin"

-- Clean Function
newaction {
    trigger     = "clean",
    description = "clean premake generated files",
    execute     = function ()
       print("cleaning...")

       os.rmdir("./bin-int")
       os.rmdir("./bin")

       os.remove("Primal.sln")
       os.remove("Makefile")

       os.remove("projects/Engine/Makefile")
       os.remove("projects/Engine/Engine.vcxproj")
       os.remove("projects/Engine/Engine.vcxproj.filters")
       os.remove("projects/Engine/Engine.vcxproj.user")

       os.remove("projects/Tests/Makefile")
       os.remove("projects/Tests/Tests.vcxproj")
       os.remove("projects/Tests/Tests.vcxproj.filters")
       os.remove("projects/Tests/Tests.vcxproj.user")

       print("done cleaning.")
    end
 }