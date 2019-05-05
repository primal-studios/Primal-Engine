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
    include "Projects/Engine"
    include "Projects/Tests"

    -- Dependencies
    include "dependencies/GLFW"
    include "dependencies/Glad"

    -- Directories
    IncludeDir = {}
    IncludeDir["GLFW"] = "%{sln.location}/dependencies/GLFW/include"
    IncludeDir["Glad"] = "%{sln.location}/dependencies/Glad/include"
    IncludeDir["tbb"] = "%{sln.location}/dependencies/tbb/include"
    IncludeDir["spdlog"] = "%{sln.location}/dependencies/spdlog/include"
    IncludeDir["glm"] = "%{sln.location}/dependencies/glm"
    IncludeDir["assimp"] = "%{sln.location}/dependencies/assimp/include"
    IncludeDir["FreeImage"] = "%{sln.location}/dependencies/FreeImage/include"
    IncludeDir["catch"] = "%{sln.location}/dependencies/catch2/include"

    LibDir = {}
    LibDir["assimp"] = "%{sln.location}/dependencies/assimp/lib"
    LibDir["FreeImage"] = "%{sln.location}/dependencies/FreeImage/lib"
    LibDir["tbb"] = "%{sln.location}/dependencies/tbb/lib/tbb_x64"

    BinDir = {}
    BinDir["assimp"] = "%{sln.location}/dependencies/assimp/bin"
    BinDir["FreeImage"] = "%{sln.location}/dependencies/FreeImage/bin"


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