#pragma once
#include <iostream>

#define RX_LOG_INFO(message) \
	std::cout << "[RenderX][INFO] " << message << std::endl;

#define RX_LOG_WARNING(message) \
	std::cout << "[RenderX][WARNING] " << message << std::endl;

#define RX_LOG_ERROR(message) \
	std::cerr << "[RenderX][ERROR] " << message << std::endl;

#define RX_LOG_VULKAN_MESSENGER(message, severity) \
	std::cerr << "[Vulkan][" << severity << "] " << message << std::endl;