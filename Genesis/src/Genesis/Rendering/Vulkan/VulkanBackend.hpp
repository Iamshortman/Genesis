#pragma once

#include "Genesis/Rendering/RenderingBackend.hpp"
#include "Genesis/Platform/Window.hpp"

namespace Genesis
{
	//Prototype
	class VulkanInstance;

	class VulkanBackend : public RenderingBackend
	{
	public:
		VulkanBackend(Window* window, uint32_t number_of_threads);
		~VulkanBackend();

		virtual bool beginFrame() override;
		virtual void endFrame() override;

		virtual Buffer* createBuffer(uint64_t size_bytes, BufferType type, MemoryUsage memory_usage) override;

		virtual TextureIndex createTexture(vector2U size) override;
		virtual void fillTexture(TextureIndex texture_index, void* data, uint64_t data_size) override;
		virtual void destroyTexture(TextureIndex texture_index) override;

		virtual void drawMeshScreen(uint32_t thread, Buffer* vertex_buffer, Buffer* index_buffer, uint32_t index_count, matrix4F mvp) override;

		virtual void waitTillDone() override;

	private:
		VulkanInstance* vulkan = nullptr;
		uint32_t swapchain_image_index = 0;
		uint32_t frame_index = 0;
	};
}