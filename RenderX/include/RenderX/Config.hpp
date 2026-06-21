#pragma once
#include <vector>

// RENDERX_VERSION allows you to make version into uint32_t datatype
#define RENDERX_VERSION(variant, major, minor, patch) \
    ((((uint32_t)(variant)) << 29U) | (((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))

namespace RenderX {
	/**
	 * @brief Config struct allows you to change settings of the RenderX library
	 */
	struct Config {
		const char* applicationName = "RenderX app";
		uint32_t applicationVersion = RENDERX_VERSION(1, 0, 0, 0);
		const char* engineName = "RenderX";
		uint32_t engineVersion = RENDERX_VERSION(1, 0, 0, 0);

		std::vector<const char*> extensions;
		std::vector<const char*> layers;

		bool debugging = false;
	};
}