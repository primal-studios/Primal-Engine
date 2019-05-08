#ifndef transformcomponent_h__
#define transformcomponent_h__

#include <Eigen/Eigen>

#include "core/Property.h"
#include "ecs/Component.h"
#include "math/QuaternionUtils.h"

class TransformComponent final : public Component
{
	public:
		TransformComponent()
		{
			eulerAngles.getCallback([=](Eigen::Vector3f & aValue)
			{
				return QuaternionUtils::toEuler(*rotation);
			});

			eulerAngles.setCallback([=](Eigen::Vector3f& aValue)
			{
				rotation.directSet(QuaternionUtils::fromEuler(aValue));
				return aValue;
			});

			rotation.getCallback([=](Eigen::Quaternionf& aValue)
			{
				return aValue;
			});

			rotation.setCallback([=](Eigen::Quaternionf& aValue)
			{
				return aValue;
			});

			scale.getCallback([=](Eigen::Vector3f& aValue)
			{
				return aValue;
			});

			scale.setCallback([=](Eigen::Vector3f& aValue)
			{
				return aValue;
			});

			right.getCallback([=](Eigen::Vector3f& aValue)
			{
				const Eigen::Vector3f right = Eigen::Vector3f(1, 0, 0);
				return *rotation * right;
			});

			right.setCallback([=](Eigen::Vector3f& aValue)
			{
				Eigen::Vector3f right = Eigen::Vector3f(1, 0, 0);
				rotation.directSet(QuaternionUtils::fromToRotation(right, aValue));
				return aValue;
			});

			up.getCallback([=](Eigen::Vector3f& aValue)
			{
				const Eigen::Vector3f up = Eigen::Vector3f(0, 1, 0);
				return *rotation * up;
			});

			up.setCallback([=](Eigen::Vector3f & aValue)
			{
				Eigen::Vector3f up = Eigen::Vector3f(0, 1, 0);
				rotation.directSet(QuaternionUtils::fromToRotation(up, aValue));
				return aValue;
			});

			forward.getCallback([=](Eigen::Vector3f& aValue)
			{
				const Eigen::Vector3f forward = Eigen::Vector3f(0, 0, 1);
				return *rotation * forward;
			});

			forward.setCallback([=](Eigen::Vector3f& aValue)
			{
				Eigen::Vector3f up = Eigen::Vector3f(0, 1, 0);
				rotation.directSet(QuaternionUtils::lookRotation(aValue, up));
				return aValue;
			});
		}

		~TransformComponent()
		{
			
		}

		Property<Eigen::Vector3f> position = Eigen::Vector3f(0, 0, 0);
		Property<Eigen::Quaternionf> rotation = QuaternionUtils::fromEuler(Eigen::Vector3f(0, 0, 0));
		Property<Eigen::Vector3f> eulerAngles = Eigen::Vector3f(0, 0, 0);
		Property<Eigen::Vector3f> scale = Eigen::Vector3f(1, 1, 1);

		Property<Eigen::Vector3f> forward = Eigen::Vector3f(0, 0, 1);
		Property<Eigen::Vector3f> right = Eigen::Vector3f(1, 0, 0);
		Property<Eigen::Vector3f> up = Eigen::Vector3f(0, 1, 0);
};

#endif // transformcomponent_h__