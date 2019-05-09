#ifndef vector3_h__
#define vector3_h__

#include <glm/glm.hpp>

template<typename T>
class Vector3
{
	public:
		Vector3();

	private:
		union
		{
			struct
			{
				T x, y, z;
			};

			glm::vec<3, T, glm::defaultp> v;
		};
};

#endif // vector3_h__