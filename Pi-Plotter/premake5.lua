project "Pi-Plotter"
    language "C++"
    cppdialect "C++17"
    defines "_CRT_SECURE_NO_WARNINGS"

    files {
        "src/**.cpp",
        "src/**.h",
        "vendor/**.cpp",
        "vendor/**.h"
    }

    includedirs {
        "src",
        "vendor",
        "../Dependencies/gmp",
        "../Dependencies/pi-data",
        "../Dependencies/mpfr/src",
        "../Dependencies/GLFW/include",
        "../Dependencies/ImGui/include",
        "../Dependencies/implot/include",
        "../Dependencies/nativefiledialog/include"
    }

    externalincludedirs {
        "vendor",
        "../Dependencies/gmp",
        "../Dependencies/mpfr/src",
        "../Dependencies/ImGui/include"
    }

    libdirs "../Dependencies/pi-data"

    flags "FatalWarnings"

    links {
        "glfw",
        "nativefiledialog",
        "ImGui",
        "ImPlot",
        "gmp",
        "mpfr",
        "pi-data"
    }

    filter "system:windows"
        links {
            "gdi32",
            "opengl32",
            "shell32",
            "ole32",
            "uuid"
        }

    filter "system:linux"
        links {
            "GL",
            "gtk-3",
            "glib-2.0",
            "gobject-2.0",
            "X11"
        }

    --gcc* clang* msc*
    filter "toolset:msc*"
        warnings "High"
        externalwarnings "Off"
        disablewarnings {}
        buildoptions { "/sdl" }
        defines "MSC"

    filter { "toolset:msc* or toolset:clang*" }
        libdirs {
            "../Dependencies/gmp/libs/clang",
            "../Dependencies/mpfr/libs/clang"
        }

    filter { "toolset:gcc* or toolset:clang*" }
        enablewarnings {
            "cast-align",
            "disabled-optimization",
            "format=2",
            "init-self",
            "missing-include-dirs",
            "overloaded-virtual",
            "redundant-decls",
            "shadow",
            "sign-promo",
            "uninitialized",
            "unreachable-code",
            "unused",
            "alloca",
            "format-security",
            "null-dereference",
            "stack-protector",
            "vla",
            "shift-overflow"
        }

    filter "toolset:gcc*"
        warnings "Extra"
        externalwarnings "Off"
        linkgroups "on" -- activate position independent linking
        enablewarnings {
            "noexcept",
            "strict-null-sentinel",
            "array-bounds=2",
            "duplicated-branches",
            "duplicated-cond",
            "logical-op",
            "arith-conversion",
            "stringop-overflow=4",
            "implicit-fallthrough=3",
            "trampolines"
        }
        disablewarnings "cast-function-type"
        defines "GCC"
        libdirs {
            "../Dependencies/gmp/libs/gcc",
            "../Dependencies/mpfr/libs/gcc"
        }

    filter "toolset:clang*"
        warnings "Extra"
        externalwarnings "Everything"
        enablewarnings {
            "array-bounds",
            "long-long"
        }
        disablewarnings {"cast-align", "sign-conversion"}
        defines "CLANG"
    filter {}


    filter { "configurations:Debug" }
        kind "ConsoleApp"
    filter { "configurations:Release" }
        kind "ConsoleApp"
        --kind "WindowedApp"
        --entrypoint "mainCRTStartup"