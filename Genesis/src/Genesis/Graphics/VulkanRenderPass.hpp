#pragma once

#include "Genesis/Graphics/VulkanDevice.hpp"
#include "Genesis/Graphics/VulkanSwapChain.hpp"


namespace Genesis
{
	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(VulkanDevice* device, VulkanSwapChain* swapChain);
		~VulkanRenderPass();

		VkRenderPass getRenderPass();

	private:
		VulkanDevice* device = nullptr;

		VkRenderPass renderPass;
	};
}