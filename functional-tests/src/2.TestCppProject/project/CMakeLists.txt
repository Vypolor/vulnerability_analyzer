cmake_minimum_required(VERSION 3.0)
project(SysProg)

set(LIBELFIN_PATH "${PROJECT_SOURCE_DIR}/lib/libelfin")

file(GLOB_RECURSE APP_SRC
        "src/*.h"
        "src/*.c"
        "src/*.cpp"
        )

add_executable(my_app ${APP_SRC})

include_directories(include/ lib/)

add_custom_target(
        libelfin
        COMMAND make
        WORKING_DIRECTORY ${LIBELFIN_PATH}
)

file(GLOB_RECURSE LINENOISE_SRC
        "lib/linenoise/*.h"
        "lib/linenoise/*.c"
        "lib/linenoise/*.cpp"
        )

add_library(linenoise SHARED ${LINENOISE_SRC})
target_link_libraries(my_app linenoise ${LIBELFIN_PATH}/dwarf/libdwarf++.so ${LIBELFIN_PATH}/elf/libelf++.so)

add_dependencies(my_app libelfin linenoise)

