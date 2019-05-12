#ifndef vector2_h__
#define vector2_h__

#include <glm/gtx/compatibility.hpp>
#include "math/VectorType.h"

template<typename T>
class Vector2
{
	public:
		Vector2()
		{
			_internal_value = detail::VectorType<T, 2>(0, 0);
		}

		explicit Vector2(const T aValue)
		{
			_internal_value = detail::VectorType<T, 2>(aValue, aValue);
		}

		Vector2(const T aX, const T aY)
		{
			_internal_value = detail::VectorType<T, 2>(aX, aY);
		}

		Vector2(const Vector2& aOther)
		{
			_internal_value = aOther._internal_value;
		}

		Vector2(Vector2&& aOther) noexcept
		{
			_internal_value = std::move(aOther._internal_value);
		}

		bool operator == (const Vector2& aOther)
		{
			return _internal_value == aOther._internal_value;
		}

		bool operator != (const Vector2& aOther)
		{
			return _internal_value != aOther._internal_value;
		}

		Vector2& operator = (const Vector2& aOther)
		{
			_internal_value = aOther._internal_value;
			return *this;
		}

		Vector2& operator = (Vector2&& aOther) noexcept
		{
			_internal_value = std::move(aOther._internal_value);
			return *this;
		}

		Vector2& operator += (const Vector2& aOther)
		{
			_internal_value += aOther._internal_value;
			return *this;
		}

		Vector2& operator -= (const Vector2& aOther)
		{
			_internal_value -= aOther._internal_value;
			return *this;
		}

		Vector2& operator *= (const Vector2& aOther)
		{
			_internal_value = detail::VectorType<T, 2>(x * aOther.x, y * aOther.y);
			return *this;
		}

		Vector2& operator *= (const T aOther)
		{
			_internal_value *= aOther;
			return *this;
		}

		Vector2& operator /= (const Vector2& aOther)
		{
			_internal_value = detail::VectorType<T, 2>(x / aOther.x, y / aOther.y);
			return *this;
		}

		Vector2& operator /= (const T aOther)
		{
			_internal_value /= aOther;
			return *this;
		}

		Vector2 cross(const Vector2& aOther) const
		{
			return glm::cross(_internal_value, aOther._internal_value);
		}

		T distance(const Vector2& aOther) const
		{
			return glm::distance(_internal_value, aOther._internal_value);
		}

		T dot(const Vector2& aOther) const
		{
			return glm::dot(_internal_value, aOther._internal_value);
		}

		T length() const
		{
			return glm::length(_internal_value);
		}

		Vector2& normalize()
		{
			_internal_value = glm::normalize(_internal_value);
			return *this;
		}

		Vector2 normalized()
		{
			return glm::normalize(_internal_value);
		}

		Vector2 reflect(const Vector2& aN) const
		{
			return glm::reflect(_internal_value, aN._internal_value);
		}

		Vector2 refract(const Vector2& aN, T aEta) const
		{
			return glm::refract(_internal_value, aN._internal_value, aEta);
		}

		Vector2& lerp(const Vector2& aOther, T aBlend)
		{
			_internal_value = glm::lerp(_internal_value, aOther._internal_value, aBlend);
			return *this;
		}

		union
		{
			struct
			{
				T x, y;
			};

			T v[2];

			detail::VectorType<T, 2> _internal_value;
		};

	private:
		Vector2(const detail::VectorType<T, 2> & aValue)
		{
			_internal_value = aValue;
		}
};

template<typename T>
Vector2<T> operator + (Vector2<T> aLeft, const Vector2<T>& aRight)
{
	aLeft += aRight;
	return aLeft;
}

template<typename T>
Vector2<T> operator - (Vector2<T> aLeft, const Vector2<T>& aRight)
{
	aLeft -= aRight;
	return aLeft;
}

template<typename T>
Vector2<T> operator * (Vector2<T> aLeft, const Vector2<T>& aRight)
{
	aLeft *= aRight;
	return aLeft;
}

template<typename T>
Vector2<T> operator * (Vector2<T> aLeft, const T aRight)
{
	aLeft *= aRight;
	return aLeft;
}

template<typename T>
Vector2<T> operator / (Vector2<T> aLeft, const Vector2<T>& aRight)
{
	aLeft /= aRight;
	return aLeft;
}

template<typename T>
Vector2<T> operator / (Vector2<T> aLeft, const T aRight)
{
	aLeft /= aRight;
	return aLeft;
}

using Vector2b = Vector2<bool>;
using Vector2u = Vector2<uint32_t>;
using Vector2i = Vector2<int32_t>;
using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;

#endif // vector2_h__