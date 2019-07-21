#include "graphics/Shader.h"

#include "glad/glad.h"

#include <vector>
#include "core/Log.h"
#include "utils/StringUtils.h"

static uint32_t sShaderTypeToOpenGl(const ShaderType& aType)
{
	switch (aType)
	{
	case ShaderType::Vertex: return GL_VERTEX_SHADER;
	case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
	case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
	case ShaderType::TesselationEvaluation: return GL_TESS_EVALUATION_SHADER;
	case ShaderType::TesselationControl: return GL_TESS_CONTROL_SHADER;
	default: return GL_NONE;
	}
}

ShaderUniformVariable::ShaderUniformVariable()
{
	name = "";
	type = UniformType::NONE;
}

ShaderUniformVariable::~ShaderUniformVariable()
{

}

UniformType ShaderUniformVariable::stringToType(const std::string& aName)
{
	if (aName == "float")
	{
		return UniformType::FLOAT32;
	}

	if (aName == "vec2")
	{
		return UniformType::VEC2;
	}

	if (aName == "vec3")
	{
		return UniformType::VEC3;
	}

	if (aName == "vec4")
	{
		return UniformType::VEC4;
	}

	if (aName == "mat3")
	{
		return UniformType::MAT3;
	}

	if (aName == "mat4")
	{
		return UniformType::MAT4;
	}

	if (aName == "int")
	{
		return UniformType::INT32;
	}

	if (aName == "struct")
	{
		return UniformType::STRUCT;
	}

	if (aName == "bool")
	{
		return UniformType::BOOL;
	}

	return UniformType::NONE;
}

Shader::Shader(const std::string& aSource, const ShaderType& aType)
{
	mSource = aSource;
	mType = aType;

	_parse();

	compile();
}

Shader::~Shader()
{
	glDeleteShader(mId);
}

void Shader::compile()
{
	mId = glCreateShader(sShaderTypeToOpenGl(mType));

	const char* source = mSource.c_str();
	glShaderSource(mId, 1, &source, nullptr);
	glCompileShader(mId);

	int32_t result;
	glGetShaderiv(mId, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int32_t errorLogLength;
		glGetShaderiv(mId, GL_INFO_LOG_LENGTH, &errorLogLength);

		std::vector<char> error(errorLogLength);
		glGetShaderInfoLog(mId, errorLogLength, &errorLogLength, &error[0]);
		const std::string errorMessage = &error[0];

		PRIMAL_ERROR("Failed to compile shader!");
		PRIMAL_ERROR(errorMessage.c_str());

		glDeleteShader(mId);
	}
}

void Shader::_parse()
{
	std::vector<std::string> lines = StringUtils::split(mSource, '\n');
	for (auto line : lines)
	{
		line = StringUtils::stripFirst(line, '\t');
		line = StringUtils::stripFirst(line, ' ');

		if (StringUtils::startsWith(line, "uniform"))
		{
			std::vector<std::string> tokens = StringUtils::splitOnAny(mSource, " \t=();");

			if (tokens.size() == 2)
			{
				// This is a uniform buffer declaration
			}
			else
			{
				// This is a normal uniform variable
				ShaderUniformVariable* variable = new ShaderUniformVariable();
				variable->name = tokens[2];
				variable->type = ShaderUniformVariable::stringToType(tokens[1]);

			}

		}
	}
}


