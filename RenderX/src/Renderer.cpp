#include "RenderX/Renderer.hpp"
#include "RenderX/Config.hpp"
#include "RenderX/Vulkan/VulkanInstance.hpp"
#include "RenderX/Vulkan/VulkanContext.hpp"
#include <iostream>
#include <memory>

namespace RenderX {
    Renderer::Renderer() = default;

    Renderer::~Renderer() {
        Shutdown();
    }

    bool Renderer::Initialize(Config& config) {
        // Allocates the VulkanContext class
        m_context = std::make_unique<VulkanContext>();

        // Allocation failed
        if (m_context == nullptr) {
            return false;
        }

        // Save config
        m_context->m_config = config;

        // Initialize the Vulkan instance
        if (!m_context->m_instance.Initialize(m_context->m_config)) {
            return false;
        }

        // If debugging is enabled initialize the debug messenger
        if (m_context->m_config.debugging) {
            if (!m_context->m_debugMessenger.Initialize(m_context->m_instance.GetHandle())) {
                std::cerr << "[RenderX] Failed to initialize debug messenger" << std::endl;
            }
        }

        return true;
    }

    void Renderer::Shutdown() {
        // Avoid multi-shutdown call
        if (!m_context) {
            return;
        }

        // Perform cleanup
        m_context->m_debugMessenger.Shutdown(m_context->m_instance.GetHandle());
        m_context->m_instance.Shutdown();

        // Reset unique pointer
        m_context.reset();
    }
}