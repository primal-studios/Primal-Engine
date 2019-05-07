#ifndef asset_h__
#define asset_h__

#include <string>

class Asset
{
	friend class AssetManager;
	public:
		Asset() = default;
		virtual ~Asset() = default;

		virtual bool load() = 0;
		bool isLoaded() const { return mLoaded; }

	protected:
		bool mLoaded;
		std::string mName;
};

#endif // asset_h__
