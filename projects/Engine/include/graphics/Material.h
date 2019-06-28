#ifndef material_h__
#define material_h__

#include "UniformBufferPool.h"
#include "core/PrimalCast.h"

#include "graphics/api/ICommandBuffer.h"
#include "graphics/api/IDescriptorSet.h"
#include "graphics/api/IGraphicsPipeline.h"

class Material
{
	public:
		Material(IGraphicsPipeline& aPipeline, const std::vector<UniformBufferObject*> aUBOs, Material* aParentMaterial = nullptr);
		Material(const Material&) = delete;
		Material(Material&&) noexcept = delete;
		~Material();

		Material& operator=(const Material&) = delete;
		Material& operator=(Material&&) noexcept = delete;

		bool isInstanced() const;
		
		template <typename T>
		void setVariable(const std::string& aName, T aValue);

		template <typename T>
		T getVariable(const std::string& aName);

		void use(ICommandBuffer* aBuffer);

	private:
		Material* mParentMaterial = nullptr;

		IDescriptorSet* mSet;
		std::unordered_map<std::string, void*> mVariableData;
		std::vector<UniformBufferObject*> mUBOs;
};

template <typename T>
void Material::setVariable(const std::string& aName, T aValue)
{
	mVariableData[aName] = aValue;
	for(const auto& ubo : mUBOs)
	{
		const auto element = ubo->getElement(aName);
		if(element != nullptr)
		{
			ubo->getBuffer()->setData(aValue, element->offset, element->size);
		}
	}
}

template <typename T>
T Material::getVariable(const std::string& aName)
{
	if (mVariableData.find(aName) == mVariableData.end())
		return primal_cast<T>(0);

	return primal_cast<T>(mVariableData[aName]);
}

#endif // material_h__
