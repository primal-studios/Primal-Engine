#ifndef bufferlayout_h__
#define bufferlayout_h__

#include <cstdint>
#include <string>
#include <vector>

#include "core/PrimalAssert.h"

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include "math/Matrix2.h"
#include "math/Matrix3.h"
#include "math/Matrix4.h"

enum class EBufferLayoutElementTypes : uint32_t
{
	UBYTE,
	BYTE,
	USHORT,
	SHORT,
	UINT,
	INT,
	ULONG,
	LONG,
	FLOAT,
	DOUBLE,
	VEC2,
	VEC3,
	VEC4,
	MAT2,
	MAT3,
	MAT4
};

struct BufferLayoutElement
{
	std::string name;
	EBufferLayoutElementTypes type;
	size_t size;
	uint32_t count;
	size_t offset;
	bool normalized;
};

class BufferLayout
{
	public:
		BufferLayout() = default;
		~BufferLayout() = default;

		const std::vector<BufferLayoutElement>& getLayout() const { return mLayout; }
		size_t getStride() const { return mSize; }

		template<typename T>
		void push(const std::string& aName, const uint32_t aCount = 1, const bool aNormalize = false)
		{
			PRIMAL_ASSERT(false);
		}
	private:
		size_t mSize = 0;
		std::vector<BufferLayoutElement> mLayout;

		void _push(const std::string& aName, const EBufferLayoutElementTypes& aType, const size_t aSize, const uint32_t aCount, const bool aNormalized)
		{
			mLayout.push_back({ aName, aType, aSize, aCount, mSize, aNormalized });
			mSize += aSize * aCount;
		}
};

template<>
inline void BufferLayout::push<uint8_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EBufferLayoutElementTypes::UBYTE, sizeof(uint8_t), aCount, aNormalized);
}

template<>
inline void BufferLayout::push<int8_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EBufferLayoutElementTypes::BYTE, sizeof(int8_t), aCount, aNormalized);
}

template<>
inline void BufferLayout::push<uint16_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EBufferLayoutElementTypes::USHORT, sizeof(uint16_t), aCount, aNormalized);
}

template<>
inline void BufferLayout::push<int16_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EBufferLayoutElementTypes::SHORT, sizeof(int16_t), aCount, aNormalized);
}

template<>
inline void BufferLayout::push<uint32_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EBufferLayoutElementTypes::UINT, sizeof(uint32_t), aCount, aNormalized);
}

template<>
inline void BufferLayout::push<int32_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EBufferLayoutElementTypes::INT, sizeof(int32_t), aCount, aNormalized);
}

template<>
inline void BufferLayout::push<uint64_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EBufferLayoutElementTypes::ULONG, sizeof(uint64_t), aCount, aNormalized);
}

template<>
inline void BufferLayout::push<int64_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EBufferLayoutElementTypes::LONG, sizeof(int64_t), aCount, aNormalized);
}

template<>
inline void BufferLayout::push<float>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EBufferLayoutElementTypes::FLOAT, sizeof(float), aCount, aNormalized);
}

template<>
inline void BufferLayout::push<double>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EBufferLayoutElementTypes::DOUBLE, sizeof(double), aCount, aNormalized);
}

template<>
inline void BufferLayout::push<Vector2f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EBufferLayoutElementTypes::VEC2, sizeof(float) * 2, aCount, aNormalized);
}

template<>
inline void BufferLayout::push<Vector3f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EBufferLayoutElementTypes::VEC3, sizeof(float) * 3, aCount, aNormalized);
}

template<>
inline void BufferLayout::push<Vector4f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EBufferLayoutElementTypes::VEC4, sizeof(float) * 4, aCount, aNormalized);
}

template<>
inline void BufferLayout::push<Matrix2f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EBufferLayoutElementTypes::FLOAT, sizeof(float) * 4, aCount, aNormalized);
}

template<>
inline void BufferLayout::push<Matrix3f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EBufferLayoutElementTypes::FLOAT, sizeof(float) * 9, aCount, aNormalized);
}

template<>
inline void BufferLayout::push<Matrix4f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EBufferLayoutElementTypes::FLOAT, sizeof(float) * 16, aCount, aNormalized);
}

#endif // bufferlayout_h__