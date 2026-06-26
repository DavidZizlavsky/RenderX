cmake_minimum_required(VERSION 3.20)

project(RenderX)

# Use C++20 for all targets
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

file(GLOB_RECURSE RENDERX_HEADERS
    RenderX/include/*.h
    RenderX/include/*.hpp
)

file(GLOB_RECURSE RENDERX_SOURCES
    RenderX/src/*.cpp
)

file(GLOB_RECURSE APP_SOURCES
    ExampleApp/src/*.cpp
)

add_library(
    RenderX
    STATIC
    ${RENDERX_SOURCES}
    ${RENDERX_HEADERS}
)

add_executable(
    ExampleApp
    ${APP_SOURCES}
)

target_include_directories(
    RenderX
    PUBLIC
    RenderX/include
)

find_package(Vulkan REQUIRED)

target_link_libraries(
    RenderX
    PRIVATE
    Vulkan::Vulkan
)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

target_link_libraries(
    RenderX
    PRIVATE
    glfw
)

target_link_libraries(
    ExampleApp
    PRIVATE
    RenderX
    glfw
)