#ifndef materialmanager_h__
#define materialmanager_h__

#include <unordered_map>
#include <string>

#include "graphics/Material.h"

class MaterialManager
{
	public:
		MaterialManager();
		~MaterialManager();

		Material* get(const std::string& aShader);
		Material* create(const std::string& aShader);

		bool hasMaterial(const std::string& aShader);

	private:
		std::unordered_map<std::string, Material*> mMaterials;
};

#endif // materialmanager_h__