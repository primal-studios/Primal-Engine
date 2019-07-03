#ifndef shaderasset_h__
#define shaderasset_h__

#include "assets/Asset.h"
#include "graphics/api/IGraphicsPipeline.h"

class ShaderAsset final : public Asset
{
	friend class AssetManager;
	public:
		explicit ShaderAsset(const std::string& aPath);
		~ShaderAsset();

		[[nodiscard]] IGraphicsPipeline* getPipeline() const;
		[[nodiscard]] IPipelineLayout* getLayout() const;
	private:
		void _load() override;

		GraphicsPipelineCreateInfo mGraphicsPipelineCreateInfo;
		IGraphicsPipeline* mPipeline;
		std::string mPath;
};

#endif // shaderasset_h__