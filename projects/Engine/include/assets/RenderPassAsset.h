#ifndef renderpassasset_h__
#define renderpassasset_h__

#include "assets/Asset.h"
#include "graphics/api/IRenderPass.h"

class RenderPassAsset final : public Asset
{
	friend class AssetManager;
public:
	explicit RenderPassAsset(std::string aPath);
	RenderPassAsset(const RenderPassAsset&) = delete;
	RenderPassAsset(RenderPassAsset&&) noexcept = delete;
	~RenderPassAsset() override;

	RenderPassAsset& operator=(const RenderPassAsset&) = delete;
	RenderPassAsset& operator=(RenderPassAsset&&) noexcept = delete;

	[[nodiscard]] IRenderPass* getRenderPass() const;
protected:
	void _load() override;
	AttachmentReference _process(const void* aData) const;

	const std::string mPath;
	IRenderPass* mRenderPass = nullptr;
	RenderPassCreateInfo mCreateInfo = {};
};

#endif // renderpassasset_h__
