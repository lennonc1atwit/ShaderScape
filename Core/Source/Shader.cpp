#include "Shader.h"
#include <fstream>
#include <Logging.h>
#include <set>

GLuint Scape::Shader::CreateShader(const std::string& shaderSource, GLenum shaderType)
{
    // Open File
    //static const std::string _openGlVersion = "#version 430 core\n";
    //static const std::string _builtInUniforms = "precision highp float;\nuniform float iTime;\nuniform vec3 iResolution;\nuniform vec4 iMouse;\nuniform int iFrame;\nuniform float iTimeDelta;\n";
    //std::string fullSource = _openGlVersion + _builtInUniforms + shaderSource;
    const char* rawSource = shaderSource.c_str();

    // Create shader object
    GLuint shaderId = glCreateShader(shaderType);
    glad_glShaderSource(shaderId, 1, &rawSource, nullptr);
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

    _isEmpty = shaderCount == 0;
    if (_isEmpty) Logger::LogMessageConsole(Logger::Severity::Warn, __FILE__, __LINE__, "linking empty shader!\n");

    // Link all attached shaders
    glLinkProgram(_programId);

    // Display errors
    GLint status, length;
    glGetProgramiv(_programId, GL_LINK_STATUS, &status);
    if (!status)
    {
        // check if error
        glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &length);
        char* buffer = (char*)malloc(sizeof(char) * length);
        glGetProgramInfoLog(_programId, length, nullptr, buffer);
        fprintf(stderr, "%s\n", buffer);
        free(buffer);
        return;
    }
    
    for (auto callback : _linkCallbacks)
        callback();
}

void Scape::Shader::SendUniform(const char* name, GLenum type, void* buffer)
{
    GLint loc = GetUniformLocation(name);
    if (loc != GL_INVALID_INDEX)
        SendUniform(loc, type, buffer);
}

void Scape::Shader::SendUniform(GLuint location, GLenum type, void* buffer)
{
    switch (type)
    {
    case GL_FLOAT:              glProgramUniform1fv(_programId, location, 1, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_VEC2:         glProgramUniform2fv(_programId, location, 1, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_VEC3:         glProgramUniform3fv(_programId, location, 1, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_VEC4:         glProgramUniform4fv(_programId, location, 1, reinterpret_cast<float*>(buffer)); break;
    case GL_DOUBLE:             glProgramUniform1dv(_programId, location, 1, reinterpret_cast<double*>(buffer)); break;
    case GL_DOUBLE_VEC2:        glProgramUniform2dv(_programId, location, 1, reinterpret_cast<double*>(buffer)); break;
    case GL_DOUBLE_VEC3:        glProgramUniform3dv(_programId, location, 1, reinterpret_cast<double*>(buffer)); break;
    case GL_DOUBLE_VEC4:        glProgramUniform4dv(_programId, location, 1, reinterpret_cast<double*>(buffer)); break;
    case GL_INT:                glProgramUniform1iv(_programId, location, 1, reinterpret_cast<int*>(buffer)); break;
    case GL_INT_VEC2:           glProgramUniform2iv(_programId, location, 1, reinterpret_cast<int*>(buffer)); break;
    case GL_INT_VEC3:           glProgramUniform3iv(_programId, location, 1, reinterpret_cast<int*>(buffer)); break;
    case GL_INT_VEC4:           glProgramUniform4iv(_programId, location, 1, reinterpret_cast<int*>(buffer)); break;
    case GL_UNSIGNED_INT:      glProgramUniform1uiv(_programId, location, 1, reinterpret_cast<unsigned int*>(buffer)); break;
    case GL_UNSIGNED_INT_VEC2: glProgramUniform2uiv(_programId, location, 1, reinterpret_cast<unsigned int*>(buffer)); break;
    case GL_UNSIGNED_INT_VEC3: glProgramUniform3uiv(_programId, location, 1, reinterpret_cast<unsigned int*>(buffer)); break;
    case GL_UNSIGNED_INT_VEC4: glProgramUniform4uiv(_programId, location, 1, reinterpret_cast<unsigned int*>(buffer)); break;
    case GL_FLOAT_MAT2:        glProgramUniformMatrix2fv(_programId, location, 1, false, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT3:        glProgramUniformMatrix3fv(_programId, location, 1, false, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT4:        glProgramUniformMatrix4fv(_programId, location, 1, false, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT2x3:      glProgramUniformMatrix2x3fv(_programId, location, 1, false, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT3x2:      glProgramUniformMatrix3x2fv(_programId, location, 1, false, reinterpret_cast<float*>(buffer)); break;
    default: break;
    }
}

void Scape::Shader::RetreiveUniform(GLuint location, GLenum type, void* buffer)
{
    switch (type)
    {
    case GL_FLOAT:             glGetUniformfv(_programId, location, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_VEC2:        glGetUniformfv(_programId, location, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_VEC3:        glGetUniformfv(_programId, location, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_VEC4:        glGetUniformfv(_programId, location, reinterpret_cast<float*>(buffer)); break;
    case GL_DOUBLE:            glGetUniformdv(_programId, location, reinterpret_cast<double*>(buffer)); break;
    case GL_DOUBLE_VEC2:       glGetUniformdv(_programId, location, reinterpret_cast<double*>(buffer)); break;
    case GL_DOUBLE_VEC3:       glGetUniformdv(_programId, location, reinterpret_cast<double*>(buffer)); break;
    case GL_DOUBLE_VEC4:       glGetUniformdv(_programId, location, reinterpret_cast<double*>(buffer)); break;
    case GL_INT:               glGetUniformiv(_programId, location, reinterpret_cast<int*>(buffer)); break;
    case GL_INT_VEC2:          glGetUniformiv(_programId, location, reinterpret_cast<int*>(buffer)); break;
    case GL_INT_VEC3:          glGetUniformiv(_programId, location, reinterpret_cast<int*>(buffer)); break;
    case GL_INT_VEC4:          glGetUniformiv(_programId, location, reinterpret_cast<int*>(buffer)); break;
    case GL_UNSIGNED_INT:      glGetUniformuiv(_programId, location, reinterpret_cast<unsigned int*>(buffer)); break;
    case GL_UNSIGNED_INT_VEC2: glGetUniformuiv(_programId, location, reinterpret_cast<unsigned int*>(buffer)); break;
    case GL_UNSIGNED_INT_VEC3: glGetUniformuiv(_programId, location, reinterpret_cast<unsigned int*>(buffer)); break;
    case GL_UNSIGNED_INT_VEC4: glGetUniformuiv(_programId, location, reinterpret_cast<unsigned int*>(buffer)); break;
    case GL_FLOAT_MAT2:        glGetUniformfv(_programId, location, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT3:        glGetUniformfv(_programId, location, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT4:        glGetUniformfv(_programId, location, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT2x3:      glGetUniformfv(_programId, location, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT3x2:      glGetUniformfv(_programId, location, reinterpret_cast<float*>(buffer)); break;
    default: break;
    }
}

std::map<std::string, Scape::Shader::UniformInfo> Scape::Shader::GetActiveUniforms()
{
    GLint count, size;
    GLenum type; 
    const GLsizei bufSize = 24; 
    GLchar name[bufSize];
    GLsizei length;

    glGetProgramiv(_programId, GL_ACTIVE_UNIFORMS, &count);
    std::map<std::string, UniformInfo> activeUniforms = std::map<std::string, UniformInfo>();

    for (int i = 0; i < count; i++)
    {
        glGetActiveUniform(_programId, i, bufSize, &length, &size, &type, name);
        UniformInfo uniform = {
            .Type = type,
            .Location = glGetUniformLocation(_programId, name)
        };

        activeUniforms[std::string(name)] = (uniform);
    }

    return activeUniforms;
}
