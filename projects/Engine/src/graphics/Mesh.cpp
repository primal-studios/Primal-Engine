#include "graphics/Mesh.h"
#include "graphics/GraphicsFactory.h"

Mesh::Mesh()
	: mVertexBuffer(nullptr), mIndexBuffer(nullptr)
{
}

Mesh::~Mesh()
{
	delete mVertexBuffer;
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

	VertexBufferCreateInfo vBufferCreateInfo = {};
	vBufferCreateInfo.flags = 0;
	vBufferCreateInfo.sharingMode = SHARING_MODE_EXCLUSIVE;
	vBufferCreateInfo.usage = EBufferUsageFlagBits::BUFFER_USAGE_VERTEX_BUFFER | EBufferUsageFlagBits::BUFFER_USAGE_TRANSFER_DST;
	vBufferCreateInfo.size = mVertices.size() * sizeof(Vertex);

	mVertexBuffer = GraphicsFactory::instance().createVertexBuffer();

	BufferLayout layout;
	layout.push<Vector3f>("iPosition");
	layout.push<Vector2f>("iTexCoord");
	layout.push<Vector3f>("iNormal");
	layout.push<Vector3f>("iTangent");
	layout.push<Vector3f>("iBinormal");
	layout.push<Vector4f>("iColor");
	
	mVertexBuffer->setLayout(layout);
	mVertexBuffer->construct(vBufferCreateInfo);

	IndexBufferCreateInfo iBufferCreateInfo = {};
	iBufferCreateInfo.flags = 0;
	iBufferCreateInfo.sharingMode = SHARING_MODE_EXCLUSIVE;
	iBufferCreateInfo.usage = EBufferUsageFlagBits::BUFFER_USAGE_INDEX_BUFFER | EBufferUsageFlagBits::BUFFER_USAGE_TRANSFER_DST;
	iBufferCreateInfo.size = triangles.size() * sizeof(uint16_t);

	mIndexBuffer = GraphicsFactory::instance().createIndexBuffer();
	mIndexBuffer->construct(iBufferCreateInfo);
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

IVertexBuffer* Mesh::getVBO() const
{
	return mVertexBuffer;
}

IIndexBuffer* Mesh::getIBO() const
{
	return mIndexBuffer;
}

void* Mesh::getData() const
{
	return const_cast<Vertex*>(mVertices.data());
}

void* Mesh::getIndices() const
{
	return const_cast<uint16_t*>(triangles.data());
}

size_t Mesh::getIndicesSize() const
{
	return triangles.size() * sizeof(uint16_t);
}

size_t Mesh::getSize() const
{
	return mVertices.size() * sizeof(Vertex);
}
