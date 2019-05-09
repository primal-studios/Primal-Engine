#ifndef matrix2_h__
#define matrix2_h__

#include <glm/gtc/matrix_transform.hpp>

#include "math/MatrixType.h"
#include "math/VectorType.h"

#include "math/Vector2.h"

template<typename T>
class Matrix2
{
	public:
		Matrix2()
		{
			_internal_value = detail::MatrixType<T, 2, 2>(1.0f);
		}

		Matrix2(const Vector2<T>& aRow0, const Vector2<T>& aRow1)
		{
			rows[0] = aRow0;
			rows[1] = aRow1;
		}

		Matrix2(const T aM00, const T aM01,
			const T aM10, const T aM11)
		{
			m00 = aM00;
			m01 = aM01;
			m10 = aM10;
			m11 = aM11;
		}

		Matrix2(const Matrix2& aOther)
		{
			_internal_value = aOther._internal_value;
		}

		Matrix2(Matrix2&& aOther) noexcept
		{
			_internal_value = std::move(aOther._internal_value);
		}

		bool operator == (const Matrix2& aOther)
		{
			return _internal_value == aOther._internal_value;
		}

		bool operator != (const Matrix2& aOther)
		{
			return _internal_value != aOther._internal_value;
		}

		Matrix2& operator = (const Matrix2& aOther)
		{
			_internal_value = aOther._internal_value;
			return *this;
		}

		Matrix2& operator = (Matrix2&& aOther) noexcept
		{
			_internal_value = std::move(aOther._internal_value);
			return *this;
		}

		Matrix2& operator *= (const Matrix2& aOther)
		{
			_internal_value *= aOther._internal_value;
			return *this;
		}

		T& operator [] (size_t aIndex)
		{
			if (aIndex > 8)
			{
				return v[0];
			}

			return v[aIndex];
		}

		static Matrix2 identity()
		{
			return detail::MatrixType<T, 2, 2>(1.0f);
		}

		Matrix2& determinant() const
		{
			return glm::determinant(_internal_value);
		}

		Matrix2& inversed() const
		{
			return glm::inverse(_internal_value);
		}

		Matrix2& inverse()
		{
			_internal_value = glm::inverse(_internal_value);
			return *this;
		}

		Matrix2& transposed() const
		{
			return glm::transpose(_internal_value);
		}

		Matrix2& transpose()
		{
			_internal_value = glm::transpose(_internal_value);
			return *this;
		}

		union
		{
			struct
			{
				T m00, m01;
				T m10, m11;
			};


			T v[4];

			detail::VectorType<T, 2> rows[2];
			detail::MatrixType<T, 2, 2> _internal_value;
		};
	private:
		Matrix2(const detail::MatrixType<T, 2, 2>& aValue)
		{
			_internal_value = aValue;
		}
};

using Matrix2f = Matrix2<float>;
using Matrix2d = Matrix2<bool>;

#endif // Matrix2_h__