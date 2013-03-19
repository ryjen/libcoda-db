project "arg3db"
    kind "StaticLib"
    files {
        "*.cpp",
        "*.h"
    }
    excludes {
        "*.test.cpp"
    }