workspace "Pi-Plotter"
    configurations {
        "Debug",
        "Release"
    }
    startproject "Pi-Plotter"

outputdir = "/BIN/%{cfg.buildcfg}/%{cfg.architecture}/"
-- get current working directory
cwd = os.getcwd()

targetdir(cwd .. outputdir .. "%{prj.name}/bin")
objdir(cwd .. outputdir .. "%{prj.name}/bin-int")

filter "system:windows"
    platforms { "x64", "x86" }
    defines "WINDOWS"
filter "system:linux"
    platforms "x64"
    defines "LINUX"

filter { "platforms:x64" }
    architecture "x64"
    defines "X64"
filter { "platforms:x86" }
    architecture "x86"
    defines "X86"

filter { "configurations:Debug" }
    runtime "Debug"
    symbols "on"
    optimize "off"
    defines "DEBUG"
filter { "configurations:Release" }
    runtime "Release"
    symbols "off"
    optimize "Speed"
    defines "RELEASE"
filter {}

-- only for visual studio
flags {
    "MultiProcessorCompile"
}
defines {
    "BOOST_MP_STANDALONE",
    "BOOST_MATH_STANDALONE"
}
staticruntime "on"
removeunreferencedcodedata "on"

include "Pi-Plotter"
include "Dependencies/math"
include "Dependencies/GLFW"
include "Dependencies/ImGui"
include "Dependencies/implot"
include "Dependencies/multiprecision"
include "Dependencies/nativefiledialog"