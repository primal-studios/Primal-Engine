#ifndef mesh_h__
#define mesh_h__

#include "graphics/VertexArray.h"

class Mesh
{
	public:
		explicit Mesh(VertexArray* aArray)
			: mVAO(aArray) {}
		~Mesh() = default;

		VertexArray* getVAO() const { return mVAO; }

	private:
		VertexArray* mVAO;
};

#endif // mesh_h__