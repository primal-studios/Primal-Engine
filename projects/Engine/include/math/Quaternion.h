#ifndef quaternion_h__
#define quaternion_h__

#include <glm/gtx/quaternion.hpp>
#include <algorithm>

#include "math/Vector3.h"
#include "math/Vector4.h"

#include "math/QuaternionType.h"

#include "math/Matrix3.h"

#include "core/Property.h"

template<typename T>
class Quaternion
{
	public:
		Quaternion()
		{
			_internal_value = detail::QuaternionType<T>(T(0), T(0), T(0), T(1));

			eulerAngles.getCallback([=](Vector3<T> & aValue)
			{
				auto e = glm::eulerAngles(_internal_value);
				return Vector3<T>(glm::degrees(e.x), glm::degrees(e.y), glm::degrees(e.z));
			});
			
			eulerAngles.setCallback([=](Vector3<T> & aValue)
			{
				T cy = cos(glm::radians(aValue.z) * T(0.5));
				T sy = sin(glm::radians(aValue.z) * T(0.5));
				T cp = cos(glm::radians(aValue.y) * T(0.5));
				T sp = sin(glm::radians(aValue.y) * T(0.5));
				T cr = cos(glm::radians(aValue.x) * T(0.5));
				T sr = sin(glm::radians(aValue.x) * T(0.5));

				w = cy * cp * cr + sy * sp * sr;
				x = cy * cp * sr - sy * sp * cr;
				y = sy * cp * sr + cy * sp * cr;
				z = sy * cp * cr - cy * sp * sr;

				return aValue;
			});
		}

		Quaternion(const T aX, const T aY, const T aZ, const T aW) : Quaternion()
		{
			_internal_value = detail::QuaternionType<T>(aX, aY, aZ, aW);
		}

		Quaternion(const Vector3<T>& aVector, const T aW) : Quaternion()
		{
			_internal_value = detail::QuaternionType<T>(aVector.x, aVector.y, aVector.z, aW);
		}

		explicit Quaternion(const Vector4<T>& aValue) : Quaternion()
		{
			_internal_value = detail::QuaternionType<T>(aValue.x, aValue.y, aValue.z, aValue.w);
		}

		explicit Quaternion(const Matrix3<T>& aMatrix) : Quaternion()
		{
			T scale = pow(aMatrix.determinant(), T(1) / T(3));

			w = static_cast<T>(sqrt(std::max(0, scale + aMatrix[0] + aMatrix[4] + aMatrix[9])) / T(2));
			x = static_cast<T>(sqrt(std::max(0, scale + aMatrix[0] - aMatrix[4] - aMatrix[9])) / T(2));
			y = static_cast<T>(sqrt(std::max(0, scale - aMatrix[0] + aMatrix[4] - aMatrix[9])) / T(2));
			z = static_cast<T>(sqrt(std::max(0, scale - aMatrix[0] - aMatrix[4] + aMatrix[9])) / T(2));

			if (aMatrix[5] - aMatrix[7] < T(0)) x = -x;
			if (aMatrix[6] - aMatrix[2] < T(0)) y = -y;
			if (aMatrix[1] - aMatrix[3] < T(0)) z = -z;
		}

		Quaternion(const Quaternion& aOther) : Quaternion()
		{
			_internal_value = aOther._internal_value;
		}

		Quaternion(Quaternion&& aOther) noexcept
		{
			_internal_value = std::move(aOther._internal_value);
		}

		bool operator == (const Quaternion& aOther)
		{
			return dot(aOther) > T(0.999999);
		}

		bool operator != (const Quaternion& aOther)
		{
			return dot(aOther) <= T(0.999999);
		}

		Quaternion& operator = (const Quaternion& aOther)
		{
			_internal_value = aOther._internal_value;
			return *this;
		}

		Quaternion& operator = (Quaternion&& aOther) noexcept
		{
			_internal_value = std::move(aOther._internal_value);
			return *this;
		}

		Quaternion& operator *= (const Quaternion& aOther)
		{
			_internal_value *= aOther._internal_value;
			return *this;
		}

		Property<Vector3<T>> eulerAngles = Vector3<T>(T(0), T(0), T(0));

		T length() const
		{
			return glm::length(_internal_value);
		}

		T lengthSquared() const
		{
			return glm::length2(_internal_value);
		}

		void normalize()
		{
			_internal_value = glm::normalize(_internal_value);
		}

		Quaternion normalized()
		{
			return glm::normalize(_internal_value);
		}

		T dot(const Quaternion& aOther)
		{
			return glm::dot(_internal_value, aOther._internal_value);
		}

		Quaternion axisAngle(const Vector3<T>& aAxis, T aAngle)
		{
			return glm::angleAxis(aAngle, aAxis._internal_value);
		}

		Vector4<T> toAxisAngle()
		{
			Quaternion q = this;
			if (abs(w) > 1.0f)
				q.normalize();

			Vector4<T> result;

			result.w = T(2) * static_cast<T>(acos(q.w));
			T den = static_cast<T>(sqrt(T(1) - q.w * q.w));
			if(den > T(0.0001))
			{
				result.x = q.x / den;
				result.y = q.y / den;
				result.z = q.z / den;
			}
			else
			{
				result.x = T(1);
				result.y = result.z = T(0);
			}

			return result;
		}

		static T angle(Quaternion& aFrom, Quaternion& aTo)
		{
			T f = aFrom.dot(aTo);
			return static_cast<T>(glm::degrees(acosf(std::min(abs(f), T(1)) * T(2))));
		}

		static Quaternion fromToRotation(Vector3<T>& aFromDirection, Vector3<T>& aToDirection)
		{
			return rotateTowards(lookRotation(aFromDirection), lookRotation(aToDirection), std::numeric_limits<T>::max());
		}

		static Quaternion lookRotation(Vector3<T>& aForward, Vector3<T>& aUp = Vector3<T>(0, 1, 0))
		{
			aForward.normalize();
			Vector3<T> right = aUp.cross(aForward).normalized();
			aUp = aForward.cross(right);
			T m00 = right.x;
			T m01 = right.y;
			T m02 = right.z;
			T m10 = aUp.x;
			T m11 = aUp.y;
			T m12 = aUp.z;
			T m20 = aForward.x;
			T m21 = aForward.y;
			T m22 = aForward.z;

			T num8 = (m00 + m11) + m22;
			Quaternion quaternion;
			if (num8 > T(0))
			{
				T num = static_cast<T>(sqrtf(num8 + T(1)));
				quaternion.w = num * T(0.5f);
				num = T(0.5f) / num;
				quaternion.x = (m12 - m21) * num;
				quaternion.y = (m20 - m02) * num;
				quaternion.z = (m01 - m10) * num;
				return quaternion;
			}
			if ((m00 >= m11) && (m00 >= m22))
			{
				T num7 = static_cast<T>(sqrtf(((T(1) + m00) - m11) - m22));
				T num4 = T(0.5f) / num7;
				quaternion.x = T(0.5f) * num7;
				quaternion.y = (m01 + m10) * num4;
				quaternion.z = (m02 + m20) * num4;
				quaternion.w = (m12 - m21) * num4;
				return quaternion;
			}
			if (m11 > m22)
			{
				T num6 = static_cast<T>(sqrtf(((T(1) + m11) - m00) - m22));
				T num3 = T(0.5f) / num6;
				quaternion.x = (m10 + m01) * num3;
				quaternion.y = T(0.5f) * num6;
				quaternion.z = (m21 + m12) * num3;
				quaternion.w = (m20 - m02) * num3;
				return quaternion;
			}

			T num5 = static_cast<T>(sqrtf(((T(1) + m22) - m00) - m11));
			T num2 = T(0.5f) / num5;
			quaternion.x = (m20 + m02) * num2;
			quaternion.y = (m21 + m12) * num2;
			quaternion.z = T(0.5f) * num5;
			quaternion.w = (m01 - m10) * num2;
			return quaternion;
		}

		static Quaternion rotateTowards(Quaternion& aFrom, Quaternion& aTo, T aMaxDegreesDelta)
		{
			T num = angle(aFrom, aTo);
			if(num == T(0))
			{
				return aTo;
			}

			T t = static_cast<T>(std::min(T(1), aMaxDegreesDelta / num));
			return glm::slerp(aFrom._internal_value, aTo._internal_value, t);
		}

		static Quaternion euler(const Vector3<T>& aEuler)
		{
			T x = glm::radians(aEuler.x);
			T y = glm::radians(aEuler.y);
			T z = glm::radians(aEuler.z);

			T sx = sin(x / 2); T sy = sin(y / 2); T sz = sin(z / 2);
			T cx = cos(x / 2); T cy = cos(y / 2); T cz = cos(z / 2);

			return Quaternion(cx * cy * cz + sx * sy * sz,
				sx * cy * cz - cx * sy * sz,
				cx * sy * cz + sx * cy * sz,
				cx * cy * sz - sx * sy * cz);
		}

		Quaternion inverse() const
		{
			return glm::inverse(_internal_value);
		}

		void conjugate()
		{
			glm::conjugate(_internal_value);
		}

		Quaternion& slerp(const Quaternion& aOther, T aBlend)
		{
			_internal_value = glm::slerp(_internal_value, aOther._internal_value, aBlend);
			return *this;
		}

		union
		{
			struct
			{
				T x, y, z, w;
			};

			T v[4];

			detail::QuaternionType<T> _internal_value;
		};

	private:
		Quaternion(const detail::QuaternionType<T>& aValue) : Quaternion()
		{
			_internal_value = aValue;
		}
};

template<typename T>
Vector3<T> operator * (const Quaternion<T>& aLeft, const Vector3<T>& aRight)
{
	auto value = aLeft._internal_value * aRight._internal_value;
	return Vector3<T>(value.x, value.y, value.z);
}

using Quaternionf = Quaternion<float>;
using Quaterniond = Quaternion<double>;

#endif // quaternion_h__