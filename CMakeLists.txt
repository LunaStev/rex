cmake_minimum_required(VERSION 3.15)
project(rex VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- SDL2 + SDL2_mixer + SDL2_ttf ---
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

    include_directories(
        ${SDL2_DIR}/include
        ${SDL2_MIXER_DIR}/include
        ${SDL2_TTF_DIR}/include
    )
    link_directories(
        ${SDL2_DIR}/lib
        ${SDL2_MIXER_DIR}/lib
        ${SDL2_TTF_DIR}/lib
    )

else()
    # --- Linux / macOS / WSL ---
    find_package(SDL2 REQUIRED)
    if(SDL2_FOUND)
        include_directories(${SDL2_INCLUDE_DIRS})
    else()
        find_package(PkgConfig REQUIRED)
        pkg_search_module(SDL2 REQUIRED sdl2)
        include_directories(${SDL2_INCLUDE_DIRS})
    endif()

    # --- SDL2_mixer ---
    find_package(SDL2_mixer)
    if(SDL2_mixer_FOUND)
        include_directories(${SDL2_MIXER_INCLUDE_DIRS})
    else()
        message(STATUS "Trying pkg-config for SDL2_mixer...")
        find_package(PkgConfig REQUIRED)
        pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)
        include_directories(${SDL2_MIXER_INCLUDE_DIRS})
    endif()

    # --- SDL2_ttf ---
    find_package(SDL2_ttf)
    if(SDL2_ttf_FOUND)
        include_directories(${SDL2_TTF_INCLUDE_DIRS})
    else()
        message(STATUS "Trying pkg-config for SDL2_ttf...")
        find_package(PkgConfig REQUIRED)
        pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf)
        include_directories(${SDL2_TTF_INCLUDE_DIRS})
    endif()

    include_directories(/opt/homebrew/include /opt/homebrew/include/SDL2)
    link_directories(/opt/homebrew/lib)
endif()

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS Engine/*.cpp)
file(GLOB_RECURSE EXAMPLE_SOURCES CONFIGURE_DEPENDS Examples/*.cpp)

add_library(rex_engine ${ENGINE_SOURCES})

if(WIN32)
    target_link_libraries(rex_engine SDL2 SDL2main SDL2_mixer SDL2_ttf)
elseif(APPLE)
    find_library(SDL2_LIB SDL2)
    find_library(SDL2_MIXER_LIB SDL2_mixer)
    find_library(SDL2_TTF_LIB SDL2_ttf)
    target_link_libraries(rex_engine ${SDL2_LIB} ${SDL2_MIXER_LIB} ${SDL2_TTF_LIB})
else()
    target_link_libraries(rex_engine
        ${SDL2_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
        SDL2_mixer
        SDL2_ttf
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
            execute_process(COMMAND bash -c "which sdl2-config" RESULT_VARIABLE MIXER_RESULT)
            if(NOT MIXER_RESULT EQUAL 0)
                message(WARNING "SDL2_mixer not found. Install with: sudo apt install libsdl2-mixer-dev")
            endif()
        endif()
    endif()
endif()

message(STATUS "Building project '${PROJECT_NAME}' (${PROJECT_VERSION}) on ${CMAKE_SYSTEM_NAME}")
message(STATUS "Using C++${CMAKE_CXX_STANDARD} with ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
