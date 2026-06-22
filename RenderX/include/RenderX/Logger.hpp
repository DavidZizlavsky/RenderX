#pragma once
#include "RenderX/Assert.hpp"
#include <iostream>
#include <string>

namespace RenderX {
	namespace Logger {
		static void Info(std::string message) {
			RENDERX_ASSERT(message.length() > 0);

			std::cout << "[RenderX] " << message << std::endl;
		}
		static void Error(std::string message) {
			RENDERX_ASSERT(message.length() > 0);

			std::cout << "[RenderX] " << message << std::endl;
		}
		static void VulkanDebug(std::string message, std::string severity) {
			RENDERX_ASSERT(message.length() > 0);
			RENDERX_ASSERT(severity.length() > 0);

			std::cout << "[Vulkan] [" << severity << "] " << message << std::endl;
		}
	}
}