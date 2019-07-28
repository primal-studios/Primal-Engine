#ifndef materialmanager_h__
#define materialmanager_h__

#include <unordered_map>
#include <string>

#include "graphics/Material.h"

class MaterialManager
{
	friend class Material;
	friend void detail::PruneNode(MaterialGraphNode* aNode);

	MaterialManager() = default;
public:
	static MaterialManager& instance();

	void prune(Material* aMaterial);
	void pruneAll();
	void reset();

	Material* createMaterial(const MaterialCreateInfo& aInfo);
private:
	static MaterialManager sManager;

	std::vector<Material*> mRootMaterials;
};

#endif // materialmanager_h__