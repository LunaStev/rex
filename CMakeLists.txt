cmake_minimum_required(VERSION 3.15)
project(rex VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)

# --------------------------
# Options
# --------------------------
option(REX_BUILD_EXAMPLES   "Build examples" ON)
option(REX_BUILD_LAUNCHER   "Build rex launcher (runtime)" ON)
option(REX_INSTALL_RUNTIME  "Install rex runtime assets (share/rex)" ON)
option(REX_INSTALL_SDK      "Install engine library for C++ SDK usage" OFF)

# --------------------------
# Sources
# --------------------------
file(GLOB_RECURSE ENGINE_SOURCES  CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Engine/*.cpp")
file(GLOB_RECURSE EXAMPLE_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Examples/*.cpp")

# --------------------------
# Engine library
# --------------------------
add_library(rex_engine STATIC ${ENGINE_SOURCES})
target_include_directories(rex_engine
    PUBLIC
        ${CMAKE_SOURCE_DIR}/Engine
)

# --------------------------
# Dependencies: SDL2 / SDL2_* / Lua
# --------------------------
if(WIN32)
    # User-configurable roots
    if(NOT DEFINED SDL2_DIR)
        set(SDL2_DIR "C:/SDL2")
    endif()
    if(NOT DEFINED SDL2_MIXER_DIR)
        set(SDL2_MIXER_DIR "C:/SDL2_mixer")
    endif()
    if(NOT DEFINED SDL2_TTF_DIR)
        set(SDL2_TTF_DIR "C:/SDL2_ttf")
    endif()
    if(NOT DEFINED SDL2_IMAGE_DIR)
        set(SDL2_IMAGE_DIR "C:/SDL2_image")
    endif()
    if(NOT DEFINED LUA_DIR)
        set(LUA_DIR "C:/Lua")
    endif()

    # Includes
    target_include_directories(rex_engine PRIVATE
        ${SDL2_DIR}/include
        ${SDL2_MIXER_DIR}/include
        ${SDL2_TTF_DIR}/include
        ${SDL2_IMAGE_DIR}/include
        ${LUA_DIR}/include
    )

    # Lib dirs
    link_directories(
        ${SDL2_DIR}/lib
        ${SDL2_MIXER_DIR}/lib
        ${SDL2_TTF_DIR}/lib
        ${SDL2_IMAGE_DIR}/lib
        ${LUA_DIR}/lib
    )

    # Lua
    find_library(LUA_LIB
        NAMES lua54 lua53 lua52 lua51 lua
        PATHS ${LUA_DIR}/lib
        NO_DEFAULT_PATH
    )
    if(NOT LUA_LIB)
        find_library(LUA_LIB NAMES lua54 lua53 lua52 lua51 lua)
    endif()
    if(NOT LUA_LIB)
        message(FATAL_ERROR "Lua library not found. Set LUA_DIR (e.g. -DLUA_DIR=C:/Lua).")
    endif()

    # Link (Windows)
    target_link_libraries(rex_engine PRIVATE
        SDL2 SDL2main
        SDL2_mixer SDL2_ttf SDL2_image
        ${LUA_LIB}
    )

else()
    # Linux / macOS
    find_package(PkgConfig REQUIRED)

    find_package(SDL2 REQUIRED)
    find_package(Lua REQUIRED)

    # Prefer Find-package includes for SDL2 itself
    target_include_directories(rex_engine PRIVATE ${SDL2_INCLUDE_DIRS})
    target_include_directories(rex_engine PRIVATE ${LUA_INCLUDE_DIR})

    # SDL2_mixer
    find_package(SDL2_mixer QUIET)
    if(SDL2_mixer_FOUND)
        set(REX_SDL2_MIXER_LIBS ${SDL2_MIXER_LIBRARIES})
        target_include_directories(rex_engine PRIVATE ${SDL2_MIXER_INCLUDE_DIRS})
    else()
        pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)
        set(REX_SDL2_MIXER_LIBS ${SDL2_MIXER_LIBRARIES})
        target_include_directories(rex_engine PRIVATE ${SDL2_MIXER_INCLUDE_DIRS})
    endif()

    # SDL2_ttf
    find_package(SDL2_ttf QUIET)
    if(SDL2_ttf_FOUND)
        set(REX_SDL2_TTF_LIBS ${SDL2_TTF_LIBRARIES})
        target_include_directories(rex_engine PRIVATE ${SDL2_TTF_INCLUDE_DIRS})
    else()
        pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf)
        set(REX_SDL2_TTF_LIBS ${SDL2_TTF_LIBRARIES})
        target_include_directories(rex_engine PRIVATE ${SDL2_TTF_INCLUDE_DIRS})
    endif()

    # SDL2_image
    find_package(SDL2_image QUIET)
    if(SDL2_image_FOUND)
        set(REX_SDL2_IMAGE_LIBS ${SDL2_IMAGE_LIBRARIES})
        target_include_directories(rex_engine PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})
    else()
        pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)
        set(REX_SDL2_IMAGE_LIBS ${SDL2_IMAGE_LIBRARIES})
        target_include_directories(rex_engine PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})
    endif()

    # Homebrew hint (macOS)
    if(APPLE)
        target_include_directories(rex_engine PRIVATE /opt/homebrew/include /opt/homebrew/include/SDL2)
        link_directories(/opt/homebrew/lib)
    endif()

    # Link (Linux/macOS)
    target_link_libraries(rex_engine PRIVATE
        ${SDL2_LIBRARIES}
        ${REX_SDL2_MIXER_LIBS}
        ${REX_SDL2_TTF_LIBS}
        ${REX_SDL2_IMAGE_LIBS}
        ${LUA_LIBRARIES}
    )
endif()

# --------------------------
# Launcher: rex
# --------------------------
if(REX_BUILD_LAUNCHER)
    add_executable(rex "${CMAKE_SOURCE_DIR}/Engine/Launcher/rex_main.cpp")
    target_link_libraries(rex PRIVATE rex_engine)

    if(APPLE)
        target_link_libraries(rex PRIVATE
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
        )
    endif()
endif()

# --------------------------
# Examples
# --------------------------
if(REX_BUILD_EXAMPLES)
    foreach(example_source ${EXAMPLE_SOURCES})
        get_filename_component(example_name ${example_source} NAME_WE)
        set(target_name "rex_${example_name}")

        add_executable(${target_name} ${example_source})
        target_link_libraries(${target_name} PRIVATE rex_engine)

        if(APPLE)
            target_link_libraries(${target_name} PRIVATE
                "-framework Cocoa"
                "-framework IOKit"
                "-framework CoreVideo"
            )
        endif()

        message(STATUS "Added example target: ${target_name}")
    endforeach()
endif()

# --------------------------
# Install (runtime-first)
# --------------------------
if(REX_BUILD_LAUNCHER)
    install(TARGETS rex
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

if(REX_INSTALL_RUNTIME)
    if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
        install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets/"
            DESTINATION "${CMAKE_INSTALL_DATADIR}/rex/assets"
        )
    endif()
endif()

if(REX_INSTALL_SDK)
    install(TARGETS rex_engine
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

# --------------------------
# Info
# --------------------------
message(STATUS "Building '${PROJECT_NAME}' (${PROJECT_VERSION}) on ${CMAKE_SYSTEM_NAME}")
message(STATUS "C++${CMAKE_CXX_STANDARD} with ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")