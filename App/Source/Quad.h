#pragma once
#include <GLBuffer.h>
#include <Shader.h>
#include <GLFW/glfw3.h>

#include <memory>

#include <glm/gtc/type_ptr.hpp>

class Quad
{
public:
	Quad(glm::vec2 size);
	~Quad();

	void Render(bool toFbo);

	void SetResolution(float width, float height)
	{
		_pixelSize.x = width; _pixelSize.y = height; _pixelSize.z = width / height;
		UpdateResolution();
	}

	void SetTime(float time) { _shaderProgram->SetUniformBuffer("iTime", &time); }

	bool SetShaderFile(std::string shaderPath);
	void RemoveShader();
	
	void OnShaderLink();

	std::shared_ptr<Scape::Shader> GetShaderProgram() { return _shaderProgram; }
	GLuint GetTextureId() { return _fbo->GetTextureId(); }
	glm::vec3 GetSize() { return _pixelSize; }

	double GetLastFrameTime() { return _lastFrameTime; }

	bool WindowedRender = true;
private:
	void UpdateResolution()
	{
		_shaderProgram->SetUniformBuffer("iResolution", glm::value_ptr(_pixelSize));
		_fbo->GenBuffers(1, _pixelSize.x, _pixelSize.y);

	}
	glm::vec3 _pixelSize;
	std::shared_ptr<Scape::Shader> _shaderProgram;
	GLuint _userShader;


	double _lastFrameTime = 0.0;
	bool _shouldResize = true;

	std::unique_ptr<FrameBufferObject> _fbo;
	std::unique_ptr<VertexArrayObject> _vao;
	std::unique_ptr<VertexBufferObject> _vbo;
	std::unique_ptr<ElementBufferObject> _ebo;
};