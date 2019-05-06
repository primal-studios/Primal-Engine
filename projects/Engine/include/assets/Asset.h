#ifndef asset_h__
#define asset_h__

#include <string>

class Asset
{
	public:
		explicit Asset(const std::string& aName);
		virtual ~Asset();

		virtual bool load() = 0;
		bool isLoaded() const { return mLoaded; }

	protected:
		bool mLoaded;
		std::string mName;
};

#endif // asset_h__
