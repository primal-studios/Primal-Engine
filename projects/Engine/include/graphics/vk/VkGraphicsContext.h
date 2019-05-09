#ifndef vkgraphicscontext_h__
#define vkgraphicscontext_h__

#include <graphics/api/IGraphicsContext.h>

#include <vector>
#include <vulkan/vulkan.h>

class VkGraphicsContext final : public IGraphicsContext
{
public:
	explicit VkGraphicsContext(const GraphicsContextCreateInfo& aCreateInfo);
	VkGraphicsContext(const VkGraphicsContext&) = delete;
	VkGraphicsContext(VkGraphicsContext&&) noexcept = delete;
	~VkGraphicsContext() override;

	VkGraphicsContext& operator=(const VkGraphicsContext&) = delete;
	VkGraphicsContext& operator=(VkGraphicsContext&&) noexcept = delete;

	void idle() const override;
	uint64_t getSurfaceHandle() const override;
private:
	void _initializeVulkan();
	void _initializeDebugMessenger();
	bool _checkValidationLayerSupport() const;
	void _createPhysicalDevice();
	std::vector<const char*> _getRequiredExtensions() const;

	std::vector<VkExtensionProperties> mProps;
	std::vector<const char*> mValidationLayers;
	std::vector<const char*> mDeviceExtensions;

	VkInstance mInstance{};
	VkDevice mDevice{};
	VkPhysicalDevice mPhysicalDevice{};
	VkSurfaceKHR mSurface{};
	VkDebugUtilsMessengerEXT mDebugMessenger{};
	GraphicsContextCreateInfo mCreateInfo;

#if defined (_DEBUG)
	const bool mDebugLayersEnabled = true;
#else
	const bool mDebugLayersEnabled = false;
#endif
};

#endif // vkgraphicscontext_h__
