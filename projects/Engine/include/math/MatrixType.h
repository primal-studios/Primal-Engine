#ifndef matrixtype_h__
#define matrixtype_h__

#include <glm/glm.hpp>

namespace detail
{
	template<typename T, size_t Cols, size_t Rows>
	struct getGLMType
	{
		typedef void Type;
	};

	template<>
	struct getGLMType<float, 2, 2>
	{
		typedef glm::mat2 Type;
	};

	template<>
	struct getGLMType<double, 2, 2>
	{
		typedef glm::dmat2 Type;
	};

	template<>
	struct getGLMType<float, 3, 3>
	{
		typedef glm::mat3 Type;
	};

	template<>
	struct getGLMType<double, 3, 3>
	{
		typedef glm::dmat3 Type;
	};

	template<>
	struct getGLMType<float, 4, 4>
	{
		typedef glm::mat4 Type;
	};

	template<>
	struct getGLMType<double, 4, 4>
	{
		typedef glm::dmat4 Type;
	};

	template<typename T, size_t Cols, size_t Rows>
	using MatrixType = typename getGLMType<T, Cols, Rows>::Type;
}

#endif // matrixtype_h__