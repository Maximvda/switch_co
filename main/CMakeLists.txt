file(GLOB private_lib
    modules/*.cpp
    drivers/*.cpp
    threading/*.cpp
    threads/*.cpp
    app/*.cpp
)

idf_component_register(
    SRCS
        "main.cpp"
        ${private_lib}
    INCLUDE_DIRS
        "."
        "defines"
        "threading"
        "threads"
        "app"
        "drivers"
        "modules"
)
