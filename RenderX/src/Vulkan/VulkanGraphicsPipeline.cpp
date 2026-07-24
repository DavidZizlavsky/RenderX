#include "RenderX/Vulkan/VulkanGraphicsPipeline.hpp"
#include "RenderX/Assert.hpp"
#include "RenderX/Logger.hpp"
#include <vulkan/vulkan.h>
#include <fstream>
#include <vector>

namespace RenderX {
	bool VulkanGraphicsPipeline::Initialize(
		VkDevice device,
		VkFormat colorFormat,
		const std::string& vertexShaderPath,
		const std::string& fragmentShaderPath
	) {
		RX_ASSERT(device != VK_NULL_HANDLE);
		m_device = device;

		std::vector<char> vertexCode = ReadFile(vertexShaderPath);
		std::vector<char> fragmentCode = ReadFile(fragmentShaderPath);

		if (vertexCode.empty() || fragmentCode.empty()) {
			RX_LOG_ERROR("Failed to read shader bytecode");
			return false;
		}

		VkShaderModule vertexModule = CreateShaderModule(vertexCode);
		VkShaderModule fragmentModule = CreateShaderModule(fragmentCode);

		if (vertexModule == VK_NULL_HANDLE || fragmentModule == VK_NULL_HANDLE) {
			vkDestroyShaderModule(device, vertexModule, nullptr);
			vkDestroyShaderModule(device, fragmentModule, nullptr);
			return false;
		}

		VkPipelineShaderStageCreateInfo vertexStage{};
		vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertexStage.module = vertexModule;
		vertexStage.pName = "main";

		VkPipelineShaderStageCreateInfo fragmentStage{};
		fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragmentStage.module = fragmentModule;
		fragmentStage.pName = "main";

		VkPipelineShaderStageCreateInfo stages[] = { vertexStage, fragmentStage };

		// No vertex buffers yet - the triangle's positions/colors are hard-coded in the vertex shader
		VkPipelineVertexInputStateCreateInfo vertexInput{};
		vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		// Viewport/scissor are set per-frame via vkCmdSetViewport/vkCmdSetScissor (dynamic state) so the
		// pipeline doesn't need to be rebuilt when the swapchain is recreated on window resize.
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;

		VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;

		// No descriptor sets/push constants yet
		VkPipelineLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		VkResult result = vkCreatePipelineLayout(device, &layoutInfo, nullptr, &m_layout);
		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to create pipeline layout");
			vkDestroyShaderModule(device, vertexModule, nullptr);
			vkDestroyShaderModule(device, fragmentModule, nullptr);
			return false;
		}

		// Dynamic rendering: describe the attachment formats directly instead of referencing a VkRenderPass
		VkPipelineRenderingCreateInfo renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachmentFormats = &colorFormat;

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext = &renderingInfo;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = stages;
		pipelineInfo.pVertexInputState = &vertexInput;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = m_layout;
		pipelineInfo.renderPass = VK_NULL_HANDLE;

		result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline);

		vkDestroyShaderModule(device, vertexModule, nullptr);
		vkDestroyShaderModule(device, fragmentModule, nullptr);

		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to create graphics pipeline");
			vkDestroyPipelineLayout(device, m_layout, nullptr);
			m_layout = VK_NULL_HANDLE;
			return false;
		}

		return true;
	}

	std::vector<char> VulkanGraphicsPipeline::ReadFile(const std::string& path) {
		std::ifstream file(path, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			RX_LOG_ERROR("Failed to open shader file: " << path);
			return {};
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), static_cast<std::streamsize>(fileSize));

		return buffer;
	}

	VkShaderModule VulkanGraphicsPipeline::CreateShaderModule(const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule = VK_NULL_HANDLE;
		VkResult result = vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule);
		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to create shader module");
			return VK_NULL_HANDLE;
		}

		return shaderModule;
	}

	void VulkanGraphicsPipeline::Shutdown() {
		// Avoid double shutdown
		if (m_device == VK_NULL_HANDLE) {
			return;
		}

		vkDestroyPipeline(m_device, m_pipeline, nullptr);
		vkDestroyPipelineLayout(m_device, m_layout, nullptr);

		m_device = VK_NULL_HANDLE;
		m_pipeline = VK_NULL_HANDLE;
		m_layout = VK_NULL_HANDLE;
	}
}
