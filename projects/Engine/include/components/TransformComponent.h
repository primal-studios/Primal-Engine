#ifndef transformcomponent_h__
#define transformcomponent_h__

#include "core/Property.h"
#include "ecs/Component.h"

#include "math/Vector3.h"
#include "math/Quaternion.h"

class TransformComponent final : public Component
{
	public:
		TransformComponent()
		{
			eulerAngles.getCallback([=](Vector3f & aValue)
			{
				auto quat = *rotation;
				return *quat.eulerAngles;
			});

			eulerAngles.setCallback([=](Vector3f& aValue)
			{
				rotation.directSet(Quaternionf::euler(aValue));
				return aValue;
			});

			rotation.getCallback([=](Quaternionf& aValue)
			{
				return aValue;
			});

			rotation.setCallback([=](Quaternionf& aValue)
			{
				return aValue;
			});

			scale.getCallback([=](Vector3f& aValue)
			{
				return aValue;
			});

			scale.setCallback([=](Vector3f& aValue)
			{
				return aValue;
			});

			right.getCallback([=](Vector3f& aValue)
			{
				const Vector3f right = Vector3f(1, 0, 0);
				return *rotation * right;
			});

			right.setCallback([=](Vector3f& aValue)
			{
				Vector3f right = Vector3f(1, 0, 0);
				rotation.directSet(Quaternionf::fromToRotation(right, aValue));
				return aValue;
			});

			up.getCallback([=](Vector3f& aValue)
			{
				const Vector3f up = Vector3f(0, 1, 0);
				return *rotation * up;
			});

			up.setCallback([=](Vector3f & aValue)
			{
				Vector3f up = Vector3f(0, 1, 0);
				rotation.directSet(Quaternionf::fromToRotation(up, aValue));
				return aValue;
			});

			forward.getCallback([=](Vector3f& aValue)
			{
				const Vector3f forward = Vector3f(0, 0, 1);
				return *rotation * forward;
			});

			forward.setCallback([=](Vector3f& aValue)
			{
				Vector3f up = Vector3f(0, 1, 0);
				rotation.directSet(Quaternionf::lookRotation(aValue, up));
				return aValue;
			});
		}

		~TransformComponent();

		Property<Vector3f> position = Vector3f(0, 0, 0);
		Property<Quaternionf> rotation = Quaternionf::euler(Vector3f(0, 0, 0));
		Property<Vector3f> eulerAngles = Vector3f(0, 0, 0);
		Property<Vector3f> scale = Vector3f(1, 1, 1);

		Property<Vector3f> forward = Vector3f(0, 0, 1);
		Property<Vector3f> right = Vector3f(1, 0, 0);
		Property<Vector3f> up = Vector3f(0, 1, 0);
};

#endif // transformcomponent_h__