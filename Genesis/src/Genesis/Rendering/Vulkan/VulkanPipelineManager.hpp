#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/Vulkan/VulkanRenderPipline.hpp"

namespace Genesis
{
	class VulkanPiplineManager
	{
	public:
		VulkanPiplineManager(VulkanDevice* device);
		~VulkanPiplineManager();

		inline VkRenderPass getScreenRenderPass() { return this->screen_render_pass; };

		void buildScreenRenderPass(VkFormat image_format, VkFormat depth_format);

		VkPipelineLayout textured_mesh_layout;

		//Pipeline
		VulkanRenderPipline* textured_mesh_screen_pipeline = nullptr;

		//DescriptorSet Layouts
		VkDescriptorSetLayout textured_descriptor_layout;

	private:
		VkDevice device = VK_NULL_HANDLE;

		//Render Passes
		VkRenderPass screen_render_pass;
	};
}