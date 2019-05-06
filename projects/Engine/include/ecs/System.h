#ifndef system_h__
#define system_h__

class System
{
	public:
		System() = default;
		virtual ~System() = default;

		virtual void initialize() {}

		virtual void update() {}
		virtual void lateUpdate() {}
		virtual void fixedUpdate() {}

		virtual void preRender() {}
		virtual void render() {}
		virtual void postRender() {}

		virtual void dispose() {}
};

#endif // system_h__