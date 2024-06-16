file(GLOB private_lib
    modules/*.cpp
    drivers/*.cpp
    threads/*.cpp
    app/*.cpp
    defines/*.cpp
    ../common/defines/*.cpp
    ../common/threading/*.cpp
)

idf_component_register(
    SRCS
        "main.cpp"
        ${private_lib}
    INCLUDE_DIRS
        "."
        "defines"
        "threads"
        "app"
        "drivers"
        "modules"
        "../common/defines"
        "../common/threading"
)
