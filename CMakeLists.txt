cmake_minimum_required(VERSION 3.6)
project(pectin)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_MACOSX_RPATH 1)

include_directories("deps/glad/include/"
        "deps/stb"
        ${SDL2_INCLUDE_DIR}
        ${SDLMIXER_INCLUDE_DIR})

file(GLOB DEPS_SOURCES deps/glad/src/glad.c)
file(GLOB PROJECT_HEADERS *.hpp *.h)
file(GLOB PROJECT_SOURCES *.cpp *.c)
file(GLOB PROJECT_SHADERS shaders/*.glsl
        shaders/*.frag
        shaders/*.geom
        shaders/*.vert)

source_group("include" FILES ${PROJECT_HEADERS})
source_group("shaders" FILES ${PROJECT_SHADERS})
source_group("src" FILES ${PROJECT_SOURCES})
source_group("deps" FILES ${DEPS_SOURCES})

add_definitions(-DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${PROJECT_SHADERS}
        ${PROJECT_CONFIGS}
        ${DEPS_SOURCES} window.cpp window.h render_util.c render_util.h misc_util.c misc_util.h game_util.c game_util.h physics.c physics.h boids.c boids.h)

set_target_properties(${PROJECT_NAME} PROPERTIES
        LINKER_FLAGS "/ignore:4099")

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

target_link_libraries(${PROJECT_NAME} ${GLAD_LIBRARIES} ${SDL2_LIBRARY}
        ${SDLMIXER_LIBRARY})
