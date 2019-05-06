#ifndef component_h__
#define component_h__

class Entity;
class Component
{
	public:
		Component() = default;
		virtual ~Component() = default;

		virtual void onEnable() {};
		virtual void onConstruct() {};
		virtual void onStart() {};

		virtual void onUpdate() {};
		virtual void onLateUpdate() {};

		virtual void onPreRender() {};
		virtual void onRender() {};
		virtual void onPostRender() {};

		virtual void onDestroy() {};

		static void destroy(Component* aComponent);

		Entity* entity;
};

#endif // component_h__
