#ifndef matrix3_h__
#define matrix3_h__

#include <glm/gtc/matrix_transform.hpp>

#include "math/MatrixType.h"
#include "math/VectorType.h"

#include "math/Vector3.h"

#include "math/Matrix2.h"

template<typename T>
class Matrix3
{
	public:
		Matrix3()
		{
			_internal_value = detail::MatrixType<T, 3, 3>(1.0f);
		}

		Matrix3(const Vector3<T>& aRow0, const Vector3<T>& aRow1, const Vector3<T>& aRow2)
		{
			rows[0] = aRow0;
			rows[1] = aRow1;
			rows[2] = aRow2;
		}

		Matrix3(const T aM00, const T aM01, const T aM02,
			const T aM10, const T aM11, const T aM12,
			const T aM20, const T aM21, const T aM22)
		{
			m00 = aM00;
			m01 = aM01;
			m02 = aM02;
			m10 = aM10;
			m11 = aM11;
			m12 = aM12;
			m20 = aM20;
			m21 = aM21;
			m22 = aM22;
		}

		explicit Matrix3(const Matrix2<T>& aOther)
		{
			m00 = aOther.m00;
			m01 = aOther.m01;
			m02 = T(0);
			m10 = aOther.m10;
			m11 = aOther.m11;
			m12 = T(0);
			m20 = T(0);
			m21 = T(0);
			m22 = T(1);
		}

		Matrix3(const Matrix3& aOther)
		{
			_internal_value = aOther._internal_value;
		}

		Matrix3(Matrix3&& aOther) noexcept
		{
			_internal_value = std::move(aOther._internal_value);
		}

		bool operator == (const Matrix3& aOther)
		{
			return _internal_value == aOther._internal_value;
		}

		bool operator != (const Matrix3& aOther)
		{
			return _internal_value != aOther._internal_value;
		}

		Matrix3& operator = (const Matrix3& aOther)
		{
			_internal_value = aOther._internal_value;
			return *this;
		}

		Matrix3& operator = (Matrix3&& aOther) noexcept
		{
			_internal_value = std::move(aOther._internal_value);
			return *this;
		}

		Matrix3& operator *= (const Matrix3& aOther)
		{
			_internal_value *= aOther._internal_value;
			return *this;
		}
		
		template<typename T2>
		friend Vector3<T2> operator * (const Matrix3<T2>& aLeft, const Vector3<T2>& aRight);

		T& operator [] (size_t aIndex)
		{
			if (aIndex > 8)
			{
				return v[0];
			}

			return v[aIndex];
		}

		static Matrix3 identity()
		{
			return detail::MatrixType<T, 3, 3>(1.0f);
		}

		T determinant() const
		{
			return glm::determinant(_internal_value);
		}

		Matrix3& inversed() const
		{
			return glm::inverse(_internal_value);
		}

		Matrix3& inverse()
		{
			_internal_value = glm::inverse(_internal_value);
			return *this;
		}

		Matrix3& transposed() const
		{
			return glm::transpose(_internal_value);
		}

		Matrix3& transpose()
		{
			_internal_value = glm::transpose(_internal_value);
			return *this;
		}

		union
		{
			struct
			{
				T m00, m01, m02;
				T m10, m11, m12;
				T m20, m21, m22;
			};


			T v[9];

			detail::VectorType<T, 3> rows[3];
			detail::MatrixType<T, 3, 3> _internal_value;
		};
	private:
		Matrix3(const detail::MatrixType<T, 3, 3>& aValue)
		{
			_internal_value = aValue;
		}
};

template<typename T>
Vector3<T> operator * (const Matrix3<T>& aLeft, const Vector3<T>& aRight)
{
	auto value = aLeft._internal_value * aRight._internal_value;
	return Vector3<T>(value.x, value.y, value.z);
}

using Matrix3f = Matrix3<float>;
using Matrix3d = Matrix3<bool>;

#endif // matrix3_h__