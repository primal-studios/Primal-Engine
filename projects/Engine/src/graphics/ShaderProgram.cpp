#include "graphics/ShaderProgram.h"

#include "glad/glad.h"

#include <vector>
#include "core/Log.h"

ShaderProgram::ShaderProgram(Shader* aVertex, Shader* aFragment, Shader* aGeometry, Shader* aTessEvaluation, Shader* aTessControl)
{
	mShaders.resize(5);

	_setVertexShader(aVertex);
	_setFragmentShader(aFragment);
	_setGeometryShader(aGeometry);
	_setTEShader(aTessEvaluation);
	_setTCShader(aTessControl);

	compile();
}

ShaderProgram::~ShaderProgram()
{
	for (auto shader : mShaders)
	{
		delete shader;
	}

	glDeleteProgram(mId);
}

void ShaderProgram::compile()
{
	mId = glCreateProgram();

	for (auto shader : mShaders)
	{
		if (shader != nullptr)
		{
			glAttachShader(mId, shader->getId());
		}
	}

	glLinkProgram(mId);

	int32_t result;
	glGetProgramiv(mId, GL_LINK_STATUS, &result);
	if (result == GL_FALSE)
	{
		int32_t errorLogLength;
		glGetProgramiv(mId, GL_INFO_LOG_LENGTH, &errorLogLength);

		std::vector<char> error(errorLogLength);
		glGetProgramInfoLog(mId, errorLogLength, &errorLogLength, &error[0]);
		const std::string errorMessage = &error[0];

		PRIMAL_ERROR("Failed to link program!");
		PRIMAL_ERROR(errorMessage.c_str());

		glDeleteProgram(mId);

		for (auto shader : mShaders)
		{
			delete shader;
		}

		return;
	}

	glValidateProgram(mId);
	glGetProgramiv(mId, GL_VALIDATE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int32_t errorLogLength;
		glGetProgramiv(mId, GL_INFO_LOG_LENGTH, &errorLogLength);

		std::vector<char> error(errorLogLength);
		glGetProgramInfoLog(mId, errorLogLength, &errorLogLength, &error[0]);
		const std::string errorMessage = &error[0];

		PRIMAL_ERROR("Failed to validate program!");
		PRIMAL_ERROR(errorMessage.c_str());

		glDeleteProgram(mId);

		for (auto shader : mShaders)
		{
			delete shader;
		}

		return;
	}

	for (auto shader : mShaders)
	{
		if (shader != nullptr)
		{
			glDetachShader(mId, shader->getId());
			glDeleteShader(shader->getId());
		}
	}
}

void ShaderProgram::bind() const
{
	glUseProgram(mId);
}

void ShaderProgram::unbind() const
{
	glUseProgram(0);
}

void ShaderProgram::_setVertexShader(Shader* aShader)
{
	mShaders[0] = aShader;
}

void ShaderProgram::_setFragmentShader(Shader* aShader)
{
	mShaders[1] = aShader;
}

void ShaderProgram::_setGeometryShader(Shader* aShader)
{
	mShaders[2] = aShader;
}

void ShaderProgram::_setTEShader(Shader* aShader)
{
	mShaders[3] = aShader;
}

void ShaderProgram::_setTCShader(Shader* aShader)
{
	mShaders[4] = aShader;
}