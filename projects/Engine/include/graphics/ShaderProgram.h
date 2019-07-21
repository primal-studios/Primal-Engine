#ifndef shaderprogram_h__
#define shaderprogram_h__

#include <vector>

#include "graphics/Shader.h"

class ShaderProgram
{
	public:
		ShaderProgram(Shader* aVertex, Shader* aFragment, Shader* aGeometry = nullptr, Shader* aTessEvaluation = nullptr, Shader* aTessControl = nullptr);
		~ShaderProgram();

		void compile();

		void bind() const;
		void unbind() const;

		uint32_t getId() const { return mId; }

		Shader* getVertexShader() const { return mShaders[0]; };
		Shader* getFragmentShader() const { return mShaders[1]; };
		Shader* getGeometryShader() const { return mShaders[2]; };
		Shader* getTEShader() const { return mShaders[3]; };
		Shader* getTCShader() const { return mShaders[4]; };

	private:
		void _setVertexShader(Shader* aShader);
		void _setFragmentShader(Shader* aShader);
		void _setGeometryShader(Shader* aShader);
		void _setTEShader(Shader* aShader);
		void _setTCShader(Shader* aShader);

		std::vector<Shader*> mShaders;
		uint32_t mId;
};

#endif // core_shaderprogram_h__