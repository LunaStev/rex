cmake_minimum_required(VERSION 3.15)
project(rex VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SDL2_DIR "C:/sdl2/x86_64-w64-mingw32")
include_directories(${SDL2_DIR}/include)
link_directories(${SDL2_DIR}/lib)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    Engine/*.cpp
)

file(GLOB_RECURSE EXAMPLE_SOURCES CONFIGURE_DEPENDS
    Examples/*.cpp
)

# --- 엔진 라이브러리 ---
add_library(rex_engine ${ENGINE_SOURCES})

# SDL2 라이브러리 링크
if(WIN32)
    target_link_libraries(rex_engine SDL2 SDL2main)
else()
    target_link_libraries(rex_engine SDL2)
endif()

add_executable(rex_example ${EXAMPLE_SOURCES})
target_link_libraries(rex_example rex_engine)
