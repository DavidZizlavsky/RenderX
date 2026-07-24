#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace RenderX {
	/**
	 * @brief Graphics pipeline built for Vulkan 1.3 dynamic rendering (no VkRenderPass/VkFramebuffer)
	 */
	class VulkanGraphicsPipeline {
	public:
		/**
		 * @brief Creates the graphics pipeline
		 *
		 * @param device: Vulkan logical device
		 * @param colorFormat: Format of the color attachment rendered into (the swapchain's format)
		 * @param vertexShaderPath: Path to a compiled SPIR-V vertex shader
		 * @param fragmentShaderPath: Path to a compiled SPIR-V fragment shader
		 * @return True if the pipeline was successfully created
		 */
		bool Initialize(
			VkDevice device,
			VkFormat colorFormat,
			const std::string& vertexShaderPath,
			const std::string& fragmentShaderPath
		);

		/**
		 * @brief Destroys the pipeline and resets member variables
		 */
		void Shutdown();

		/**
		 * @brief Returns the VkPipeline handle
		 *
		 * @return VkPipeline handle
		 */
		VkPipeline GetHandle() const { return m_pipeline; };

		/**
		 * @brief Returns the VkPipelineLayout handle
		 *
		 * @return VkPipelineLayout handle
		 */
		VkPipelineLayout GetLayout() const { return m_layout; };
	private:
		/**
		 * @brief Reads a binary file into memory
		 *
		 * @param path: Path to the file
		 * @return File contents, or an empty vector on failure
		 */
		static std::vector<char> ReadFile(const std::string& path);

		/**
		 * @brief Wraps SPIR-V bytecode in a VkShaderModule
		 *
		 * @param code: SPIR-V bytecode
		 * @return VkShaderModule handle, or VK_NULL_HANDLE on failure
		 */
		VkShaderModule CreateShaderModule(const std::vector<char>& code);

		VkDevice m_device = VK_NULL_HANDLE;
		VkPipelineLayout m_layout = VK_NULL_HANDLE;
		VkPipeline m_pipeline = VK_NULL_HANDLE;
	};
}
