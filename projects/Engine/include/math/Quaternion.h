#ifndef quaternion_h__
#define quaternion_h__

#include <glm/gtx/quaternion.hpp>
#include <algorithm>

#include "math/Vector3.h"
#include "math/Vector4.h"

#include "math/QuaternionType.h"

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
					T xx = glm::radians(aValue.x);
					T yy = glm::radians(aValue.y);
					T zz = glm::radians(aValue.z);

					T sx = sin(xx / 2); T sy = sin(yy / 2); T sz = sin(zz / 2);
					T cx = cos(xx / 2); T cy = cos(yy / 2); T cz = cos(zz / 2);

					x = cx * cy * cz + sx * sy * sz;
					y = sx * cy * cz - cx * sy * sz;
					z = cx * sy * cz + sx * cy * sz;
					w = cx * cy * sz - sx * sy * cz;

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

		Quaternion angleAxis(T aAngle, const Vector3<T>& aAxis)
		{
			return glm::angleAxis(aAngle, aAxis._internal_value);
		}

		static T angle(const Quaternion& aFrom, const Quaternion& aTo)
		{
			T f = aFrom.dot(aTo);
			return static_cast<T>(glm::degrees(acosf(std::min(abs(f), T(1)) * T(2))));
		}

		static Quaternion fromToRotation(const Vector3<T>& aFromDirection, const Vector3<T>& aToDirection)
		{
			return rotateTowards(lookRotation(aFromDirection), lookRotation(aToDirection), std::numeric_limits<T>::max());
		}

		static Quaternion lookRotation(const Vector3<T>& aForward, const Vector3<T>& aUp = Vector3<T>(0, 1, 0))
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

		static Quaternion rotateTowards(const Quaternion& aFrom, const Quaternion& aTo, T aMaxDegreesDelta)
		{
			T num = angle(aFrom, aTo);
			if(num == T(0))
			{
				return aTo;
			}

			T t = static_cast<T>(std::min(T(1), aMaxDegreesDelta / num));
			return glm::slerp(aFrom, aTo, t);
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

#endif // quaternion_h__