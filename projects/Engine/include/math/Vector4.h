#ifndef vector4_h__
#define vector4_h__

#include "math/VectorType.h"

#include "math/Vector3.h"

template<typename T>
class Vector4
{
	public:
		Vector4()
		{
			_internal_value = detail::VectorType<T, 4>(0, 0, 0, 0);
		}

		explicit Vector4(const T aValue)
		{
			_internal_value = detail::VectorType<T, 4>(aValue, aValue, aValue, aValue);
		}

		Vector4(const T aX, const T aY, const T aZ, const T aW)
		{
			_internal_value = detail::VectorType<T, 4>(aX, aY, aZ, aW);
		}

		explicit Vector4(const Vector3<T>& aOther)
		{
			_internal_value = detail::VectorType<T, 4>(aOther.x, aOther.y, aOther.z, T(0));
		}

		Vector4(const Vector3<T>& aOther, T aW)
		{
			_internal_value = detail::VectorType<T, 4>(aOther.x, aOther.y, aOther.z, aW);
		}

		Vector4(const Vector4& aOther)
		{
			_internal_value = aOther._internal_value;
		}

		Vector4(Vector4&& aOther) noexcept
		{
			_internal_value = std::move(aOther._internal_value);
		}

		bool operator == (const Vector4& aOther)
		{
			return _internal_value == aOther._internal_value;
		}

		bool operator != (const Vector4& aOther)
		{
			return _internal_value != aOther._internal_value;
		}

		Vector4& operator = (const Vector4& aOther)
		{
			_internal_value = aOther._internal_value;
			return *this;
		}

		Vector4& operator = (Vector4&& aOther) noexcept
		{
			_internal_value = std::move(aOther._internal_value);
			return *this;
		}

		Vector4& operator += (const Vector4& aOther)
		{
			_internal_value += aOther._internal_value;
			return *this;
		}

		Vector4& operator -= (const Vector4& aOther)
		{
			_internal_value -= aOther._internal_value;
			return *this;
		}

		Vector4& operator *= (const Vector4& aOther)
		{
			_internal_value = detail::VectorType<T, 4>(x * aOther.x, y * aOther.y, z * aOther.z, w * aOther.w);
			return *this;
		}

		Vector4& operator *= (const T aOther)
		{
			_internal_value *= aOther;
			return *this;
		}

		Vector4& operator /= (const Vector4& aOther)
		{
			_internal_value = detail::VectorType<T, 4>(x / aOther.x, y / aOther.y, z / aOther.z, w / aOther.w);
			return *this;
		}

		Vector4& operator /= (const T aOther)
		{
			_internal_value /= aOther;
			return *this;
		}

		Vector4 cross(const Vector4& aOther) const
		{
			return glm::cross(_internal_value, aOther._internal_value);
		}

		T distance(const Vector4& aOther) const
		{
			return glm::distance(_internal_value, aOther._internal_value);
		}

		T dot(const Vector4& aOther) const
		{
			return glm::dot(_internal_value, aOther._internal_value);
		}

		T length() const
		{
			return glm::length(_internal_value);
		}

		Vector4& normalize()
		{
			_internal_value = glm::normalize(_internal_value);
			return *this;
		}

		Vector4 normalized()
		{
			return glm::normalize(_internal_value);
		}

		Vector4 reflect(const Vector4& aN) const
		{
			return glm::reflect(_internal_value, aN._internal_value);
		}

		Vector4 refract(const Vector4& aN, T aEta) const
		{
			return glm::refract(_internal_value, aN._internal_value, aEta);
		}

		union
		{
			struct
			{
				T x, y, z, w;
			};

			T v[4];

			detail::VectorType<T, 4> _internal_value;
		};

	private:
		Vector4(const detail::VectorType<T, 4>& aValue)
		{
			_internal_value = aValue;
		}
};

template<typename T>
Vector4<T> operator + (Vector4<T> aLeft, const Vector4<T>& aRight)
{
	aLeft += aRight;
	return aLeft;
}

template<typename T>
Vector4<T> operator - (Vector4<T> aLeft, const Vector4<T>& aRight)
{
	aLeft -= aRight;
	return aLeft;
}

template<typename T>
Vector4<T> operator * (Vector4<T> aLeft, const Vector4<T>& aRight)
{
	aLeft *= aRight;
	return aLeft;
}

template<typename T>
Vector4<T> operator * (Vector4<T> aLeft, const T aRight)
{
	aLeft *= aRight;
	return aLeft;
}

template<typename T>
Vector4<T> operator / (Vector4<T> aLeft, const Vector4<T>& aRight)
{
	aLeft /= aRight;
	return aLeft;
}

template<typename T>
Vector4<T> operator / (Vector4<T> aLeft, const T aRight)
{
	aLeft /= aRight;
	return aLeft;
}

using Vector4b = Vector4<bool>;
using Vector4u = Vector4<uint32_t>;
using Vector4i = Vector4<int32_t>;
using Vector4f = Vector4<float>;
using Vector4d = Vector4<double>;

#endif // vector4_h__