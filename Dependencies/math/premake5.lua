project "math"
    kind "StaticLib"
    language "C++"

    files {
        "include/**.hpp"
    }

    includedirs {
        "include"
    }