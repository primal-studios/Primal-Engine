#ifndef shaderasset_h__
#define shaderasset_h__

#include "assets/Asset.h"

class ShaderAsset final : public Asset
{
	friend class AssetManager;
	public:
		explicit ShaderAsset(const std::string& aPath);
		~ShaderAsset();
	
	private:
		void _load() override;

		std::string mPath;
};

#endif // shaderasset_h__