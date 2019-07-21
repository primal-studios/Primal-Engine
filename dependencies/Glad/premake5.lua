project "Glad"
    kind "StaticLib"
    language "C"
    
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.c"
    }

    includedirs
    {
        "include"
    }
    
	filter "system:windows"
        cdialect "C11"
        systemversion "latest"
        staticruntime "on"

		defines 
		{ 
            "_CRT_SECURE_NO_WARNINGS"
		}

    filter "system:linux"
        buildoptions "-std=c11" 
        systemversion "latest"
        staticruntime "on"

    filter "configurations:Debug"
        runtime "Debug"

    filter "configurations:Release"
        runtime "Release"

    filter "configurations:Dist"
        runtime "Release"