cmake_minimum_required(VERSION 3.20)
project(RexEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Dependencies
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
find_package(Freetype REQUIRED)

# Internal Includes
include_directories(
    Engine
    ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
)

# Core Engine Library (including Physics & Graphics)
file(GLOB_RECURSE ENGINE_SOURCES 
    Engine/Core/*.cpp 
    Engine/Graphics/*.cpp
    Engine/Physics/*.cpp
    Engine/UI/*.cpp
)

add_library(rex_core STATIC ${ENGINE_SOURCES})
target_link_libraries(rex_core
    ${SDL2_LIBRARIES} 
    ${OPENGL_LIBRARIES}
    Freetype::Freetype
)

# Editor (RexUI + SDL + OpenGL)
add_executable(rex-editor Engine/EditorRex/rexui_main.cpp)
target_link_libraries(rex-editor rex_core)

# Runtime
add_executable(rex-runtime Engine/Runtime/runtime_main.cpp)
target_link_libraries(rex-runtime rex_core)
