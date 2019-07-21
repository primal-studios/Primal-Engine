#include "graphics/Mesh.h"

#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"

Mesh::Mesh()
	: mVertexArray(nullptr), mIndexBuffer(nullptr)
{
}

Mesh::~Mesh()
{
	delete mVertexArray;
	delete mIndexBuffer;
}

void Mesh::build()
{
	const size_t vertexCount = positions.size();

	for(size_t i = 0; i < vertexCount; i++)
	{
		Vertex v = {};
		v.position = positions[i];

		if(uvs.size() > i)
		{
			v.uv = uvs[i];
		}
		else
		{
			v.uv = { 0, 0 };
		}

		if(normals.size() > i)
		{
			v.normal = normals[i];
		}
		else
		{
			v.normal = { 0, 0, 0 };
		}

		if(tangents.size() > i)
		{
			v.tangent = tangents[i];
		}
		else
		{
			v.tangent = { 0,0,0 };
		}

		if(binormals.size() > i)
		{
			v.binormal = binormals[i];
		}
		else
		{
			v.binormal = { 0, 0, 0 };
		}

		if(colors.size() > i)
		{
			v.color = colors[i];
		}
		else
		{
			v.color = { 1, 1, 1, 1 };
		}

		mVertices.push_back(v);
	}

	VertexBuffer* vBuffer = new VertexBuffer(mVertices.data(), mVertices.size() * sizeof(Vertex));

	BufferLayout layout;
	layout.push<Vector3f>("iPosition");
	layout.push<Vector2f>("iTexCoord");
	layout.push<Vector3f>("iNormal");
	layout.push<Vector3f>("iTangent");
	layout.push<Vector3f>("iBinormal");
	layout.push<Vector4f>("iColor");
	
	vBuffer->setLayout(layout);
	
	IndexBuffer* iBuffer = new IndexBuffer(triangles.data(), triangles.size());

	mVertexArray = new VertexArray(vBuffer, iBuffer);
}

void Mesh::recalculateTangents()
{

}

void Mesh::recalculateNormals()
{

}

void Mesh::calculateBinormals()
{
	for (size_t i = 0; i < tangents.size(); i++)
	{
		const float w = binormals[i].x;
		binormals[i] = normals[i].cross(tangents[i]) * w;
	}
}

void Mesh::triangulate()
{

}

VertexArray* Mesh::getVAO() const
{
	return mVertexArray;
}

IndexBuffer* Mesh::getIBO() const
{
	return mIndexBuffer;
}
