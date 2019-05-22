#ifndef vertexbufferlayout_h__
#define vertexbufferlayout_h__

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

enum class EVertexBufferLayoutElementTypes : uint32_t
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

struct VertexBufferLayoutElement
{
	std::string name;
	EVertexBufferLayoutElementTypes type;
	size_t size;
	uint32_t count;
	size_t offset;
	bool normalized;
};

class VertexBufferLayout
{
public:
	VertexBufferLayout() = default;
	~VertexBufferLayout() = default;

	const std::vector<VertexBufferLayoutElement>& getLayout() const { return mLayout; }
	size_t getStride() const { return mSize; }

	template<typename T>
	void push(const std::string& aName, const uint32_t aCount = 1, const bool aNormalize = false)
	{
		PRIMAL_ASSERT(false);
	}
private:
	size_t mSize = 0;
	std::vector<VertexBufferLayoutElement> mLayout;

	void _push(const std::string& aName, const EVertexBufferLayoutElementTypes& aType, const size_t aSize, const uint32_t aCount, const bool aNormalized)
	{
		mLayout.push_back({ aName, aType, aSize, aCount, mSize, aNormalized });
		mSize += aSize * aCount;
	}
};

template<>
inline void VertexBufferLayout::push<uint8_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EVertexBufferLayoutElementTypes::UBYTE, sizeof(uint8_t), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<int8_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EVertexBufferLayoutElementTypes::BYTE, sizeof(int8_t), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<uint16_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EVertexBufferLayoutElementTypes::USHORT, sizeof(uint16_t), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<int16_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EVertexBufferLayoutElementTypes::SHORT, sizeof(int16_t), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<uint32_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EVertexBufferLayoutElementTypes::UINT, sizeof(uint32_t), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<int32_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EVertexBufferLayoutElementTypes::INT, sizeof(int32_t), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<uint64_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EVertexBufferLayoutElementTypes::ULONG, sizeof(uint64_t), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<int64_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EVertexBufferLayoutElementTypes::LONG, sizeof(int64_t), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<float>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EVertexBufferLayoutElementTypes::FLOAT, sizeof(float), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<double>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EVertexBufferLayoutElementTypes::DOUBLE, sizeof(double), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<Vector2f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EVertexBufferLayoutElementTypes::FLOAT, sizeof(float) * 2, aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<Vector3f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EVertexBufferLayoutElementTypes::FLOAT, sizeof(float) * 3, aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<Vector4f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EVertexBufferLayoutElementTypes::FLOAT, sizeof(float) * 4, aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<Matrix2f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EVertexBufferLayoutElementTypes::FLOAT, sizeof(float) * 4, aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<Matrix3f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EVertexBufferLayoutElementTypes::FLOAT, sizeof(float) * 9, aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<Matrix4f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, EVertexBufferLayoutElementTypes::FLOAT, sizeof(float) * 16, aCount, aNormalized);
}

#endif // vertexbufferlayout_h__