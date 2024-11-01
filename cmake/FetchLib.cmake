macro(FetchLib)
    FetchContent_Declare(
        rapidcsv GIT_REPOSITORY https://github.com/d99kris/rapidcsv.git
        GIT_TAG v8.82
    )
    FetchContent_MakeAvailable(rapidcsv)

    FetchContent_Declare(
        magic_enum GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
        GIT_TAG v0.9.6
    )
    FetchContent_MakeAvailable(magic_enum)

    FetchContent_Declare(
        json GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
    )
    FetchContent_MakeAvailable(json)
endmacro(FetchLib)
