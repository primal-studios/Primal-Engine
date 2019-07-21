#ifndef shader_h__
#define shader_h__

#include <string>

enum class ShaderType
{
	Vertex = 0,
	Fragment,
	Geometry,
	TesselationEvaluation,
	TesselationControl
};

enum class UniformType : uint32_t
{
	NONE,
	FLOAT32,
	VEC2,
	VEC3,
	VEC4,
	MAT3,
	MAT4,
	INT32,
	STRUCT,
	BOOL,
	SAMPLER2D,
	SAMPLER2DSHADOW,
	SAMPLER2DRECT,
	SAMPLERCUBE
};

class ShaderUniformVariable
{
	public:
		ShaderUniformVariable();
		~ShaderUniformVariable();

		std::string name;
		UniformType type;

		static UniformType stringToType(const std::string& aName);

};

class Shader
{
	public:
		Shader(const std::string& aSource, const ShaderType& aType);
		~Shader();

		void compile();

		uint32_t getId() const { return mId; };

	private:
		std::string mSource;
		uint32_t mId;
		ShaderType mType;

		void _parse();
};

#endif // core_shader_h__