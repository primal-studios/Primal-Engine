#include <vulkan/vulkan.h>

#include <graphics/vk/VulkanGraphicsContext.h>
#include <GLFW/glfw3.h>
#include <core/Log.h>
#include <core/PrimalAssert.h>

#include <map>
#include <unordered_set>

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

	static bool sIsDeviceSuitable(VkPhysicalDevice aDevice, VkSurfaceKHR aSurface, std::vector<const char*> aExtensions)
	{
		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(aDevice, aSurface, &capabilities);

		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(aDevice, aSurface, &formatCount, nullptr);

		std::vector<VkSurfaceFormatKHR> formats;

		if (formatCount > 0)
		{
			formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(aDevice, aSurface, &formatCount, formats.data());
		}

		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(aDevice, aSurface, &presentModeCount, nullptr);
		
		std::vector<VkPresentModeKHR> presentModes;

		if(presentModeCount > 0)
		{
			presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(aDevice, aSurface, &presentModeCount, presentModes.data());
		}

		const bool swapChainAdequate = !presentModes.empty() && !formats.empty();
		
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(aDevice, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions;
		availableExtensions.resize(extensionCount);

		vkEnumerateDeviceExtensionProperties(aDevice, nullptr, &extensionCount, availableExtensions.data());

		std::unordered_set<std::string> requiredExtensions;
		for (const auto& extension : aExtensions)
		{
			requiredExtensions.insert(extension);
		}

		for(const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}
		
		const bool extensionsSupported = requiredExtensions.empty();

		return swapChainAdequate && extensionsSupported;
	}
}

VulkanGraphicsContext::VulkanGraphicsContext(const GraphicsContextCreateInfo& aCreateInfo)
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

	_createPhysicalDevice();
	_createLogicalDevice();
}

VulkanGraphicsContext::~VulkanGraphicsContext()
{
	if (mCreateInfo.window)
	{
		vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
	}

	DEBUG_ONLY_BLOCK({
		const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(mInstance, "vkDestroyDebugUtilsMessengerEXT"));
		if (func)
		{
			func(mInstance, mDebugMessenger, nullptr);
		}
	})

	vkDestroyInstance(mInstance, nullptr);
}

void VulkanGraphicsContext::idle() const
{
	vkDeviceWaitIdle(mDevice);
}

VkSurfaceKHR VulkanGraphicsContext::getSurfaceHandle() const
{
	return mSurface;
}

VkDevice VulkanGraphicsContext::getDevice() const
{
	return mDevice;
}

uint32_t VulkanGraphicsContext::getGraphicsQueueIndex() const
{
	return mGraphicsQueueFamily;
}

uint32_t VulkanGraphicsContext::getPresentQueueIndex() const
{
	return mPresentQueueFamily;
}

void VulkanGraphicsContext::_initializeVulkan()
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

	DEBUG_ONLY_BLOCK({
		if (!validationLayerSupport)
		{
			PRIMAL_INTERNAL_CRITICAL("Vulkan validation layers requested, but could not be loaded.");
		}
		else
		{
			PRIMAL_INTERNAL_INFO("Vulkan validation layers requested and available.");
			mValidationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
			createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
			createInfo.ppEnabledLayerNames = mValidationLayers.data();
		}
	})

	RELEASE_ONLY_BLOCK({
		PRIMAL_INTERNAL_INFO("Vulkan validation layers not requested.");
	})

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

void VulkanGraphicsContext::_initializeDebugMessenger()
{
	DEBUG_ONLY_BLOCK({
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
	})
}

bool VulkanGraphicsContext::_checkValidationLayerSupport() const
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

void VulkanGraphicsContext::_createPhysicalDevice()
{
	using namespace std;

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to find GPU with Vulkan Support.");
		return;
	}

	vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

	multimap<int, VkPhysicalDevice> deviceMap;

	for (const auto & device : devices)
	{
		if (detail::sIsDeviceSuitable(device, mSurface, mDeviceExtensions))
		{
			VkPhysicalDeviceProperties props;
			vkGetPhysicalDeviceProperties(device, &props);
			VkPhysicalDeviceFeatures feats;
			vkGetPhysicalDeviceFeatures(device, &feats);

			int score = 0;

			if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				score += 1000;
			} 
			else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
			{
				score += 100;
			}

			if (!feats.geometryShader)
			{
				score = 0;
			}

			deviceMap.insert({ score, device });
		}
	}

	const auto last = deviceMap.rbegin();
	mPhysicalDevice = last->second;

	DEBUG_ONLY_BLOCK({
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(mPhysicalDevice, &props);
		VkPhysicalDeviceFeatures feats;
		vkGetPhysicalDeviceFeatures(mPhysicalDevice, &feats);

		PRIMAL_INTERNAL_INFO("Selected Device: {0}", props.deviceName);
	})
}

void VulkanGraphicsContext::_createLogicalDevice()
{
	using namespace std;

	DeviceQueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, nullptr);
	vector<VkQueueFamilyProperties> props;
	props.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, props.data());

	uint32_t i = 0;
	for (const auto& queueFamily : props)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, i, mSurface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.graphicsFamily.has_value() && indices.presentFamily.has_value())
		{
			break;
		}

		i++;
	}

	mGraphicsQueueFamily = indices.graphicsFamily.value();
	mPresentQueueFamily = indices.presentFamily.value();

	vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	unordered_set<uint32_t> uniqueFamilies;
	uniqueFamilies.insert(indices.graphicsFamily.value());
	uniqueFamilies.insert(indices.presentFamily.value());

	float queuePriority = 0.0f;

	if (queueFamilyCount == 1)
	{
		queuePriority = 1.0f;

		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	else
	{
		queuePriority = 1.0f / static_cast<float>(uniqueFamilies.size());
		for (uint32_t queueFamily : uniqueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(mDeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = mDeviceExtensions.data();

	DEBUG_RELEASE_TOGGLE_BLOCK({
		createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
		createInfo.ppEnabledLayerNames = mValidationLayers.data();
		createInfo.enabledLayerCount = 0;
	},
	{
		createInfo.enabledLayerCount = 0;
	})

	const VkResult res = vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice);
	if (res != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create Vulkan device.");
		return;
	}
	PRIMAL_INTERNAL_INFO("Successfully created Vulkan device.");
}

	std::vector<const char*> VulkanGraphicsContext::_getRequiredExtensions() const
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
