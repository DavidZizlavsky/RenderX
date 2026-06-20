cmake_minimum_required(VERSION 3.20)

project(RenderX)

include(FetchContent)

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
)