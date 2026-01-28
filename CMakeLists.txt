cmake_minimum_required(VERSION 3.15)
project(rex VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- SDL2 + SDL2_mixer + SDL2_ttf + SDL2_image + Lua ---
if(WIN32)
    # --- Windows ---
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

    # --- Lua (for Lua bindings) ---
    if(NOT DEFINED LUA_DIR)
        set(LUA_DIR "C:/Lua")
    endif()

    include_directories(
        ${SDL2_DIR}/include
        ${SDL2_MIXER_DIR}/include
        ${SDL2_TTF_DIR}/include
        ${SDL2_IMAGE_DIR}/include
        ${LUA_DIR}/include
    )

    link_directories(
        ${SDL2_DIR}/lib
        ${SDL2_MIXER_DIR}/lib
        ${SDL2_TTF_DIR}/lib
        ${SDL2_IMAGE_DIR}/lib
        ${LUA_DIR}/lib
    )

    find_library(LUA_LIB
        NAMES lua54 lua53 lua52 lua51 lua
        PATHS ${LUA_DIR}/lib
        NO_DEFAULT_PATH
    )
    if(NOT LUA_LIB)
        find_library(LUA_LIB NAMES lua54 lua53 lua52 lua51 lua)
    endif()
    if(NOT LUA_LIB)
        message(FATAL_ERROR "Lua library not found. Set LUA_DIR to your Lua installation path (e.g. -DLUA_DIR=C:/Lua).")
    endif()

else()
    # --- Linux / macOS / WSL ---
    find_package(SDL2 REQUIRED)
    find_package(Lua REQUIRED)

    if(SDL2_FOUND)
        include_directories(${SDL2_INCLUDE_DIRS})
    else()
        find_package(PkgConfig REQUIRED)
        pkg_search_module(SDL2 REQUIRED sdl2)
        include_directories(${SDL2_INCLUDE_DIRS})
    endif()

    # PkgConfig fallback helper
    find_package(PkgConfig REQUIRED)

    # --- SDL2_mixer ---
    find_package(SDL2_mixer)
    if(SDL2_mixer_FOUND)
        include_directories(${SDL2_MIXER_INCLUDE_DIRS})
        set(REX_SDL2_MIXER_LIBS ${SDL2_MIXER_LIBRARIES})
    else()
        message(STATUS "Trying pkg-config for SDL2_mixer...")
        pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)
        include_directories(${SDL2_MIXER_INCLUDE_DIRS})
        set(REX_SDL2_MIXER_LIBS ${SDL2_MIXER_LIBRARIES})
    endif()

    # --- SDL2_ttf ---
    find_package(SDL2_ttf)
    if(SDL2_ttf_FOUND)
        include_directories(${SDL2_TTF_INCLUDE_DIRS})
        set(REX_SDL2_TTF_LIBS ${SDL2_TTF_LIBRARIES})
    else()
        message(STATUS "Trying pkg-config for SDL2_ttf...")
        pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf)
        include_directories(${SDL2_TTF_INCLUDE_DIRS})
        set(REX_SDL2_TTF_LIBS ${SDL2_TTF_LIBRARIES})
    endif()

    # --- SDL2_image ---
    find_package(SDL2_image)
    if(SDL2_image_FOUND)
        include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
        set(REX_SDL2_IMAGE_LIBS ${SDL2_IMAGE_LIBRARIES})
    else()
        message(STATUS "Trying pkg-config for SDL2_image...")
        pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)
        include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
        set(REX_SDL2_IMAGE_LIBS ${SDL2_IMAGE_LIBRARIES})
    endif()

    # Homebrew (macOS) convenience
    include_directories(/opt/homebrew/include /opt/homebrew/include/SDL2)
    link_directories(/opt/homebrew/lib)
endif()

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS Engine/*.cpp)
file(GLOB_RECURSE EXAMPLE_SOURCES CONFIGURE_DEPENDS Examples/*.cpp)

add_library(rex_engine ${ENGINE_SOURCES})

if(WIN32)
    target_link_libraries(rex_engine
        SDL2 SDL2main
        SDL2_mixer SDL2_ttf SDL2_image
        ${LUA_LIB}
    )

elseif(APPLE)
    find_library(SDL2_LIB SDL2)
    find_library(SDL2_MIXER_LIB SDL2_mixer)
    find_library(SDL2_TTF_LIB SDL2_ttf)
    find_library(SDL2_IMAGE_LIB SDL2_image)

    target_include_directories(rex_engine PRIVATE ${LUA_INCLUDE_DIR})
    target_link_libraries(rex_engine
        ${SDL2_LIB} ${SDL2_MIXER_LIB} ${SDL2_TTF_LIB} ${SDL2_IMAGE_LIB}
        ${LUA_LIBRARIES}
    )

else()
    # Linux / others
    target_include_directories(rex_engine PRIVATE ${LUA_INCLUDE_DIR})
    target_link_libraries(rex_engine
        ${SDL2_LIBRARIES}
        ${REX_SDL2_MIXER_LIBS}
        ${REX_SDL2_TTF_LIBS}
        ${REX_SDL2_IMAGE_LIBS}
        ${LUA_LIBRARIES}

        # 일부 환경에서 pkg-config/Find 모듈 혼용 시 심볼 누락 방지용
        SDL2_mixer
        SDL2_ttf
        SDL2_image
    )
endif()

foreach(example_source ${EXAMPLE_SOURCES})
    get_filename_component(example_name ${example_source} NAME_WE)
    set(target_name "rex_${example_name}")

    add_executable(${target_name} ${example_source})
    target_link_libraries(${target_name} rex_engine)

    if(APPLE)
        target_link_libraries(${target_name}
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
        )
    endif()

    message(STATUS "Added example target: ${target_name}")
endforeach()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(EXISTS "/proc/version")
        file(READ "/proc/version" PROC_VERSION)
        if(PROC_VERSION MATCHES "Microsoft")
            message(STATUS "Detected WSL environment ✅")
            execute_process(COMMAND bash -c "which sdl2-config" RESULT_VARIABLE SDL2_RESULT)
            if(NOT SDL2_RESULT EQUAL 0)
                message(WARNING "SDL2 not found. Install with: sudo apt install libsdl2-dev")
            endif()
        endif()
    endif()
endif()

message(STATUS "Building project '${PROJECT_NAME}' (${PROJECT_VERSION}) on ${CMAKE_SYSTEM_NAME}")
message(STATUS "Using C++${CMAKE_CXX_STANDARD} with ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
