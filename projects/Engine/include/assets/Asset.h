#ifndef asset_h__
#define asset_h__

#include <string>

class Asset
{
	friend class AssetManager;
	public:
		Asset() = default;
		virtual ~Asset() = default;

		bool isLoaded() const { return mLoaded; }

	protected:
		bool mLoaded = false;
		std::string mName = "Asset";

		virtual void _load() {}
};

#endif // asset_h__
