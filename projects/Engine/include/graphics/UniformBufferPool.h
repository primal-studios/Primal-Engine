#ifndef uniformbufferpool_h__
#define uniformbufferpool_h__

#include <cstdint>
#include <unordered_set>
#include <vector>

#include "graphics/GraphicsFactory.h"

struct UniformBufferObject
{
	IUniformBuffer* buffer;
	uint32_t offset;
	uint32_t size;
	uint32_t poolIndex;
};

class UniformBufferPool
{
	public:
		explicit UniformBufferPool(uint32_t aChunkSize, uint32_t aStride, UniformBufferCreateInfo aUboCreateInfo);
		UniformBufferPool(const UniformBufferPool&) = delete;
		UniformBufferPool(UniformBufferPool&&) noexcept = delete;
		~UniformBufferPool();

		UniformBufferPool& operator=(const UniformBufferPool&) = delete;
		UniformBufferPool& operator=(UniformBufferPool&&) noexcept = delete;

		UniformBufferObject acquire();
		UniformBufferObject acquire(uint32_t aIndex);
		void release(UniformBufferObject aObject);

	private:
		std::vector<IUniformBuffer*> mBuffers;
		std::unordered_set<uint32_t> mFreeSlots;
		UniformBufferCreateInfo mCreateInfo;
		uint32_t mCursor;
		uint32_t mChunkSize;
		uint32_t mStride;
};

#endif // uniformbufferpool_h__
