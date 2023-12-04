#include "Shader.h"
#include <fstream>
#include <Logging.h>
#include <set>

GLuint Scape::Shader::CreateShader(const std::string& shaderSource, GLenum shaderType)
{
    // Open File
    static const std::string _openGlVersion = "#version 430 core\n";
    static const std::string _builtInUniforms = "precision highp float;\nuniform float iTime;\nuniform vec3 iResolution;\nuniform vec4 iMouse;\nuniform int iFrame;\nuniform float iTimeDelta;\n";
    std::string fullSource = _openGlVersion + _builtInUniforms + shaderSource;
    const char* rawSource = fullSource.c_str();

    // Create shader object
    GLuint shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &rawSource, nullptr);
    glCompileShader(shaderId);

    // Display errors
    GLint _status = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &_status);
    if (!_status)
    {
        GLint length = 0;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char* buffer = (char*)malloc(sizeof(char) * length);
        glGetShaderInfoLog(shaderId, length, nullptr, buffer);
        fprintf(stderr, "%s\n", buffer);
        free(buffer);
        return 0xFFFFFFFF;
    }

    return shaderId;
}

void Scape::Shader::Link()
{
    // Make sure we have something to link
    GLsizei shaderCount;
    glGetProgramiv(_programId, GL_ATTACHED_SHADERS, &shaderCount);

    if (shaderCount == 0) Logger::LogMessageConsole(Logger::Severity::Warn, __FILE__, __LINE__, "linking empty shader!\n");

    // Link all attached shaders
    glLinkProgram(_programId);

    // Display errors
    GLint status, length;
    glGetProgramiv(_programId, GL_LINK_STATUS, &status);
    if (!status)
    {
        // check if error
        glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &length);

        if (length == 0) {
            _activeUniforms.clear();
            return;
        }

        char* buffer = (char*)malloc(sizeof(char) * length);
        glGetProgramInfoLog(_programId, length, nullptr, buffer);
        fprintf(stderr, "%s\n", buffer);
        free(buffer);
    }
    else
    {
        RetreiveActiveUniforms();
        for (auto callback : _linkCallbacks)
            callback();
    }
}



void Scape::Shader::SendUniform(std::string uniformName)
{
    if (_activeUniforms.find(uniformName) == _activeUniforms.end())
        return;

    std::shared_ptr<Uniform> unif = _activeUniforms[uniformName];

    switch (unif->GetType())
    {
    case GL_FLOAT:              glProgramUniform1fv(_programId, unif->GetLocation(), 1, unif->GetBuffer<float>()); break;
    case GL_FLOAT_VEC2:         glProgramUniform2fv(_programId, unif->GetLocation(), 1, unif->GetBuffer<float>()); break;
    case GL_FLOAT_VEC3:         glProgramUniform3fv(_programId, unif->GetLocation(), 1, unif->GetBuffer<float>()); break;
    case GL_FLOAT_VEC4:         glProgramUniform4fv(_programId, unif->GetLocation(), 1, unif->GetBuffer<float>()); break;
    case GL_DOUBLE:             glProgramUniform1dv(_programId, unif->GetLocation(), 1, unif->GetBuffer<double>()); break;
    case GL_DOUBLE_VEC2:        glProgramUniform2dv(_programId, unif->GetLocation(), 1, unif->GetBuffer<double>()); break;
    case GL_DOUBLE_VEC3:        glProgramUniform3dv(_programId, unif->GetLocation(), 1, unif->GetBuffer<double>()); break;
    case GL_DOUBLE_VEC4:        glProgramUniform4dv(_programId, unif->GetLocation(), 1, unif->GetBuffer<double>()); break;
    case GL_INT:                glProgramUniform1iv(_programId, unif->GetLocation(), 1, unif->GetBuffer<int>()); break;
    case GL_INT_VEC2:           glProgramUniform2iv(_programId, unif->GetLocation(), 1, unif->GetBuffer<int>()); break;
    case GL_INT_VEC3:           glProgramUniform3iv(_programId, unif->GetLocation(), 1, unif->GetBuffer<int>()); break;
    case GL_INT_VEC4:           glProgramUniform4iv(_programId, unif->GetLocation(), 1, unif->GetBuffer<int>()); break;
    case GL_UNSIGNED_INT:      glProgramUniform1uiv(_programId, unif->GetLocation(), 1, unif->GetBuffer<unsigned int>()); break;
    case GL_UNSIGNED_INT_VEC2: glProgramUniform2uiv(_programId, unif->GetLocation(), 1, unif->GetBuffer<unsigned int>()); break;
    case GL_UNSIGNED_INT_VEC3: glProgramUniform3uiv(_programId, unif->GetLocation(), 1, unif->GetBuffer<unsigned int>()); break;
    case GL_UNSIGNED_INT_VEC4: glProgramUniform4uiv(_programId, unif->GetLocation(), 1, unif->GetBuffer<unsigned int>()); break;
    case GL_FLOAT_MAT2:        glProgramUniformMatrix2fv(_programId, unif->GetLocation(), 1, false, unif->GetBuffer<float>()); break;
    case GL_FLOAT_MAT3:        glProgramUniformMatrix3fv(_programId, unif->GetLocation(), 1, false, unif->GetBuffer<float>()); break;
    case GL_FLOAT_MAT4:        glProgramUniformMatrix4fv(_programId, unif->GetLocation(), 1, false, unif->GetBuffer<float>()); break;
    case GL_FLOAT_MAT2x3:      glProgramUniformMatrix2x3fv(_programId, unif->GetLocation(), 1, false, unif->GetBuffer<float>()); break;
    case GL_FLOAT_MAT3x2:      glProgramUniformMatrix3x2fv(_programId, unif->GetLocation(), 1, false, unif->GetBuffer<float>()); break;
    default: break;
    }
}

void Scape::Shader::RetreiveUniform(std::string uniformName)
{
    if (_activeUniforms.find(uniformName) == _activeUniforms.end())
        return;

    std::shared_ptr<Uniform> unif = _activeUniforms[uniformName];
    
    switch (unif->GetType())
    {
    case GL_FLOAT:             glGetUniformfv(_programId, unif->GetLocation(), unif->GetBuffer<float>()); break;
    case GL_FLOAT_VEC2:        glGetUniformfv(_programId, unif->GetLocation(), unif->GetBuffer<float>()); break;
    case GL_FLOAT_VEC3:        glGetUniformfv(_programId, unif->GetLocation(), unif->GetBuffer<float>()); break;
    case GL_FLOAT_VEC4:        glGetUniformfv(_programId, unif->GetLocation(), unif->GetBuffer<float>()); break;
    case GL_DOUBLE:            glGetUniformdv(_programId, unif->GetLocation(), unif->GetBuffer<double>()); break;
    case GL_DOUBLE_VEC2:       glGetUniformdv(_programId, unif->GetLocation(), unif->GetBuffer<double>()); break;
    case GL_DOUBLE_VEC3:       glGetUniformdv(_programId, unif->GetLocation(), unif->GetBuffer<double>()); break;
    case GL_DOUBLE_VEC4:       glGetUniformdv(_programId, unif->GetLocation(), unif->GetBuffer<double>()); break;
    case GL_INT:               glGetUniformiv(_programId, unif->GetLocation(), unif->GetBuffer<int>()); break;
    case GL_INT_VEC2:          glGetUniformiv(_programId, unif->GetLocation(), unif->GetBuffer<int>()); break;
    case GL_INT_VEC3:          glGetUniformiv(_programId, unif->GetLocation(), unif->GetBuffer<int>()); break;
    case GL_INT_VEC4:          glGetUniformiv(_programId, unif->GetLocation(), unif->GetBuffer<int>()); break;
    case GL_UNSIGNED_INT:      glGetUniformuiv(_programId, unif->GetLocation(), unif->GetBuffer<unsigned int>()); break;
    case GL_UNSIGNED_INT_VEC2: glGetUniformuiv(_programId, unif->GetLocation(), unif->GetBuffer<unsigned int>()); break;
    case GL_UNSIGNED_INT_VEC3: glGetUniformuiv(_programId, unif->GetLocation(), unif->GetBuffer<unsigned int>()); break;
    case GL_UNSIGNED_INT_VEC4: glGetUniformuiv(_programId, unif->GetLocation(), unif->GetBuffer<unsigned int>()); break;
    case GL_FLOAT_MAT2:        glGetUniformfv(_programId, unif->GetLocation(), unif->GetBuffer<float>()); break;
    case GL_FLOAT_MAT3:        glGetUniformfv(_programId, unif->GetLocation(), unif->GetBuffer<float>()); break;
    case GL_FLOAT_MAT4:        glGetUniformfv(_programId, unif->GetLocation(), unif->GetBuffer<float>()); break;
    case GL_FLOAT_MAT2x3:      glGetUniformfv(_programId, unif->GetLocation(), unif->GetBuffer<float>()); break;
    case GL_FLOAT_MAT3x2:      glGetUniformfv(_programId, unif->GetLocation(), unif->GetBuffer<float>()); break;
    default: break;
    }
}

void Scape::Shader::SetUniformBuffer(std::string uniformName, const void* data)
{
    if (_activeUniforms.find(uniformName) == _activeUniforms.end())
        return;

    auto unif = _activeUniforms[uniformName];
    memcpy(unif->GetBuffer<void*>(), data, unif->GetBufferSize());
    SendUniform(uniformName);
}

GLenum Scape::Shader::GetUniformType(std::string uniformName)
{
	if (_activeUniforms.find(uniformName) != _activeUniforms.end())
		return _activeUniforms[uniformName]->GetType();
	return GL_INVALID_ENUM;
}

GLint Scape::Shader::GetUniformLocation(std::string uniformName)
{
	if (_activeUniforms.find(uniformName) != _activeUniforms.end())
		return _activeUniforms[uniformName]->GetLocation();
	return -1;
}

size_t Scape::Shader::GetUniformSize(std::string uniformName)
{
	if (_activeUniforms.find(uniformName) != _activeUniforms.end())
		return _activeUniforms[uniformName]->GetBufferSize();
	return 0;
}

void Scape::Shader::RetreiveActiveUniforms()
{
    GLint count, size;
    GLenum type; 
    const GLsizei bufSize = 24; 
    GLchar name[bufSize];
    GLsizei length;

    std::set<std::string> newNames = std::set<std::string>();

    glGetProgramiv(_programId, GL_ACTIVE_UNIFORMS, &count);
    for (int i = 0; i < count; i++)
    {
        glGetActiveUniform(_programId, i, bufSize, &length, &size, &type, name);

        // Check if this uniform exists and is still the same
        if (_activeUniforms.find(name) != _activeUniforms.end() && _activeUniforms.at(name)->GetType() == type)
        {
            _activeUniforms.at(name)->UpdateLocation(); // In case new uniforms moved it
            SendUniform(name);
        }
        else // Create populate and grab new uniform value from its shader
        {
            _activeUniforms[name] = std::make_shared<Uniform>(name, type, _programId);
            RetreiveUniform(name);
        }
        
        newNames.insert(name);
    }

    // Remove old uniforms
    std::erase_if(_activeUniforms, [newNames](std::pair<std::string, std::shared_ptr<Uniform>> entry) {
        return !newNames.contains(entry.first);
    });
}