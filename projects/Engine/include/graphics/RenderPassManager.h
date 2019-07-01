#ifndef renderpassmanager_h__
#define renderpassmanager_h__

#include <string>
#include <unordered_map>

#include "graphics/api/IRenderPass.h"

class RenderPassManager
{
	public:
		static RenderPassManager& instance();
		RenderPassManager(const RenderPassManager&) = delete;
		RenderPassManager(RenderPassManager&&) noexcept = delete;
		~RenderPassManager();

		RenderPassManager& operator=(const RenderPassManager&) = delete;
		RenderPassManager& operator=(RenderPassManager&&) noexcept = delete;
		
		IRenderPass* create(const std::string& aName, const RenderPassCreateInfo& aCreateInfo);
		[[nodiscard]] IRenderPass* findPass(const std::string& aName) const;

	private:
		
		RenderPassManager() = default;

		std::unordered_map<std::string, IRenderPass*> mRenderPassMap;
};

#endif // renderpassmanager_h__
