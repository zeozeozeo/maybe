cmake_minimum_required(VERSION 3.5)

project(maybe LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find SDL2
find_package(SDL2 REQUIRED)
include_directories(maybe ${SDL2_INCLUDE_DIRS})

# embed assets folder into the executable with cmrc
include(CMakeRC.cmake)
cmrc_add_resource_library(assets-rc
    ALIAS assets::rc
    NAMESPACE assets
    assets/level1.png
    assets/level1.run
    assets/spike.png
)

add_executable(maybe
    src/main.cpp
    src/game.cpp
    src/game.h
    src/color.h
    src/camera.cpp
    src/camera.h
    src/vec2.h
    src/level.cpp
    src/level.h
    src/error.cpp
    src/error.h
    src/player.cpp
    src/player.h
    src/particles.cpp
    src/particles.h
    src/utils.cpp
    src/utils.h
    src/ghost.cpp
    src/ghost.h
)

# link
target_link_libraries(maybe ${SDL2_LIBRARIES} assets::rc)
target_include_directories(maybe PRIVATE "include")

install(TARGETS maybe
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
