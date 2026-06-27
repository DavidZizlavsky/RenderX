#include "RenderX/Vulkan/VulkanDevice.hpp"
#include "RenderX/Vulkan/VulkanQueueFamily.hpp"
#include "RenderX/Logger.hpp"
#include "RenderX/Assert.hpp"
#include <vulkan/vulkan.h>
#include <vector>
#include <set>

namespace RenderX {
	bool VulkanDevice::Initialize(VkPhysicalDevice physicalDevice, const QueueFamilyIndices& indices, VkSurfaceKHR surface) {
		// Validate indices struct
		if (!indices.IsComplete()) {
			RX_LOG_ERROR("VulkanDevice initialization requires complete queue family indices");
			return false;
		}

		// Save indices
		m_indices = indices;

		// Create logical device and print error on failure
		if (!CreateLogicalDevice(physicalDevice, indices)) {
			RX_LOG_ERROR("Failed to create logical device");
			return false;
		}

		// Get Vulkan queues
		vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);

		RX_ASSERT(m_graphicsQueue != VK_NULL_HANDLE);
		RX_ASSERT(m_presentQueue != VK_NULL_HANDLE);

		return true;
	}

	bool VulkanDevice::CreateLogicalDevice(VkPhysicalDevice physicalDevice, const QueueFamilyIndices& indices) {
		// Graphics and present family can be from the same family
		std::set<uint32_t> uniqueFamilies = {
			indices.graphicsFamily.value(),
			indices.presentFamily.value()
		};

		float priority = 1.0f;

		// Fill list of queue create infos
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		for (uint32_t family : uniqueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = family;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &priority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		// TODO: add features
		VkPhysicalDeviceFeatures deviceFeatures{};

		std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		// Fill create info for Vulkan logical device
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		// Create Vulkan logical device and save the result
		VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_device);

		// Print error on failure
		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to create VkDevice");
			return false;
		}

		return true;
	}

	void VulkanDevice::Shutdown() {
		// Avoid double cleanup
		if (m_device == VK_NULL_HANDLE) {
			return;
		}
		
		// Destroy the device
		vkDestroyDevice(m_device, nullptr);

		// Reset all member variables
		m_device = VK_NULL_HANDLE;
		m_graphicsQueue = VK_NULL_HANDLE;
		m_presentQueue = VK_NULL_HANDLE;
		m_indices = {};
	}
}