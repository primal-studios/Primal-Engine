#ifndef vectortype_h__
#define vectortype_h__

#include <glm/glm.hpp>

namespace detail
{
	template<typename T, size_t Sz>
	struct getGLMVectorType
	{
		typedef void Type;
	};

	template<>
	struct getGLMVectorType<bool, 2>
	{
		typedef glm::bvec2 Type;
	};

	template<>
	struct getGLMVectorType<int32_t, 2>
	{
		typedef glm::ivec2 Type;
	};

	template<>
	struct getGLMVectorType<uint32_t, 2>
	{
		typedef glm::uvec2 Type;
	};

	template<>
	struct getGLMVectorType<float, 2>
	{
		typedef glm::vec2 Type;
	};

	template<>
	struct getGLMVectorType<double, 2>
	{
		typedef glm::dvec2 Type;
	};

	template<>
	struct getGLMVectorType<bool, 3>
	{
		typedef glm::bvec3 Type;
	};

	template<>
	struct getGLMVectorType<int32_t, 3>
	{
		typedef glm::ivec3 Type;
	};

	template<>
	struct getGLMVectorType<uint32_t, 3>
	{
		typedef glm::uvec3 Type;
	};

	template<>
	struct getGLMVectorType<float, 3>
	{
		typedef glm::vec3 Type;
	};

	template<>
	struct getGLMVectorType<double, 3>
	{
		typedef glm::dvec3 Type;
	};

	template<>
	struct getGLMVectorType<bool, 4>
	{
		typedef glm::bvec4 Type;
	};

	template<>
	struct getGLMVectorType<int32_t, 4>
	{
		typedef glm::ivec4 Type;
	};

	template<>
	struct getGLMVectorType<uint32_t, 4>
	{
		typedef glm::uvec4 Type;
	};

	template<>
	struct getGLMVectorType<float, 4>
	{
		typedef glm::vec4 Type;
	};

	template<>
	struct getGLMVectorType<double, 4>
	{
		typedef glm::dvec4 Type;
	};

	template<typename T, size_t Sz>
	using VectorType = typename getGLMVectorType<T, Sz>::Type;
}

#endif // vectortype_h__