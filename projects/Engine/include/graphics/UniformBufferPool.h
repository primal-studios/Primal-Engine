#ifndef uniformbufferpool_h__
#define uniformbufferpool_h__

#include <cstdint>
#include <unordered_set>
#include <vector>

#include "graphics/GraphicsFactory.h"

enum class EUniformBufferObjectElementType
{
	UBO_TYPE_BOOL,
	UBO_TYPE_INT,
	UBO_TYPE_UINT,
	UBO_TYPE_FLOAT,
	UBO_TYPE_DOUBLE,
	UBO_TYPE_BOOL_VEC2,
	UBO_TYPE_BOOL_VEC3,
	UBO_TYPE_BOOL_VEC4,
	UBO_TYPE_INT_VEC2,
	UBO_TYPE_INT_VEC3,
	UBO_TYPE_INT_VEC4,
	UBO_TYPE_UINT_VEC2,
	UBO_TYPE_UINT_VEC3,
	UBO_TYPE_UINT_VEC4,
	UBO_TYPE_FLOAT_VEC2,
	UBO_TYPE_FLOAT_VEC3,
	UBO_TYPE_FLOAT_VEC4,
	UBO_TYPE_DOUBLE_VEC2,
	UBO_TYPE_DOUBLE_VEC3,
	UBO_TYPE_DOUBLE_VEC4,
	UBO_TYPE_MAT2,
	UBO_TYPE_MAT2X3,
	UBO_TYPE_MAT2X4,
	UBO_TYPE_MAT3X2,
	UBO_TYPE_MAT3,
	UBO_TYPE_MAT3X4,
	UBO_TYPE_MAT4X2,
	UBO_TYPE_MAT4X3,
	UBO_TYPE_MAT4
};

struct UniformBufferObjectElement
{
	std::string name;
	EUniformBufferObjectElementType type;
	uint32_t offset;
	uint32_t size;
};

class UniformBufferObject
{
	public:
		explicit UniformBufferObject(IUniformBuffer* aBuffer, uint32_t aOffset, uint32_t aSize, uint32_t aPoolIndex, const std::vector<UniformBufferObjectElement*>& aElements);
		UniformBufferObject(const UniformBufferObject&) = default;
		UniformBufferObject(UniformBufferObject&&) noexcept = default;
		~UniformBufferObject() = default;
		UniformBufferObject& operator=(const UniformBufferObject&) = delete;
		UniformBufferObject& operator=(UniformBufferObject&&) noexcept = delete;

		const std::vector<UniformBufferObjectElement*>& getElements() const;
		const UniformBufferObjectElement* getElement(const std::string& aName) const;
		IUniformBuffer* getBuffer() const;
		uint32_t getOffset() const;
		uint32_t getSize() const;
		uint32_t getPoolIndex() const;
	private:
		const std::vector<UniformBufferObjectElement*>& mElements;
		IUniformBuffer* mBuffer;
		uint32_t mOffset;
		uint32_t mSize;
		uint32_t mPoolIndex;
};

class UniformBufferPool
{
	public:
		explicit UniformBufferPool(uint32_t aChunkSize, uint32_t aStride, UniformBufferCreateInfo aUboCreateInfo,
		                           std::vector<UniformBufferObjectElement*> aBufferElements);
		UniformBufferPool(const UniformBufferPool&) = delete;
		UniformBufferPool(UniformBufferPool&&) noexcept = delete;
		~UniformBufferPool();

		UniformBufferPool& operator=(const UniformBufferPool&) = delete;
		UniformBufferPool& operator=(UniformBufferPool&&) noexcept = delete;

		UniformBufferObject* acquire();
		UniformBufferObject* acquire(uint32_t aIndex);
		void release(UniformBufferObject* aObject);

	private:
		std::vector<IUniformBuffer*> mBuffers;
		std::unordered_set<uint32_t> mFreeSlots;
		UniformBufferCreateInfo mCreateInfo;
		const std::vector<UniformBufferObjectElement*> mBufferElements;
		uint32_t mCursor;
		uint32_t mChunkSize;
		uint32_t mStride;
};

#endif // uniformbufferpool_h__
