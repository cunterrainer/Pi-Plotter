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
    platforms "x64"
    defines "WINDOWS"
filter "system:linux"
    platforms "x64"
    defines "LINUX"

architecture "x64"
defines "X64"

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
    defines "NDEBUG"
filter {}

-- only for visual studio
flags {
    "MultiProcessorCompile"
}
staticruntime "on"
removeunreferencedcodedata "on"

include "Pi-Plotter"
include "Dependencies/GLFW"
include "Dependencies/ImGui"
include "Dependencies/implot"
include "Dependencies/nativefiledialog"