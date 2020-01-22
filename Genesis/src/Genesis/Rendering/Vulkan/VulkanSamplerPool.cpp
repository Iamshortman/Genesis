#include "VulkanSamplerPool.hpp"

#include "Genesis/Debug/Assert.hpp"

using namespace Genesis;

VulkanSamplerPool::VulkanSamplerPool(VkDevice device)
{
	this->device = device;
}

VulkanSamplerPool::~VulkanSamplerPool()
{
	for (auto sampler : this->samplers)
	{
		vkDestroySampler(this->device, sampler.second, nullptr);
	}
}

VkFilter getFilterMode(FilterMode mode)
{
	switch (mode)
	{
	case Genesis::FilterMode::Nearest:
		return VK_FILTER_NEAREST;
	case Genesis::FilterMode::Linear:
		return VK_FILTER_LINEAR;
	}
	return VK_FILTER_NEAREST;
}

VkSamplerMipmapMode getMipmapMode(FilterMode mode)
{
	switch (mode)
	{
	case Genesis::FilterMode::Nearest:
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
	case Genesis::FilterMode::Linear:
		return VK_SAMPLER_MIPMAP_MODE_LINEAR;
	}
	return VK_SAMPLER_MIPMAP_MODE_NEAREST;
}

VkSamplerAddressMode getAddressMode(AddressMode mode)
{
	switch (mode)
	{
	case Genesis::AddressMode::Repeat:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	case Genesis::AddressMode::Mirrored_Repeat:
		return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	case Genesis::AddressMode::Clamp_Edge:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	case Genesis::AddressMode::Clamp_Border:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	}
	return VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

VkCompareOp getCompareOp(CompareOp op)
{
	switch (op)
	{
	case Genesis::CompareOp::Never:
		return VK_COMPARE_OP_NEVER;
	case Genesis::CompareOp::Less:
		return VK_COMPARE_OP_LESS;
	case Genesis::CompareOp::Equal:
		return VK_COMPARE_OP_EQUAL;
	case Genesis::CompareOp::Less_Equal:
		return VK_COMPARE_OP_LESS_OR_EQUAL;
	case Genesis::CompareOp::Greater:
		return VK_COMPARE_OP_GREATER;
	case Genesis::CompareOp::Not_Equal:
		return VK_COMPARE_OP_NOT_EQUAL;
	case Genesis::CompareOp::Greater_Equal:
		return VK_COMPARE_OP_GREATER_OR_EQUAL;
	case Genesis::CompareOp::Always:
		return VK_COMPARE_OP_ALWAYS;
	}
	return VK_COMPARE_OP_NEVER;
}

//Using ints for the moment, may change
VkBorderColor getBorderColor(BorderColor color)
{
	switch (color)
	{
	case Genesis::BorderColor::Transparent_Black:
		return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
	case Genesis::BorderColor::Opaque_Black:
		return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	case Genesis::BorderColor::Opaque_White:
		return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
	}
	return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
}

VkSampler VulkanSamplerPool::getSampler(Sampler& sampler)
{
	uint32_t hash_value = sampler.getHash();
	VkSampler return_sampler = VK_NULL_HANDLE;

	this->map_lock.lock();
	if (!has_value(this->samplers, hash_value))
	{
		VkSamplerCreateInfo sampler_info = {};
		sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_info.magFilter = getFilterMode(sampler.mag_filter);
		sampler_info.minFilter = getFilterMode(sampler.min_filter);
		sampler_info.mipmapMode = getMipmapMode(sampler.mipmap_mode);
		sampler_info.addressModeU = getAddressMode(sampler.U_address_mode);
		sampler_info.addressModeV = getAddressMode(sampler.V_address_mode);
		sampler_info.addressModeW = getAddressMode(sampler.W_address_mode);
		sampler_info.mipLodBias = sampler.mip_lod_bias;
		sampler_info.anisotropyEnable = (sampler.max_anisotropy == 0) ? VK_FALSE : VK_TRUE;
		sampler_info.maxAnisotropy = (float)sampler.max_anisotropy;
		sampler_info.compareEnable = (sampler.compare_op == CompareOp::Never) ? VK_FALSE : VK_TRUE;
		sampler_info.compareOp = getCompareOp(sampler.compare_op);
		sampler_info.minLod = sampler.min_lod;
		sampler_info.maxLod = sampler.max_lod;
		sampler_info.borderColor = getBorderColor(sampler.border_color);
		GENESIS_ENGINE_ASSERT_ERROR((vkCreateSampler(this->device, &sampler_info, nullptr, &return_sampler) == VK_SUCCESS), "failed to create texture sampler");

		this->samplers[hash_value] = return_sampler;
	}

	return_sampler = this->samplers[hash_value];
	this->map_lock.unlock();
	return return_sampler;
}
