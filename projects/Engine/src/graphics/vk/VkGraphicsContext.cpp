#include <vulkan/vulkan.h>
#include <graphics/vk/VkGraphicsContext.h>

#include <GLFW/glfw3.h>
#include <core/Log.h>

namespace detail
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL sDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT aSeverity,
		VkDebugUtilsMessageTypeFlagsEXT aType, const VkDebugUtilsMessengerCallbackDataEXT* aCallbackData,
		void* aUserData)
	{
		if (aSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			PRIMAL_INTERNAL_ERROR("Validation Layer: {0}", aCallbackData->pMessage);
		}
		else if (aSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT || aSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
		{
			PRIMAL_INTERNAL_INFO("Validation Layer: {0}", aCallbackData->pMessage);
		}
		else if (aSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			PRIMAL_INTERNAL_WARN("Validation Layer: {0}", aCallbackData->pMessage);
		}
		return VK_FALSE;
	}
}

VkGraphicsContext::VkGraphicsContext(const GraphicsContextCreateInfo& aCreateInfo)
	: IGraphicsContext(aCreateInfo), mCreateInfo(aCreateInfo)
{
	_initializeVulkan();
	_initializeDebugMessenger();

	if (mCreateInfo.window)
	{
		const VkResult result = glfwCreateWindowSurface(mInstance, mCreateInfo.window, nullptr, &mSurface);
		if (result != VK_SUCCESS)
		{
			PRIMAL_INTERNAL_CRITICAL("Failed to create Vulkan window surface.");
		}
		else
		{
			PRIMAL_INTERNAL_INFO("Successfully created Vulkan window surface.");
		}
	}
}

VkGraphicsContext::~VkGraphicsContext()
{
	if (mCreateInfo.window)
	{
		vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
	}

	if (mDebugLayersEnabled)
	{
		const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
			mInstance, "vkDestroyDebugUtilsMessengerEXT"));
		if (func)
		{
			func(mInstance, mDebugMessenger, nullptr);
		}
	}
	vkDestroyInstance(mInstance, nullptr);
}

void VkGraphicsContext::idle() const
{
}

uint64_t VkGraphicsContext::getSurfaceHandle() const
{
	return reinterpret_cast<uint64_t>(mSurface);
}

void VkGraphicsContext::_initializeVulkan()
{
	using namespace std;

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = mCreateInfo.applicationName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(mCreateInfo.versionMajor, mCreateInfo.versionMinor, mCreateInfo.versionPatch);
	appInfo.pEngineName = "Primal Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = 0;

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	mProps.resize(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, mProps.data());

	for (const auto & prop : mProps)
	{
		PRIMAL_INTERNAL_INFO("Vulkan Extension Property Found: {0}", prop.extensionName);
	}

	const bool validationLayerSupport = _checkValidationLayerSupport();

	if (!validationLayerSupport && mDebugLayersEnabled)
	{
		PRIMAL_INTERNAL_CRITICAL("Vulkan validation layers requested, but could not be loaded.");
	}
	else if (mDebugLayersEnabled)
	{
		PRIMAL_INTERNAL_INFO("Vulkan validation layers requested and available.");
		mValidationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
		createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
		createInfo.ppEnabledLayerNames = mValidationLayers.data();
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Vulkan validation layers not requested.");
	}

	mDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	vector<const char*> extensions = _getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	const VkResult result = vkCreateInstance(&createInfo, nullptr, &mInstance);

	if (result != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create Vulkan instance.");
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Successfully created Vulkan Instance.");
	}
}

void VkGraphicsContext::_initializeDebugMessenger()
{
	if (!mDebugLayersEnabled)
	{
		return;
	}

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = detail::sDebugCallback;
	createInfo.pUserData = nullptr;

	const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
		mInstance, "vkCreateDebugUtilsMessengerEXT"));

	if (func)
	{
		VkResult result = func != nullptr ? func(mInstance, &createInfo, nullptr, &mDebugMessenger) : VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

bool VkGraphicsContext::_checkValidationLayerSupport() const
{
	using namespace std;

	vector<const char*> validationLayers;
	validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");

	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	vector<VkLayerProperties> layerProperties;
	layerProperties.resize(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

	for (const char* layerName : validationLayers)
	{
		bool found = false;
		for (const auto& layerProp : layerProperties)
		{
			if (strcmp(layerName, layerProp.layerName) == 0)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			return false;
		}
	}

	return true;
}

std::vector<const char*> VkGraphicsContext::_getRequiredExtensions() const
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions;
	extensions.reserve(glfwExtensionCount);

	for (uint32_t i = 0; i < glfwExtensionCount; i++)
	{
		PRIMAL_INTERNAL_INFO("GLFW Vulkan Extension Found: {0}", glfwExtensions[i]);
		extensions.push_back(glfwExtensions[i]);
	}

#if defined (_DEBUG)
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	return extensions;
}
