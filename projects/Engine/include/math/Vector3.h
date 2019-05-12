#ifndef vector3_h__
#define vector3_h__

#include <glm/gtx/compatibility.hpp>

#include "math/VectorType.h"

#include "math/Vector2.h"

template<typename T>
class Vector3
{
	public:
		Vector3()
		{
			_internal_value = detail::VectorType<T, 3>(T(0), T(0), T(0));
		}

		explicit Vector3(const T& aValue)
		{
			_internal_value = detail::VectorType<T, 3>(aValue, aValue, aValue);
		}

		Vector3(const T aX, const T aY, const T aZ)
		{
			_internal_value = detail::VectorType<T, 3>(aX, aY, aZ);
		}

		explicit Vector3(const Vector2<T>& aOther)
		{
			_internal_value = detail::VectorType<T, 3>(aOther.x, aOther.y, T(0));
		}

		Vector3(const Vector2<T>& aOther, const T aZ)
		{
			_internal_value = detail::VectorType<T, 3>(aOther.x, aOther.y, aZ);
		}

		Vector3(const Vector3& aOther)
		{
			_internal_value = aOther._internal_value;
		}

		Vector3(Vector3&& aOther) noexcept
		{
			_internal_value = std::move(aOther._internal_value);
		}

		bool operator == (const Vector3& aOther)
		{
			return _internal_value == aOther._internal_value;
		}

		bool operator != (const Vector3& aOther)
		{
			return _internal_value != aOther._internal_value;
		}

		Vector3& operator = (const Vector3& aOther)
		{
			_internal_value = aOther._internal_value;
			return *this;
		}

		Vector3& operator = (Vector3&& aOther) noexcept
		{
			_internal_value = std::move(aOther._internal_value);
			return *this;
		}

		Vector3& operator += (const Vector3& aOther)
		{
			_internal_value += aOther._internal_value;
			return *this;
		}

		Vector3& operator -= (const Vector3& aOther)
		{
			_internal_value -= aOther._internal_value;
			return *this;
		}

		Vector3& operator *= (const Vector3& aOther)
		{
			_internal_value = detail::VectorType<T, 3>(x * aOther.x, y * aOther.y, z * aOther.z);
			return *this;
		}

		Vector3& operator *= (const T aOther)
		{
			_internal_value *= aOther;
			return *this;
		}

		Vector3& operator /= (const Vector3& aOther)
		{
			_internal_value = detail::VectorType<T, 3>(x / aOther.x, y / aOther.y, z / aOther.z);
			return *this;
		}

		Vector3& operator /= (const T aOther)
		{
			_internal_value /= aOther;
			return *this;
		}

		Vector3 cross(const Vector3& aOther) const
		{
			return glm::cross(_internal_value, aOther._internal_value);
		}

		T distance(const Vector3& aOther) const
		{
			return glm::distance(_internal_value, aOther._internal_value);
		}

		T dot(const Vector3& aOther) const
		{
			return glm::dot(_internal_value, aOther._internal_value);
		}

		T length() const
		{
			return glm::length(_internal_value);
		}

		Vector3& normalize()
		{
			_internal_value = glm::normalize(_internal_value);
			return *this;
		}

		Vector3 normalized()
		{
			return glm::normalize(_internal_value);
		}

		Vector3 reflect(const Vector3& aN) const
		{
			return glm::reflect(_internal_value, aN._internal_value);
		}

		Vector3 refract(const Vector3& aN, T aEta) const
		{
			return glm::refract(_internal_value, aN._internal_value, aEta);
		}

		Vector3& lerp(const Vector3& aOther, T aBlend)
		{
			_internal_value = glm::lerp(_internal_value, aOther._internal_value, aBlend);
			return *this;
		}

		union
		{
			struct
			{
				T x, y, z;
			};

			T v[3];
			
			detail::VectorType<T, 3> _internal_value;
		};

	private:
		Vector3(const detail::VectorType<T, 3>& aValue)
		{
			_internal_value = aValue;
		}
};

template<typename T>
Vector3<T> operator + (Vector3<T> aLeft, const Vector3<T>& aRight)
{
	aLeft += aRight;
	return aLeft;
}

template<typename T>
Vector3<T> operator - (Vector3<T> aLeft, const Vector3<T>& aRight)
{
	aLeft -= aRight;
	return aLeft;
}

template<typename T>
Vector3<T> operator * (Vector3<T> aLeft, const Vector3<T>& aRight)
{
	aLeft *= aRight;
	return aLeft;
}

template<typename T>
Vector3<T> operator * (Vector3<T> aLeft, const T aRight)
{
	aLeft *= aRight;
	return aLeft;
}

template<typename T>
Vector3<T> operator / (Vector3<T> aLeft, const Vector3<T>& aRight)
{
	aLeft /= aRight;
	return aLeft;
}

template<typename T>
Vector3<T> operator / (Vector3<T> aLeft, const T aRight)
{
	aLeft /= aRight;
	return aLeft;
}

using Vector3b = Vector3<bool>;
using Vector3u = Vector3<uint32_t>;
using Vector3i = Vector3<int32_t>;
using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;

#endif // vector3_h__