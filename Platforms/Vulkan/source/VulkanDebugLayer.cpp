#include "VulkanDebugLayer.hpp"

#include "Genesis/Debug/Assert.hpp"
#include "Genesis/Debug/Log.hpp"

using namespace Genesis;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	if (strcmp(pCallbackData->pMessageIdName, "UNASSIGNED-CoreValidation-DrawState-InvalidImageLayout") == 0)
	{
		//Ignore this warning for now
		return VK_FALSE;
	}

	GENESIS_ENGINE_DEBUG("Vulkan Validation Layer: {}", pCallbackData->pMessage);
	return VK_FALSE;
}

VulkanDebugLayer::VulkanDebugLayer(VkInstance instance)
{
	this->instance = instance;

	VkDebugUtilsMessengerCreateInfoEXT create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = debugCallback;

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(this->instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		GENESIS_ENGINE_ASSERT_ERROR((func(this->instance, &create_info, nullptr, &this->debug_messenger) == VK_SUCCESS), "failed to set up debug messenger");
	}
	else
	{
		GENESIS_ENGINE_ERROR("failed to set up debug messenger");
	}
}

VulkanDebugLayer::~VulkanDebugLayer()
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(this->instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(this->instance, this->debug_messenger, nullptr);
	}
}
