#ifndef transformcomponent_h__
#define transformcomponent_h__

#include <Eigen/Eigen>

#include "ecs/Component.h"

#include "core/Property.h"

class TransformComponent final : public Component
{
	public:
		TransformComponent()
		{
			position = Eigen::Vector3f(0, 0, 0);
			rotation = Eigen::Vector3f(0, 0, 0);
			scale = Eigen::Vector3f(1, 1, 1);

			forward = Eigen::Vector3f(0, 0, 1);
			right = Eigen::Vector3f(1, 0, 0);
			up = Eigen::Vector3f(0, 1, 0);
		}

		~TransformComponent()
		{
			
		}

		Eigen::Vector3f position;
		Eigen::Vector3f rotation;
		Eigen::Vector3f scale;

		Eigen::Vector3f forward;
		Eigen::Vector3f right;
		Eigen::Vector3f up;

	private:
		Eigen::Quaternionf mInternalRotation;
};

#endif // transformcomponent_h__