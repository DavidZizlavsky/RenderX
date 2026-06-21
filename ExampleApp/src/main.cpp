#include <iostream>
#include <RenderX/Renderer.hpp>
#include <GLFW/glfw3.h>

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cout << "[App] Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "RenderX Vulkan Window", nullptr, nullptr);

    if (!window) {
        std::cout << "[App] Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    std::cout << "[App] GLFW window created" << std::endl;

    RenderX::Renderer renderer;

    // Create config and enable debugging
    RenderX::Config renderConfig;
    renderConfig.debugging = true;

    if (!renderer.Initialize(renderConfig)) {
        std::cout << "[App] Failed to initialize RenderX renderer" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    // Cleanup Vulkan
    renderer.Shutdown();

    // Cleanup GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}