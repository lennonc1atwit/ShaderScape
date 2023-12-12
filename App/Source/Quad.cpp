#include "Quad.h"



Quad::Quad(glm::vec2 size, std::shared_ptr<Scape::Shader> shader)
{
    std::vector<Vertex> _vertexData = {
        {
            glm::vec3(-1.0f, 1.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
            //glm::vec2(-1.0f, 1.0f)
        },
        {
            glm::vec3(1.0f, 1.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
            //glm::vec2(1.0f, 1.0f)
        },
        {
            glm::vec3(1.0f, -1.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
            //::vec2(1.0f, -1.0f)
        },
        {
            glm::vec3(-1.0f, -1.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
            //glm::vec2(-1.0f, 1.0f)
        }
    };

    std::vector<GLuint> _elements = {
        0, 1, 3,
        1, 2, 3
    };

    _shaderProgram = shader;

    _vao = std::make_unique<VertexArrayObject>();
    _vbo = std::make_unique<VertexBufferObject>();
    _ebo = std::make_unique<ElementBufferObject>();
    _fbo = std::make_unique<FrameBufferObject>();

    _vao->GenBuffers(1);
    _vao->Bind();

    _vbo->GenBuffers(1);
    _vbo->UpdateBuffers(&_vertexData);

    _ebo->GenBuffers(1);
    _ebo->UpdateBuffers(&_elements);

    _vao->UpdateBuffers(_vbo.get());

    _vao->Unbind();
    _vbo->Unbind();
    _ebo->Unbind();

    SetResolution(size.x, size.y);
}

Quad::~Quad()
{

}

void Quad::Render(bool toFbo = true)
{
    if(toFbo) _fbo->Bind();
    _shaderProgram->Activate();

    _vao->Bind();
    _ebo->Bind();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    _shaderProgram->Deactivate();
    if (toFbo) _fbo->UnBind();
    
}

void Quad::SetResolution(float width, float height)
{
    _pixelSize.x = width; _pixelSize.y = height; _pixelSize.z = width / height;
    _fbo->GenBuffers(1, (GLuint)_pixelSize.x, (GLuint)_pixelSize.y);
}
