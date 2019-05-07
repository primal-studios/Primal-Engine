#ifndef mesh_h__
#define mesh_h__
#include "graphics/VertexArray.h"

class Mesh
{
	public:
		Mesh(VertexArray* aArray);
		~Mesh();

		VertexArray* getVAO() const;

	private:
		VertexArray* mVAO;
};

#endif // mesh_h__