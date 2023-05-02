workspace "Pi-Plotter"
    configurations {
        "Debug",
        "Release"
    }
    startproject "Pi-Plotter"

outputdir = "/BIN/%{cfg.toolset}/%{cfg.shortname}/%{prj.name}/"
cwd = os.getcwd() -- get current working directory

targetdir(cwd .. outputdir .. "bin")
objdir(cwd .. outputdir .. "bin-int")

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
filter { "toolset:gcc* or toolset:clang*" }
    buildoptions "-static"
    linkoptions "-static"
filter {}

flags "MultiProcessorCompile" -- only for visual studio
rtti "off"
staticruntime "on" -- doesn't work for gcc hence the build/link-options above
removeunreferencedcodedata "on"

include "Pi-Plotter"
include "Dependencies/GLFW"
include "Dependencies/ImGui"
include "Dependencies/implot"
include "Dependencies/nativefiledialog"