#pragma once
#include <Uniform.h>
#include <vector>
#include <map>
#include <string>
#include <set>
#include <functional>
#include <memory>

namespace Scape
{
	class Shader
	{
	public:
		Shader()  { _programId = glCreateProgram(); }
		~Shader() { glDeleteProgram(_programId); }

		GLuint GetProgramID() { return _programId; }

		GLuint CreateShader(const std::string& shaderSource, GLenum shaderType);
		void AttachShader(GLuint shaderId) { glAttachShader(_programId, shaderId); }
		void DetachShader(GLuint shaderId) { glDetachShader(_programId, shaderId); }
		void DeleteShader(GLuint shaderId) { glDetachShader(_programId, shaderId);  glDeleteShader(shaderId); }

		void Link();
		void AddOnLink(std::function<void()> callback) { _linkCallbacks.push_back(callback); }

		void Activate() { glUseProgram(_programId); }
		void Deactivate() { glUseProgram(0); }

		GLint GetUniformCount() { GLint count;  glGetProgramiv(_programId, GL_ACTIVE_UNIFORMS, &count); return count; }
		std::map<std::string, std::shared_ptr<Uniform>> GetActiveUniforms() { return _activeUniforms; }

		template<typename T>
		T* GetUniformBuffer(std::string uniformName)
		{
			if (_activeUniforms.find(uniformName) != _activeUniforms.end())
				return _activeUniforms[uniformName]->GetBuffer<T>();
			return nullptr;
		}

		GLenum GetUniformType(std::string uniformName);
		GLint GetUniformLocation(std::string uniformName);
		size_t GetUniformSize(std::string uniformName);

		void SendUniform(std::string uniformName);
		void RetreiveUniform(std::string uniformName);
		void SetUniformBuffer(std::string uniformName, const void* data);

		

		inline static const char* SHADER_DIR = "../Shaders/";
		inline static const std::set<std::string> SUPPORTED_EXTENSIONS = { ".f", ".frag" , ".fragment" };
	private:
		// Disable copying and assignment
		Shader(Shader const&) = delete;
		Shader& operator =(Shader const&) = delete;

		void RetreiveActiveUniforms(bool preserve = false);
		std::vector<std::function<void()>> _linkCallbacks;

		GLuint _programId;
		std::vector<GLuint> _shaderIds;

		std::map<std::string, std::shared_ptr<Uniform>> _activeUniforms;
	};
}