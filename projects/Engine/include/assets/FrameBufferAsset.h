#ifndef framebufferasset_h__
#define framebufferasset_h__

#include "assets/Asset.h"
#include "graphics/api/IRenderPass.h"
#include "graphics/api/IFramebuffer.h"

class FrameBufferAsset final : public Asset
{
	friend class AssetManager;
	public:
		explicit FrameBufferAsset(const std::string& aFile);
		~FrameBufferAsset();

		[[nodiscard]] IFramebuffer* getFramebuffer() const;
		[[nodiscard]] IRenderPass* getRenderPass() const;

	private:
		IFramebuffer* mFrameBuffer;
		IRenderPass* mRenderPass;

		std::string mFileName;
		void _load() override;
};

#endif // framebufferasset_h__