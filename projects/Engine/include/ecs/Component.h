#ifndef component_h__
#define component_h__

class Entity;
class Component
{
	public:
		Component() = default;
		virtual ~Component();

		virtual void onEnable() {};
		virtual void onConstruct() {};
		virtual void onStart() {};

		virtual void onUpdate() {};
		virtual void onLateUpdate() {};

		virtual void onPreRender() {};
		virtual void onRender() {};
		virtual void onPostRender() {};

		virtual void onDestroy() {};

		Entity* entity = nullptr;
};

#endif // component_h__
