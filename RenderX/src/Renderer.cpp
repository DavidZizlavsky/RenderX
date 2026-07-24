#include "RenderX/Renderer.hpp"
#include "RenderX/Config.hpp"
#include "RenderX/Logger.hpp"
#include "RenderX/Vulkan/VulkanInstance.hpp"
#include "RenderX/Vulkan/VulkanContext.hpp"
#include <RenderX/Vulkan/VulkanSwapchainSupport.hpp>
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <cstdint>

namespace {
	// Records a layout transition for a single color image via a Vulkan 1.3 synchronization2 barrier
	void TransitionColorImageLayout(
		VkCommandBuffer commandBuffer,
		VkImage image,
		VkImageLayout oldLayout,
		VkImageLayout newLayout,
		VkPipelineStageFlags2 srcStageMask,
		VkAccessFlags2 srcAccessMask,
		VkPipelineStageFlags2 dstStageMask,
		VkAccessFlags2 dstAccessMask
	) {
		VkImageMemoryBarrier2 barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		barrier.srcStageMask = srcStageMask;
		barrier.srcAccessMask = srcAccessMask;
		barrier.dstStageMask = dstStageMask;
		barrier.dstAccessMask = dstAccessMask;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkDependencyInfo dependencyInfo{};
		dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		dependencyInfo.imageMemoryBarrierCount = 1;
		dependencyInfo.pImageMemoryBarriers = &barrier;

		vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
	}
}

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

        // Initialize the Vulkan graphics pipeline
        if (!m_context->m_graphicsPipeline.Initialize(logicalDevice, format, config.vertexShaderPath, config.fragmentShaderPath)) {
            RX_LOG_ERROR("Failed to initialize the VulkanGraphicsPipeline");
            return false;
        }

        // Initialize the Vulkan command pool
        uint32_t graphicsQueue = indices.graphicsFamily.value();
        if (!m_context->m_commandPool.Initialize(logicalDevice, graphicsQueue)) {
            RX_LOG_ERROR("Failed to initialize the VulkanCommandPool");
            return false;
        }

        // Initialize the Vulkan command buffers
        uint32_t imageViewCount = static_cast<size_t>(m_context->m_imageViews.GetImageViews().size());
        VkCommandPool commandPool = m_context->m_commandPool.GetHandle();
        if (!m_context->m_commandBuffers.Initialize(logicalDevice, commandPool, imageViewCount)) {
            RX_LOG_ERROR("Failed to initialize the VulkanCommandBuffers");
            return false;
        }

        // Initialize the Vulkan sync objects
        if (!m_context->m_syncObjects.Initialize(logicalDevice, imageViewCount)) {
            RX_LOG_ERROR("Failed to initialize the VulkanSyncObjects");
            return false;
        }

        return true;
    }

    bool Renderer::DrawFrame() {
        VkDevice device = m_context->m_device.GetHandle();
        uint32_t frameCount = static_cast<uint32_t>(m_context->m_imageViews.GetImageViews().size());
        uint32_t frame = m_context->m_currentFrame;

        // Wait for this frame slot's previous work to finish before reusing its command buffer/fence
        VkFence fence = m_context->m_syncObjects.GetFence(frame);
        vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);

        // Acquire the next swapchain image
        VkSemaphore imageAvailable = m_context->m_syncObjects.GetImageAvailable(frame);
        uint32_t imageIndex = 0;
        VkResult result = vkAcquireNextImageKHR(
            device,
            m_context->m_swapchain.GetHandle(),
            UINT64_MAX,
            imageAvailable,
            VK_NULL_HANDLE,
            &imageIndex
        );

        // Swapchain recreation on resize is not implemented yet; skip this frame rather than crash
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            RX_LOG_WARNING("Swapchain out of date, skipping frame (resize handling not implemented yet)");
            return true;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            RX_LOG_ERROR("Failed to acquire swapchain image");
            return false;
        }

        // Only reset the fence once we know we're actually submitting work this frame
        vkResetFences(device, 1, &fence);

        VkImage image = m_context->m_swapchain.GetImages()[imageIndex];
        VkImageView imageView = m_context->m_imageViews.GetImageViews()[imageIndex];
        VkCommandBuffer commandBuffer = m_context->m_commandBuffers.GetBuffers()[imageIndex];

        vkResetCommandBuffer(commandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        // UNDEFINED -> COLOR_ATTACHMENT_OPTIMAL so the image can be cleared/rendered into
        TransitionColorImageLayout(
            commandBuffer,
            image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            0,
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT
        );

        // Dynamic rendering: clear and render into the swapchain image view directly,
        // no VkRenderPass/VkFramebuffer needed.
        VkRenderingAttachmentInfo colorAttachment{};
        colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        colorAttachment.imageView = imageView;
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue.color = { { 0.02f, 0.02f, 0.05f, 1.0f } };

        VkRenderingInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea.offset = { 0, 0 };
        renderingInfo.renderArea.extent = m_context->m_swapchain.GetExtent();
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;

        vkCmdBeginRendering(commandBuffer, &renderingInfo);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_context->m_graphicsPipeline.GetHandle());

        // Viewport/scissor are dynamic pipeline state, so they're set here every frame from the
        // current swapchain extent instead of being baked into the pipeline at creation time.
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_context->m_swapchain.GetExtent().width);
        viewport.height = static_cast<float>(m_context->m_swapchain.GetExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = m_context->m_swapchain.GetExtent();
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        // Triangle's 3 vertices are hard-coded in the vertex shader - no vertex buffer bound
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkCmdEndRendering(commandBuffer);

        // COLOR_ATTACHMENT_OPTIMAL -> PRESENT_SRC_KHR so the image can be presented
        TransitionColorImageLayout(
            commandBuffer,
            image,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
            0
        );

        vkEndCommandBuffer(commandBuffer);

        // Render-finished is indexed by imageIndex (not frame slot) so it can't be signaled twice
        // concurrently if a frame slot is reused before its image's previous present has completed.
        VkSemaphore renderFinished = m_context->m_syncObjects.GetRenderFinished(imageIndex);

        VkCommandBufferSubmitInfo commandBufferSubmitInfo{};
        commandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        commandBufferSubmitInfo.commandBuffer = commandBuffer;

        VkSemaphoreSubmitInfo waitSemaphoreInfo{};
        waitSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        waitSemaphoreInfo.semaphore = imageAvailable;
        waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSemaphoreSubmitInfo signalSemaphoreInfo{};
        signalSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        signalSemaphoreInfo.semaphore = renderFinished;
        signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo2 submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        submitInfo.waitSemaphoreInfoCount = 1;
        submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
        submitInfo.commandBufferInfoCount = 1;
        submitInfo.pCommandBufferInfos = &commandBufferSubmitInfo;
        submitInfo.signalSemaphoreInfoCount = 1;
        submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

        result = vkQueueSubmit2(m_context->m_device.GetGraphicsQueue(), 1, &submitInfo, fence);
        if (result != VK_SUCCESS) {
            RX_LOG_ERROR("Failed to submit draw command buffer");
            return false;
        }

        VkSwapchainKHR swapchain = m_context->m_swapchain.GetHandle();

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinished;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(m_context->m_device.GetPresentQueue(), &presentInfo);
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR && result != VK_ERROR_OUT_OF_DATE_KHR) {
            RX_LOG_ERROR("Failed to present swapchain image");
            return false;
        }

        m_context->m_currentFrame = (frame + 1) % frameCount;
        return true;
    }

    void Renderer::WaitIdle() {
        if (!m_context) {
            return;
        }

        vkDeviceWaitIdle(m_context->m_device.GetHandle());
    }

    void Renderer::Shutdown() {
        // Avoid multi-shutdown call
        if (!m_context) {
            return;
        }

        // Perform cleanup
        m_context->m_syncObjects.Shutdown();
        m_context->m_commandBuffers.Shutdown();
        m_context->m_commandPool.Shutdown();
        m_context->m_graphicsPipeline.Shutdown();
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