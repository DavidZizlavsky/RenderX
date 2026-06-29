#include "RenderX/Renderer.hpp"
#include "RenderX/Config.hpp"
#include "RenderX/Logger.hpp"
#include "RenderX/Vulkan/VulkanInstance.hpp"
#include "RenderX/Vulkan/VulkanContext.hpp"
#include <RenderX/Vulkan/VulkanSwapchainSupport.hpp>
#include <memory>
#include <vector>

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
            RX_LOG_ERROR("Failed to allocate memory for VulkanContext class");
            return false;
        }

        // Save config
        m_context->m_config = config;

        // Initialize the Vulkan instance
        if (!m_context->m_instance.Initialize(m_context->m_config)) {
            RX_LOG_ERROR("Failed to initialize VulkanInstance");
            return false;
        }

        // Cache instance handle
        VkInstance instance = m_context->m_instance.GetHandle();

        // If debugging is enabled initialize the debug messenger
        if (config.debugging) {
            if (!m_context->m_debugMessenger.Initialize(instance)) {
                RX_LOG_ERROR("Failed to initialize debug messenger");
                return false;
            }
        }

        // Initialize the Vulkan surface
        if (!m_context->m_surface.Initialize(instance, config.windowHandle)) {
            RX_LOG_ERROR("Failed to initialize the VulkanSurface");
            return false;
        }

        // Cache surface handle
        VkSurfaceKHR surface = m_context->m_surface.GetHandle();

        // Initialize the Vulkan physical device
        if (!m_context->m_physicalDevice.Initialize(instance, surface)) {
            RX_LOG_ERROR("Failed to initialize the VulkanPhysicalDevice");
            return false;
        }

        // Initialize the Vulkan logical device
        QueueFamilyIndices indices = VulkanQueueFamily::Find(m_context->m_physicalDevice.GetHandle(), surface);
        if (!m_context->m_device.Initialize(m_context->m_physicalDevice.GetHandle(), indices, surface)) {
            RX_LOG_ERROR("Failed to initialize the VulkanDevice");
            return false;
        }

        // Initialize the Vulkan swapchain
        VkDevice logicalDevice = m_context->m_device.GetHandle();
        SwapchainSupportDetails swapchainSupport = m_context->m_physicalDevice.GetInfo().swapchainSupport;
        if (!m_context->m_swapchain.Initialize(logicalDevice, surface, indices, swapchainSupport)) {
            RX_LOG_ERROR("Failed to initialize the VulkanSwapchain");
            return false;
        }

        // Initialize the Vulkan image views
        VkFormat format = m_context->m_swapchain.GetFormat();
        const std::vector<VkImage>& images = m_context->m_swapchain.GetImages();
        if (!m_context->m_imageViews.Initialize(logicalDevice, format, images)) {
            RX_LOG_ERROR("Failed to initialize the VulkanImageViews");
            return false;
        }

        return true;
    }

    void Renderer::Shutdown() {
        // Avoid multi-shutdown call
        if (!m_context) {
            return;
        }

        // Perform cleanup
        m_context->m_imageViews.Shutdown();
        m_context->m_swapchain.Shutdown();
        m_context->m_device.Shutdown();
        m_context->m_physicalDevice.Shutdown();
        m_context->m_surface.Shutdown(m_context->m_instance.GetHandle());
        m_context->m_debugMessenger.Shutdown(m_context->m_instance.GetHandle());
        m_context->m_instance.Shutdown();

        // Reset unique pointer
        m_context.reset();
    }
}