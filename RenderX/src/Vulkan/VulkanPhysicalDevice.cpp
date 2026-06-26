#include "RenderX/Vulkan/VulkanPhysicalDevice.hpp"
#include "RenderX/Vulkan/VulkanQueueFamily.hpp"
#include "RenderX/Logger.hpp"
#include "RenderX/Assert.hpp"
#include <vulkan/vulkan.h>
#include <vector>

namespace RenderX {
	bool VulkanPhysicalDevice::Initialize(VkInstance instance, VkSurfaceKHR surface) {
		// Get physical device count
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		// Get list of physical devices
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		// Choose the most suitable device
		m_device = GetBestDevice(devices, surface);

		// Return false if suitable device was not found
		if (m_device == VK_NULL_HANDLE) {
			RX_LOG_ERROR("No suitable physical device found");
			return false;
		}

		// Get info
		vkGetPhysicalDeviceProperties(m_device, &m_info.properties);
		vkGetPhysicalDeviceFeatures(m_device, &m_info.features);
		vkGetPhysicalDeviceMemoryProperties(m_device, &m_info.memory);
		m_info.queueFamilies = VulkanQueueFamily::Find(m_device, surface);

		// Log selected GPU
		RX_LOG_INFO("Using GPU: " << m_info.properties.deviceName);
		return true;
	}

	VkPhysicalDevice VulkanPhysicalDevice::GetBestDevice(const std::vector<VkPhysicalDevice>& devices, VkSurfaceKHR surface) {
		VkPhysicalDevice selectedDevice = VK_NULL_HANDLE;
		int bestScore = 0;
		
		for (VkPhysicalDevice device : devices) {
			// Check if this device can even be used
			if (!IsDeviceSuitable(device, surface)) {
				continue;
			}

			// Get device rating
			int score = RateDevice(device);

			// Pick device with the best score
			if (score > bestScore) {
				selectedDevice = device;
				bestScore = score;
			}
		}

		return selectedDevice;
	}

	int VulkanPhysicalDevice::RateDevice(VkPhysicalDevice device) {
		RX_ASSERT(device != VK_NULL_HANDLE);

		int score = 0;

		// Gets properties of the physical device
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(device, &properties);

		// Add score based on the device type
		switch (properties.deviceType) {
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			score += 1000;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			score += 500;
			break;
		}

		return score;
	}

	bool VulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
		// TODO: add extension check and swapchain support check
		QueueFamilyIndices indices = VulkanQueueFamily::Find(device, surface);

		return indices.IsComplete();
	}

	void VulkanPhysicalDevice::Shutdown() {
		// Avoid double shutdown
		if (m_device == VK_NULL_HANDLE) {
			return;
		}

		m_device = VK_NULL_HANDLE;
		m_info = {};
	}
}