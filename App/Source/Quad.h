#pragma once
#include <GLBuffer.h>
#include <Shader.h>
#include <GLFW/glfw3.h>
#include <memory>

#include <glm/gtc/type_ptr.hpp>

class Quad
{
public:
	Quad(glm::vec2 size, std::shared_ptr<Scape::Shader> shader);
	~Quad();

	void Render(bool toFbo);
	void SetResolution(float width, float height);
	
	GLuint GetTextureId() { return _fbo->GetTextureId(); }
	glm::vec3 GetSize() const { return _pixelSize; }
private:
	glm::vec3 _pixelSize;
	std::shared_ptr<Scape::Shader> _shaderProgram;

	std::unique_ptr<FrameBufferObject> _fbo;
	std::unique_ptr<VertexArrayObject> _vao;
	std::unique_ptr<VertexBufferObject> _vbo;
	std::unique_ptr<ElementBufferObject> _ebo;
};