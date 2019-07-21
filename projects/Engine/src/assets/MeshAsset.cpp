#include "assets/MeshAsset.h"

#include <fx/gltf.h>

#include "utils/StringUtils.h"
#include "math/Vector3.h"

namespace detail
{
	struct BufferInfo
	{
		fx::gltf::Accessor const* accessor;

		uint8_t const* data;
		uint32_t dataStride;
		uint32_t totalSize;

		bool hasData() const noexcept
		{
			return data != nullptr;
		}
	};

	static uint32_t sCalculateDataTypeSize(const fx::gltf::Accessor& aAccessor) noexcept
	{
		uint32_t elementSize;
		switch(aAccessor.componentType)
		{
			case fx::gltf::Accessor::ComponentType::Byte:
			case fx::gltf::Accessor::ComponentType::UnsignedByte:
				elementSize = 1;
				break;

			case fx::gltf::Accessor::ComponentType::Short:
			case fx::gltf::Accessor::ComponentType::UnsignedShort:
				elementSize = 2;
				break;

			case fx::gltf::Accessor::ComponentType::Float:
			case fx::gltf::Accessor::ComponentType::UnsignedInt:
				elementSize = 4;
				break;

			default: 
				elementSize = 0;
				break;
		}

		switch(aAccessor.type)
		{
			case fx::gltf::Accessor::Type::Mat2:
				return 4 * elementSize;
			case fx::gltf::Accessor::Type::Mat3:
				return 9 * elementSize;
			case fx::gltf::Accessor::Type::Mat4:
				return 16 * elementSize;
			case fx::gltf::Accessor::Type::Scalar:
				return elementSize;
			case fx::gltf::Accessor::Type::Vec2:
				return 2 * elementSize;
			case fx::gltf::Accessor::Type::Vec3:
				return 3 * elementSize;
			case fx::gltf::Accessor::Type::Vec4:
				return 4 * elementSize;
			default: 
				return 0;
		}
	}

	static uint32_t sGetAccessorTypeSize(fx::gltf::Accessor const& aAccessor)
	{
		switch (aAccessor.componentType)
		{
			case fx::gltf::Accessor::ComponentType::Byte:
			case fx::gltf::Accessor::ComponentType::UnsignedByte:
				return 1;

			case fx::gltf::Accessor::ComponentType::Short:
			case fx::gltf::Accessor::ComponentType::UnsignedShort:
				return 2;

			case fx::gltf::Accessor::ComponentType::Float:
			case fx::gltf::Accessor::ComponentType::UnsignedInt:
				return 4;

			default:
				return 0;
		}
	}

	static BufferInfo sGetData(fx::gltf::Document const& aDocument, fx::gltf::Accessor const& aAccessor)
	{
		fx::gltf::BufferView const& bufferView = aDocument.bufferViews[aAccessor.bufferView];
		fx::gltf::Buffer const& buffer = aDocument.buffers[bufferView.buffer];

		const uint32_t dataTypeSize = sCalculateDataTypeSize(aAccessor);
		return BufferInfo{ &aAccessor, &buffer.data[static_cast<uint64_t>(bufferView.byteOffset) + aAccessor.byteOffset], dataTypeSize, aAccessor.count * dataTypeSize };
	}
}

MeshAsset::MeshAsset(const std::string& aPath)
{
	mPath = aPath;
}

MeshAsset::~MeshAsset()
{
	for(size_t i = 0; i < mMeshes.size(); i++)
	{
		delete mMeshes[i];
	}

	mMeshes.clear();
}

Mesh* MeshAsset::getMesh(const size_t aIndex)
{
	if(aIndex < mMeshes.size())
	{
		return mMeshes[aIndex];
	}

	return nullptr;
}

void MeshAsset::_load()
{
	const bool isText = StringUtils::endsWith(mPath, "gltf");
	const bool isBinary = StringUtils::endsWith(mPath, "glb");

	fx::gltf::Document document;

	if (isText)
	{
		document = fx::gltf::LoadFromText(mPath);
	}

	if(isBinary)
	{
		document = fx::gltf::LoadFromBinary(mPath);
	}

	//size_t sceneCount = document.scenes.size();
	//size_t meshCount = document.meshes.size();
	//size_t textureCount = document.textures.size();
	//size_t imageCount = document.images.size();
	//size_t materialCount = document.materials.size();
	//size_t cameraCount = document.cameras.size();
	//size_t nodeCount = document.nodes.size();
	//size_t skinCount = document.skins.size();
	//size_t samplerCount = document.samplers.size();
	//size_t animationCount = document.animations.size();
	//size_t accessorCount = document.accessors.size();
	//size_t bufferCount = document.buffers.size();
	//size_t bufferViewCount = document.bufferViews.size();

	for(const auto& mesh : document.meshes)
	{
		std::vector<Vector3f> positions;
		std::vector<Vector2f> uvs;
		std::vector<Vector3f> normals;
		std::vector<Vector3f> tangents;
		std::vector<Vector3f> binormals;

		std::vector<uint32_t> indices;

		for(const auto& primitive : mesh.primitives)
		{
			for(const auto& attribute : primitive.attributes)
			{
				if(attribute.first == "POSITION")
				{
					auto accessor = document.accessors[attribute.second];
					auto positionBuffer = detail::sGetData(document, accessor);

					uint32_t elementSize = detail::sGetAccessorTypeSize(accessor);

					if (positionBuffer.hasData())
					{
						for (uint32_t i = 0; i < positionBuffer.totalSize / sizeof(float); i += 3)
						{
							float x = ((float*)positionBuffer.data)[i];
							float y = ((float*)positionBuffer.data)[i + 1];
							float z = ((float*)positionBuffer.data)[i + 2];

							positions.push_back({ x, y, z });
						}
					}
				}
				else if(attribute.first == "TEXCOORD_0")
				{
					auto accessor = document.accessors[attribute.second];
					auto uvBuffer = detail::sGetData(document, accessor);

					uint32_t elementSize = detail::sGetAccessorTypeSize(accessor);

					if(uvBuffer.hasData())
					{
						for (uint32_t i = 0; i < uvBuffer.totalSize / sizeof(float); i += 2)
						{
							float x = ((float*)uvBuffer.data)[i];
							float y = ((float*)uvBuffer.data)[i + 1];

							uvs.push_back({ x, y });
						}
					}
				}
				else if(attribute.first == "NORMAL")
				{
					auto accessor = document.accessors[attribute.second];
					auto normalBuffer = detail::sGetData(document, accessor);

					uint32_t elementSize = detail::sGetAccessorTypeSize(accessor);

					if (normalBuffer.hasData())
					{
						for (uint32_t i = 0; i < normalBuffer.totalSize / sizeof(float); i += 3)
						{
							float x = ((float*)normalBuffer.data)[i];
							float y = ((float*)normalBuffer.data)[i + 1];
							float z = ((float*)normalBuffer.data)[i + 2];

							normals.push_back({ x, y, z });
						}
					}
				}
				else if(attribute.first == "TANGENT")
				{
					auto accessor = document.accessors[attribute.second];
					auto tangentBuffer = detail::sGetData(document, accessor);

					uint32_t elementSize = detail::sGetAccessorTypeSize(accessor);

					if (tangentBuffer.hasData())
					{
						for (uint32_t i = 0; i < tangentBuffer.totalSize / sizeof(float); i += 4)
						{
							float x = ((float*)tangentBuffer.data)[i];
							float y = ((float*)tangentBuffer.data)[i + 1];
							float z = ((float*)tangentBuffer.data)[i + 2];
							float w = ((float*)tangentBuffer.data)[i + 3];

							tangents.push_back({ x, y, z });
							binormals.push_back({ w, 0, 0 });
						}
					}
				}
			}

			auto indexAccessor = document.accessors[primitive.indices];
			auto indexBuffer = detail::sGetData(document, indexAccessor);

			if(indexBuffer.hasData())
			{
				for (uint32_t i = 0; i < indexBuffer.totalSize / sizeof(uint16_t); i++)
				{
					uint32_t idx = ((uint32_t*)indexBuffer.data)[i];

					indices.push_back({ idx });
				}
			}

			if(primitive.material >= 0)
			{
				auto material = document.materials[primitive.material];
			}

			Mesh* primalMesh = new Mesh();
			primalMesh->positions = positions;
			primalMesh->uvs = uvs;
			primalMesh->normals = normals;
			primalMesh->tangents = tangents;

			primalMesh->triangles = indices;

			primalMesh->calculateBinormals();
			primalMesh->build();

			mMeshes.push_back(primalMesh);
		}
	}
}
