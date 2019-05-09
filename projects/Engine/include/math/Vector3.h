#ifndef vector3_h__
#define vector3_h__

#include "math/VectorType.h"

template<typename T>
class Vector3
{
	public:
		Vector3()
		{
			_internal_value = detail::VectorType<T, 3>(0, 0, 0);
		}

		explicit Vector3(const T& aValue)
		{
			_internal_value = detail::VectorType<T, 3>(aValue, aValue, aValue);
		}

		Vector3(const T& aX, const T& aY, const T& aZ)
		{
			_internal_value = detail::VectorType<T, 3>(aX, aY, aZ);
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

		Vector3& operator *= (const T& aOther)
		{
			_internal_value *= aOther;
			return *this;
		}

		Vector3& operator /= (const Vector3& aOther)
		{
			_internal_value = detail::VectorType<T, 3>(x / aOther.x, y / aOther.y, z / aOther.z);
			return *this;
		}

		Vector3& operator /= (const T& aOther)
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

		union
		{
			struct
			{
				T x, y, z;
			};
			
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
Vector3<T> operator * (Vector3<T> aLeft, const T& aRight)
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
Vector3<T> operator / (Vector3<T> aLeft, const T& aRight)
{
	aLeft /= aRight;
	return aLeft;
}

#endif // vector3_h__