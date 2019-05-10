#ifndef quaterniontype_h__
#define quaterniontype_h__

#include <glm/glm.hpp>

namespace detail
{
	template<typename T>
	struct getGLMQuaternionType
	{
		typedef void Type;
	};

	template<>
	struct getGLMQuaternionType<float>
	{
		typedef glm::quat Type;
	};

	template<>
	struct getGLMQuaternionType<double>
	{
		typedef glm::dquat Type;
	};
	
	template<typename T>
	using QuaternionType = typename getGLMQuaternionType<T>::Type;
}

#endif // quaterniontype_h__