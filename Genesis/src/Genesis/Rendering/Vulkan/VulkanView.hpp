#pragma once

#include "Genesis/Rendering/Vulkan/VulkanInclude.hpp"
#include "Genesis/Rendering/Vulkan/VulkanDevice.hpp"
#include "Genesis/Rendering/Vulkan/VulkanAllocator.hpp"
#include "Genesis/Rendering/Vulkan/VulkanMultithreadCommandBuffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanFramebuffer.hpp"

namespace Genesis
{
	class VulkanView
	{
	public:
		VulkanView(VulkanDevice* device, VulkanAllocator* allocator, uint32_t frames_in_flight, VulkanCommandPoolSet* command_pool_set, VkExtent2D size, VulkanFramebufferLayout* layout);
		~VulkanView();

		void startView(uint32_t frame);
		void endView(uint32_t frame);
		void submitView(uint32_t frame, vector<VulkanView*> sub_views, VkFence view_done_fence);

		inline VkSemaphore getWaitSemaphore(uint32_t i) { return this->view_done_semaphores[i]; };
		inline VkExtent2D getViewSize() { return this->size; };

		inline void setClearValues(Array<VkClearValue>& clear) { this->clear_values = clear; };

	private:
		VulkanDevice* device = nullptr;
		VkRenderPass render_pass;
		VkExtent2D size;

		Array<VkSemaphore> view_done_semaphores;
		Array<VulkanMultithreadCommandBuffer*> command_buffers;
		Array<VulkanFramebuffer*> framebuffers;

		Array<VkClearValue> clear_values;
	};
}