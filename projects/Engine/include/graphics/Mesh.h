#ifndef mesh_h__
#define mesh_h__

#include <vector>

#include "math/Vector3.h"
#include "graphics/api/IIndexBuffer.h"
#include "graphics/api/IVertexBuffer.h"

struct Vertex
{
	Vector3f position;
	Vector2f uv;
	Vector3f normal;
	Vector3f tangent;
	Vector3f binormal;
	Vector4f color;
};

class Mesh
{
	public:
		Mesh();
		~Mesh();

		void build();
		void recalculateTangents();
		void recalculateNormals();

		void calculateBinormals();
		void triangulate();

		std::vector<Vector3f> positions;
		std::vector<Vector2f> uvs;
		std::vector<Vector3f> normals;
		std::vector<Vector3f> tangents;
		std::vector<Vector3f> binormals;
		std::vector<Vector4f> colors;

		std::vector<uint16_t> triangles;

		IVertexBuffer* getVBO() const;
		IIndexBuffer* getIBO() const;

		void* getData() const;
		void* getIndices() const;
		size_t getIndicesSize() const;
		size_t getSize() const;

	private:
		IVertexBuffer* mVertexBuffer;
		IIndexBuffer* mIndexBuffer;

		std::vector<Vertex> mVertices;
};

#endif // mesh_h__