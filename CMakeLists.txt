cmake_minimum_required(VERSION 3.30)
project(GameOfLife)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

function(include_dependency libName gitURL gitTag)
    FetchContent_Declare(${libName}
        GIT_REPOSITORY ${gitURL}
            GIT_TAG ${gitTag}
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(${libName})
endfunction()

set(LIB1 raylib)
find_package(${LIB1} QUIET)
if (NOT ${LIB1}_FOUND)
    include_dependency(${LIB1} https://github.com/raysan5/raylib.git 5.5)
endif ()

add_executable(GameOfLife main.cpp
        Headers/RaylibOverloads.h
        Headers/Util.h
        Headers/Components.h
        Headers/GameCamera.h)

target_include_directories(GameOfLife PRIVATE ${raylib_INCLUDE_DIRS})

target_link_libraries(GameOfLife PRIVATE ${LIB1})