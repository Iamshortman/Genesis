#pragma once

#include "Genesis/Rendering/Buffer.hpp"
#include "Genesis/Rendering/Vulkan/VulkanInstance.hpp"

namespace Genesis
{
	class VulkanBuffer : public Buffer
	{
	public:
		VulkanBuffer(VulkanInstance* instance, uint64_t size_bytes, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage);
		virtual ~VulkanBuffer();

		virtual void fill(void* data, uint64_t data_size);

		inline void* getHandle() { return (void*)this->buffer; };

	private:
		VulkanInstance* instance;
		uint64_t size;

		VkBuffer buffer;
		VmaAllocation buffer_memory;
		VmaAllocationInfo buffer_memory_info;
	};
}