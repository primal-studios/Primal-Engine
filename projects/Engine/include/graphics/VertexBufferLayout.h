#ifndef vertexbufferlayout_h__
#define vertexbufferlayout_h__

#include <string>
#include <vector>

#include <Eigen/Eigen>
#include <glad/glad.h>

#include "core/Log.h"
#include "core/PrimalAssert.h"

struct VertexBufferLayoutElement
{
	std::string name;

	uint32_t type;
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
			PRIMAL_INTERNAL_ASSERT(false, "Unknown type!");
		}

	private:
		size_t mSize = 0;
		std::vector<VertexBufferLayoutElement> mLayout;

		void _push(const std::string& aName, const uint32_t aType, const size_t aSize, const uint32_t aCount, const bool aNormalized)
		{
			mLayout.push_back({ aName, aType, aSize, aCount, mSize, aNormalized });
			mSize += aSize * aCount;
		}
};

template<>
inline void VertexBufferLayout::push<int8_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, GL_BYTE, sizeof(int8_t), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<uint8_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, GL_UNSIGNED_BYTE, sizeof(uint8_t), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<int16_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, GL_SHORT, sizeof(int16_t), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<uint16_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, GL_UNSIGNED_SHORT, sizeof(uint16_t), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<int32_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, GL_INT, sizeof(int32_t), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<uint32_t>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, GL_UNSIGNED_INT, sizeof(uint32_t), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<float>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, GL_FLOAT, sizeof(float), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<double>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, GL_DOUBLE, sizeof(double), aCount, aNormalized);
}

template<>
inline void VertexBufferLayout::push<Eigen::Vector2f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, GL_FLOAT, sizeof(float), aCount * 2, aNormalized);
}

template<>
inline void VertexBufferLayout::push<Eigen::Vector3f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, GL_FLOAT, sizeof(float), aCount * 3, aNormalized);
}

template<>
inline void VertexBufferLayout::push<Eigen::Vector4f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, GL_FLOAT, sizeof(float), aCount * 4, aNormalized);
}

template<>
inline void VertexBufferLayout::push<Eigen::Matrix2f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, GL_FLOAT, sizeof(float), aCount * 4, aNormalized);
}

template<>
inline void VertexBufferLayout::push<Eigen::Matrix3f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, GL_FLOAT, sizeof(float), aCount * 9, aNormalized);
}

template<>
inline void VertexBufferLayout::push<Eigen::Matrix4f>(const std::string& aName, const uint32_t aCount, const bool aNormalized)
{
	_push(aName, GL_FLOAT, sizeof(float), aCount * 16, aNormalized);
}

#endif // vertexbufferlayout_h__