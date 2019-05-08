#ifndef quaternionutils_h__
#define quaternionutils_h__

#include <Eigen/Eigen>
#include <stdlib.h>
#include <math.h>

constexpr float internal_quat_radToDeg = static_cast<float>(180.0f / EIGEN_PI);
constexpr float internal_quat_degToRad = static_cast<float>(EIGEN_PI / 180.0f);

class QuaternionUtils
{
	public:
		static Eigen::Quaternionf fromEuler(const Eigen::Vector3f& aEuler);
		static Eigen::Vector3f toEuler(const Eigen::Quaternionf& aQuaternion);

		static Eigen::Quaternionf lookRotation(Eigen::Vector3f& aForward, Eigen::Vector3f& aUp)
		{
			aForward.normalize();

			Eigen::Vector3f vector = aForward.normalized();
			Eigen::Vector3f vector2 = aUp.cross(vector).normalized();
			Eigen::Vector3f vector3 = vector.cross(vector2);

			auto m00 = vector2.x();
			auto m01 = vector2.y();
			auto m02 = vector2.z();
			auto m10 = vector3.x();
			auto m11 = vector3.y();
			auto m12 = vector3.z();
			auto m20 = vector.x();
			auto m21 = vector.y();
			auto m22 = vector.z();

			float num8 = (m00 + m11) + m22;
			Eigen::Quaternionf quaternion;
			if(num8 > 0.0f)
			{
				auto num = sqrtf(num8 + 1.0f);

				auto w = num * 0.5f;

				num = 0.5f / num;
				auto x = (m12 - m21) * num;
				auto y = (m20 - m02) * num;
				auto z = (m01 - m10) * num;

				quaternion = Eigen::Quaternionf(x, y, z, w);
				return quaternion;
			}
			if((m00 > m11) && (m00 >= m22))
			{
				float num7 = sqrtf(((1.0f + m00) - m11) - m22);
				auto num4 = 0.5f / num7;

				auto x = 0.5f * num7;
				auto y = (m01 + m10) * num4;
				auto z = (m02 + m20) * num4;
				auto w = (m12 - m21) * num4;

				quaternion = Eigen::Quaternionf(x, y, z, w);
				return quaternion;
			}
			if(m11 > m22)
			{
				auto num6 = sqrtf(((1.0f + m11) - m00) - m22);
				auto num3 = 0.5f / num6;

				auto x = (m10 + m01) * num3;
				auto y = 0.5f * num6;
				auto z = (m21 + m12) * num3;
				auto w = (m20 - m02) * num3;

				quaternion = Eigen::Quaternionf(x, y, z, w);
				return quaternion;
			}

			float num5 = sqrtf(((1.0f + m22) - m00) - m11);
			auto num2 = 0.5f / num5;

			auto x = (m20 + m02) * num2;
			auto y = (m21 + m12) * num2;
			auto z = 0.5f * num5;
			auto w = (m01 - m10) * num2;

			quaternion = Eigen::Quaternionf(x, y, z, w);
			return quaternion;
		}

		static Eigen::Quaternionf fromToRotation(Eigen::Vector3f& aFromDirection, Eigen::Vector3f& aToDirection)
		{
			Eigen::Vector3f up = Eigen::Vector3f(0, 1, 0);

			auto from = lookRotation(aFromDirection, up);
			auto to = lookRotation(aToDirection, up);

			return rotateTowards(from, to, std::numeric_limits<float>::max());
		}

		static Eigen::Quaternionf rotateTowards(Eigen::Quaternionf& aFrom, Eigen::Quaternionf& aTo, float aMaxDegreesDelta)
		{
			const float num = angle(aFrom, aTo);
			if(num == 0.0f)
			{
				return aTo;
			}

			const float t = std::min(1.0f, aMaxDegreesDelta / num);
			return aFrom.slerp(t, aTo);
		}

		static float angle(Eigen::Quaternionf& aFrom, Eigen::Quaternionf& aTo)
		{
			const float f = aFrom.dot(aTo);
			return acosf(std::min(abs(f), 1.0f))* 2.0f* internal_quat_radToDeg;
		}
};
#endif // quaternionutils_h__