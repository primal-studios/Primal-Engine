#ifndef matrix4_h__
#define matrix4_h__

#include <glm/gtc/matrix_transform.hpp>

#include "math/MatrixType.h"
#include "math/VectorType.h"

#include "math/Vector3.h"
#include "math/Vector4.h"

#include "math/Matrix3.h"

template<typename T>
class Matrix4
{
	public:
		Matrix4()
		{
			_internal_value = detail::MatrixType<T, 4, 4>(1.0f);
		}

		Matrix4(const Vector4<T>& aRow0, const Vector4<T>& aRow1, const Vector4<T>& aRow2, const Vector4<T>& aRow3)
		{
			rows[0] = aRow0;
			rows[1] = aRow1;
			rows[2] = aRow2;
			rows[3] = aRow3;
		}

		Matrix4(const T aM00, const T aM01, const T aM02, const T aM03,
				const T aM10, const T aM11, const T aM12, const T aM13,
				const T aM20, const T aM21, const T aM22, const T aM23,
				const T aM30, const T aM31, const T aM32, const T aM33)
		{
			m00 = aM00;
			m01 = aM01;
			m02 = aM02;
			m03 = aM03;
			m10 = aM10;
			m11 = aM11;
			m12 = aM12;
			m13 = aM13;
			m20 = aM20;
			m21 = aM21;
			m22 = aM22;
			m23 = aM23;
			m30 = aM30;
			m31 = aM31;
			m32 = aM32;
			m33 = aM33;
		}

		explicit Matrix4(const Matrix3<T>& aOther)
		{
			m00 = aOther.m00;
			m01 = aOther.m01;
			m02 = aOther.m02;
			m03 = T(0);
			m10 = aOther.m10;
			m11 = aOther.m11;
			m12 = aOther.m12;
			m13 = T(0);
			m20 = aOther.m20;
			m21 = aOther.m21;
			m22 = aOther.m22;
			m23 = T(0);
			m30 = T(0);
			m31 = T(0);
			m32 = T(0);
			m33 = T(1);
		}
	
		Matrix4(const Matrix4& aOther)
		{
			_internal_value = aOther._internal_value;
		}

		Matrix4(Matrix4&& aOther) noexcept
		{
			_internal_value = std::move(aOther._internal_value);
		}

		bool operator == (const Matrix4& aOther)
		{
			return _internal_value == aOther._internal_value;
		}

		bool operator != (const Matrix4& aOther)
		{
			return _internal_value != aOther._internal_value;
		}

		Matrix4& operator = (const Matrix4& aOther)
		{
			_internal_value = aOther._internal_value;
			return *this;
		}

		Matrix4& operator = (Matrix4&& aOther) noexcept
		{
			_internal_value = std::move(aOther._internal_value);
			return *this;
		}

		Matrix4& operator *= (const Matrix4& aOther)
		{
			_internal_value *= aOther._internal_value;
			return *this;
		}

		template<typename T2>
		friend Vector3<T2> operator * (const Matrix4<T2>& aLeft, const Vector3<T2>& aRight);

		template<typename T2>
		friend Vector4<T2> operator * (const Matrix4<T2>& aLeft, const Vector4<T2>& aRight);

		T& operator [] (size_t aIndex)
		{
			if( aIndex > 15)
			{
				return v[0];
			}

			return v[aIndex];
		}

		static Matrix4 identity()
		{
			return detail::MatrixType<T, 4, 4>(1.0f);
		}

		static Matrix4 ortho(const T aLeft, const T aRight, const T aBottom, const T aTop, const T aNear, const T aFar)
		{
			return glm::ortho(aLeft, aRight, aBottom, aTop, aNear, aFar);
		}

		static Matrix4 perspective(const T aFov, const T aAspect, const T aNear, const T aFar)
		{
			return glm::perspective(aFov, aAspect, aNear, aFar);
		}

		static Matrix4 lookAt(const Vector3<T>& aEye, const Vector3<T>& aTarget, const Vector3<T>& aUp)
		{
			return glm::lookAt(aEye._internal_value, aTarget._internal_value, aUp._internal_value);
		}

		static Matrix4 rotate(const Matrix4& aMatrix, const Vector3f& aAxis, const T aAngle)
		{
			return glm::rotate(aMatrix._internal_value, aAngle, aAxis._internal_value);
		}

		static Matrix4 translate(const Matrix4& aMatrix, const Vector3f& aTranslation)
		{
			return glm::translate(aMatrix._internal_value, aTranslation._internal_value);
		}

		T determinant() const
		{
			return glm::determinant(_internal_value);
		}

		Matrix4& inversed() const
		{
			return glm::inverse(_internal_value);
		}

		Matrix4& inverse()
		{
			_internal_value = glm::inverse(_internal_value);
			return *this;
		}

		Matrix4& transposed() const
		{
			return glm::transpose(_internal_value);
		}

		Matrix4& transpose()
		{
			_internal_value = glm::transpose(_internal_value);
			return *this;
		}

		union
		{
			struct
			{
				T m00, m01, m02, m03;
				T m10, m11, m12, m13;
				T m20, m21, m22, m23;
				T m30, m31, m32, m33;
			};


			T v[16];

			detail::VectorType<T, 4> rows[4];
			detail::MatrixType<T, 4, 4> _internal_value;
		};

		Matrix4(const detail::MatrixType<T, 4, 4> & aValue)
		{
			_internal_value = aValue;
		}
	private:
		
};

template<typename T>
Vector3<T> operator * (const Matrix4<T>& aLeft, const Vector3<T>& aRight)
{
	Vector4<T> right = Vector4<T>(aRight.x, aRight.y, aRight.z, T(1));
	auto value = aLeft._internal_value * right._internal_value;
	return Vector3<T>(value.x, value.y, value.z);
}

template<typename T>
Vector4<T> operator * (const Matrix4<T>& aLeft, const Vector4<T>& aRight)
{
	auto value = aLeft._internal_value * aRight._internal_value;
	return Vector4<T>(value.x, value.y, value.z, value.w);
}

using Matrix4f = Matrix4<float>;
using Matrix4d = Matrix4<bool>;

#endif // matrix4_h__