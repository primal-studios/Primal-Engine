#ifndef irenderpass_h__
#define irenderpass_h__

#include "graphics/AccessFlags.h"
#include "graphics/AttachmentOperations.h"
#include "graphics/DataFormat.h"
#include "graphics/DependencyFlags.h"
#include "graphics/PipelineStageFlags.h"
#include "graphics/api/IGraphicsContext.h"
#include "graphics/api/IImage.h"

#include <cstdint>
#include <vector>

struct AttachmentDescription
{
	EDataFormat format;
	EImageSampleFlagBits samples;
	EAttachmentLoadOperation load;
	EAttachmentStoreOperation store;
	EAttachmentLoadOperation loadStencil;
	EAttachmentStoreOperation storeStencil;
	EImageLayout initial;
	EImageLayout final;
};

struct AttachmentReference
{
	uint32_t index = 0xFFFFFFFF;
	EImageLayout layout;
};

struct SubPassDescription
{
	std::vector<AttachmentReference> inputs;
	std::vector<AttachmentReference> colors;
	std::vector<AttachmentReference> resolve;
	AttachmentReference depthStencil;
	std::vector<uint32_t> preserve;
};

struct SubPassDependency
{
	uint32_t srcSubPass;
	uint32_t dstSubPass;
	PipelineStageFlags srcStages;
	PipelineStageFlags dstStages;
	AccessFlags srcAccess;
	AccessFlags dstAccess;
	DependencyFlags dependencyFlags;
};

struct RenderPassCreateInfo
{
	std::vector<AttachmentDescription> attachments;
	std::vector<SubPassDescription> descriptions;
	std::vector<SubPassDependency> dependencies;
};

class IRenderPass
{
	public:
		explicit IRenderPass(IGraphicsContext* aContext);
		IRenderPass(const IRenderPass&) = delete;
		IRenderPass(IRenderPass&&) noexcept = delete;
		virtual ~IRenderPass() = default;

		IRenderPass& operator=(const IRenderPass&) = delete;
		IRenderPass& operator=(IRenderPass&&) noexcept = delete;

		virtual void construct(const RenderPassCreateInfo&) = 0;
};

#endif // irenderpass_h__
