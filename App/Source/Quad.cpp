#include "Quad.h"



Quad::Quad(glm::vec2 size)
{
    _pixelSize = glm::vec3(size, size.x / size.y);

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

    _shaderProgram = std::make_shared<Scape::Shader>();

    _vao = std::make_unique<VertexArrayObject>();
    _vbo = std::make_unique<VertexBufferObject>();
    _ebo = std::make_unique<ElementBufferObject>();
    _fbo = std::make_unique<FrameBufferObject>();

    UpdateResolution();

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

    std::function<void()> callback = std::bind(&Quad::OnShaderLink, this);
    _shaderProgram->AddOnLink(callback);
}

Quad::~Quad()
{

}

void Quad::OnShaderLink()
{
    UpdateResolution();
}

void Quad::Render(bool toFbo = true)
{
    if (_shouldResize) {
        UpdateResolution();
        _shouldResize = false;
    }

    
    if(toFbo) _fbo->Bind();
    _shaderProgram->Activate();

    _vao->Bind();
    _ebo->Bind();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    _shaderProgram->Deactivate();
    if (toFbo) _fbo->UnBind();
    
}

bool Quad::SetShaderFile(std::string shaderPath)
{
    _userShader = _shaderProgram->CreateShader(shaderPath, GL_FRAGMENT_SHADER);

    _shaderProgram->AttachShader(_userShader);
    _shaderProgram->Link();

    UpdateResolution();

    return true;
}

void Quad::RemoveShader()
{
    _shaderProgram->DetachShader(_userShader);
    _shaderProgram->DeleteShader(_userShader);
    _shaderProgram->Link();
}
