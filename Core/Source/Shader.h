#pragma once
#include <Uniform.h>
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
		Shader() {
			_programId = glCreateProgram(); 
			_isEmpty = true; 
		}
		~Shader() { glDeleteProgram(_programId); }

		GLuint GetProgramID() { return _programId; }

		GLuint CreateShader(const std::string& shaderSource, GLenum shaderType);
		void AttachShader(GLuint shaderId) { glAttachShader(_programId, shaderId); }
		void DetachShader(GLuint shaderId) { glDetachShader(_programId, shaderId); }
		void DeleteShader(GLuint shaderId) { glDeleteShader(shaderId); }

		void Link();
		void AddOnLink(std::function<void()> callback) { _linkCallbacks.push_back(callback); }

		void Activate() { glUseProgram(_programId); }
		void Deactivate() { glUseProgram(0); }

		bool Empty() { return _isEmpty; }

		struct UniformInfo
		{
			GLenum Type;
			GLint Location;
		};
		std::map<std::string, UniformInfo> GetActiveUniforms();

		GLuint GetUniformBlockIndex(const char* name) { return glGetUniformBlockIndex(_programId, name); }
		GLint GetUniformLocation(const char* name) { return glGetUniformLocation(_programId, name); }
		void SendUniform(GLuint location, GLenum type, void* buffer);
		void SendUniform(const char* name, GLenum type, void* buffer);
		void RetreiveUniform(GLuint location, GLenum type, void* buffer);

	private:
		// Disable copying and assignment
		Shader(Shader const&) = delete;
		Shader& operator =(Shader const&) = delete;

		
		std::vector<std::function<void()>> _linkCallbacks;

		GLuint _programId;
		bool _isEmpty;
	};
}