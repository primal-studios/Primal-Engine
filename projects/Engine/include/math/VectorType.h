#ifndef vectortype_h__
#define vectortype_h__

#include <glm/glm.hpp>

namespace detail
{
	template<typename T, size_t Sz>
	struct getGLMType
	{
		typedef void Type;
	};

	template<>
	struct getGLMType<bool, 2>
	{
		typedef glm::bvec2 Type;
	};

	template<>
	struct getGLMType<int32_t, 2>
	{
		typedef glm::ivec2 Type;
	};

	template<>
	struct getGLMType<uint32_t, 2>
	{
		typedef glm::uvec2 Type;
	};

	template<>
	struct getGLMType<float, 2>
	{
		typedef glm::vec2 Type;
	};

	template<>
	struct getGLMType<double, 2>
	{
		typedef glm::dvec2 Type;
	};

	template<>
	struct getGLMType<bool, 3>
	{
		typedef glm::bvec3 Type;
	};

	template<>
	struct getGLMType<int32_t, 3>
	{
		typedef glm::ivec3 Type;
	};

	template<>
	struct getGLMType<uint32_t, 3>
	{
		typedef glm::uvec3 Type;
	};

	template<>
	struct getGLMType<float, 3>
	{
		typedef glm::vec3 Type;
	};

	template<>
	struct getGLMType<double, 3>
	{
		typedef glm::dvec3 Type;
	};

	template<>
	struct getGLMType<bool, 4>
	{
		typedef glm::bvec4 Type;
	};

	template<>
	struct getGLMType<int32_t, 4>
	{
		typedef glm::ivec4 Type;
	};

	template<>
	struct getGLMType<uint32_t, 4>
	{
		typedef glm::uvec4 Type;
	};

	template<>
	struct getGLMType<float, 4>
	{
		typedef glm::vec4 Type;
	};

	template<>
	struct getGLMType<double, 4>
	{
		typedef glm::dvec4 Type;
	};

	template<typename T, size_t Sz>
	using VectorType = typename getGLMType<T, Sz>::Type;
}

#endif // vectortype_h__