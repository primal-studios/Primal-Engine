#ifndef shaderasset_h__
#define shaderasset_h__

#include "assets/Asset.h"
#include "graphics/api/IGraphicsPipeline.h"

class ShaderAsset final : public Asset
{
	public:
		explicit ShaderAsset(const std::string& aPath);
		~ShaderAsset();

	private:
		void _load() override;

		GraphicsPipelineCreateInfo mGraphicsPipelineCreateInfo;
		std::string mPath;
};

#endif // shaderasset_h__