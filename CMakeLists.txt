cmake_minimum_required(VERSION 3.20)
project(RexEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Dependencies
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
find_package(Freetype REQUIRED)
find_program(CARGO_EXECUTABLE cargo REQUIRED)

# Internal Includes
include_directories(
    Engine
    ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
)

set(RUST_PHYSICS_DIR ${CMAKE_SOURCE_DIR}/Engine/Rust/physics_core)
set(RUST_TARGET_DIR ${CMAKE_BINARY_DIR}/cargo-target)
file(GLOB_RECURSE RUST_PHYSICS_SOURCES CONFIGURE_DEPENDS
    ${RUST_PHYSICS_DIR}/src/*.rs
    ${RUST_PHYSICS_DIR}/Cargo.toml
)

set(RUST_CARGO_ARGS build --manifest-path ${RUST_PHYSICS_DIR}/Cargo.toml)
if(CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND RUST_CARGO_ARGS --release)
    set(RUST_LIB_SUBDIR release)
else()
    set(RUST_LIB_SUBDIR debug)
endif()

set(RUST_PHYSICS_LIB ${RUST_TARGET_DIR}/${RUST_LIB_SUBDIR}/librex_physics_core.a)
add_custom_command(
    OUTPUT ${RUST_PHYSICS_LIB}
    COMMAND ${CMAKE_COMMAND} -E env CARGO_TARGET_DIR=${RUST_TARGET_DIR}
        ${CARGO_EXECUTABLE} ${RUST_CARGO_ARGS}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS ${RUST_PHYSICS_SOURCES}
    COMMENT "Building Rust physics core"
    VERBATIM
)

add_custom_target(rust_physics_core_build DEPENDS ${RUST_PHYSICS_LIB})
add_library(rust_physics_core STATIC IMPORTED GLOBAL)
set_target_properties(rust_physics_core PROPERTIES
    IMPORTED_LOCATION ${RUST_PHYSICS_LIB}
)
add_dependencies(rust_physics_core rust_physics_core_build)

# Core Engine Library (including Physics & Graphics)
file(GLOB_RECURSE ENGINE_SOURCES 
    Engine/Core/*.cpp 
    Engine/Graphics/*.cpp
    Engine/Physics/*.cpp
    Engine/UI/*.cpp
)

add_library(rex_core STATIC ${ENGINE_SOURCES})
target_link_libraries(rex_core
    rust_physics_core
    ${SDL2_LIBRARIES} 
    ${OPENGL_LIBRARIES}
    Freetype::Freetype
)

if(UNIX)
    target_link_libraries(rex_core dl pthread m)
endif()

# Editor (RexUI + SDL + OpenGL)
add_executable(rex-editor Engine/EditorRex/rexui_main.cpp)
target_link_libraries(rex-editor rex_core)

# Runtime
add_executable(rex-runtime Engine/Runtime/runtime_main.cpp)
target_link_libraries(rex-runtime rex_core)
