project "multiprecision"
    kind "StaticLib"
    language "C++"

    files {
        "include/**.hpp"
    }

    includedirs {
        "include"
    }