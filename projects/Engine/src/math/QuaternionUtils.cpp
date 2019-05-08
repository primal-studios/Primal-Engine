#include "math/QuaternionUtils.h"

Eigen::Quaternionf QuaternionUtils::fromEuler(const Eigen::Vector3f& aEuler)
{
	const Eigen::Quaternionf q = Eigen::AngleAxisf(aEuler.x(), Eigen::Vector3f::UnitX()) *
			Eigen::AngleAxisf(aEuler.y(), Eigen::Vector3f::UnitY()) *
			Eigen::AngleAxisf(aEuler.z(), Eigen::Vector3f::UnitZ());

	return q;
}

Eigen::Vector3f QuaternionUtils::toEuler(const Eigen::Quaternionf& aQuaternion)
{
	auto euler = aQuaternion.toRotationMatrix().eulerAngles(0, 1, 2);
	return Eigen::Vector3f(euler.x(), euler.y(), euler.z());
}

