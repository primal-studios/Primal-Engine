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
    include "dependencies/GLFW"
    include "dependencies/vma"

    -- Directories
    IncludeDir = {}
    IncludeDir["assimp"] = "%{sln.location}/dependencies/assimp/include"
    IncludeDir["catch"] = "%{sln.location}/dependencies/catch2/include"
    IncludeDir["Eigen"] = "%{sln.location}/dependencies/Eigen/include"
    IncludeDir["GLFW"] = "%{sln.location}/dependencies/GLFW/include"
    IncludeDir["phonon"] = "%{sln.location}/dependencies/phonon/include"
    IncludeDir["spdlog"] = "%{sln.location}/dependencies/spdlog/include"
    IncludeDir["stb"] = "%{sln.location}/dependencies/stb/include"
    IncludeDir["tbb"] = "%{sln.location}/dependencies/tbb/include"
    IncludeDir["vma"] = "%{sln.location}/dependencies/vma/include"
    IncludeDir["vulkan"] = "%{sln.location}/dependencies/vulkan/include"

    LibDir = {}
    LibDir["assimp"] = "%{sln.location}/dependencies/assimp/lib"
    LibDir["phonon"] = "%{sln.location}/dependencies/phonon/lib"
    LibDir["tbb"] = "%{sln.location}/dependencies/tbb/lib"
    LibDir["vulkan"] = "%{sln.location}/dependencies/vulkan/lib"

    BinDir = {}
    BinDir["assimp"] = "%{sln.location}/dependencies/assimp/bin"
    BinDir["phonon"] = "%{sln.location}/dependencies/phonon/bin"
    BinDir["tbb"] = "%{sln.location}/dependencies/tbb/bin"
    BinDir["vulkan"] = "%{sln.location}/dependencies/vulkan/bin"


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