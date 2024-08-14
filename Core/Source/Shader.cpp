#include "Shader.h"
#include <fstream>
#include <Logging.h>
#include <set>

GLuint Scape::Shader::CreateShader(const char* shaderSource, GLenum shaderType)
{
    // Create shader object
    GLuint shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shaderSource, nullptr);
    glCompileShader(shaderId);

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
    GLint status = 0;
    glGetProgramiv(_programId, GL_LINK_STATUS, &status);
    if (!status)
    {
        // check if error
        GLint length = 0;
        glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) 
        {
            char* buffer = (char*)malloc(sizeof(char) * length);
            glGetProgramInfoLog(_programId, length, nullptr, buffer);
            Logger::LogMessageConsole(Logger::Severity::Fail, __FILE__, __LINE__, buffer);
            free(buffer);
            return;
        }
    }
    
    for (auto callback : _linkCallbacks)
        callback();
}

const std::string Scape::Shader::GetShaderError(GLuint shaderId)
{
    // Display errors
    GLint _status = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &_status);
    if (_status) return "";

    GLint length = 0;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
    if (length == 0) return "";

    char* buffer = (char*)malloc(sizeof(char) * length);
    glGetShaderInfoLog(shaderId, length, nullptr, buffer);
    std::string test(buffer);
    free(buffer);

    Logger::LogMessageConsole(Logger::Severity::Fail, __FILE__, __LINE__, test);   
    return test;
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
    case GL_FLOAT:             glProgramUniform1fv(_programId, location, 1, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_VEC2:        glProgramUniform2fv(_programId, location, 1, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_VEC3:        glProgramUniform3fv(_programId, location, 1, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_VEC4:        glProgramUniform4fv(_programId, location, 1, reinterpret_cast<float*>(buffer)); break;
    case GL_DOUBLE:            glProgramUniform1dv(_programId, location, 1, reinterpret_cast<double*>(buffer)); break;
    case GL_DOUBLE_VEC2:       glProgramUniform2dv(_programId, location, 1, reinterpret_cast<double*>(buffer)); break;
    case GL_DOUBLE_VEC3:       glProgramUniform3dv(_programId, location, 1, reinterpret_cast<double*>(buffer)); break;
    case GL_DOUBLE_VEC4:       glProgramUniform4dv(_programId, location, 1, reinterpret_cast<double*>(buffer)); break;
    case GL_INT:               glProgramUniform1iv(_programId, location, 1, reinterpret_cast<int*>(buffer)); break;
    case GL_INT_VEC2:          glProgramUniform2iv(_programId, location, 1, reinterpret_cast<int*>(buffer)); break;
    case GL_INT_VEC3:          glProgramUniform3iv(_programId, location, 1, reinterpret_cast<int*>(buffer)); break;
    case GL_INT_VEC4:          glProgramUniform4iv(_programId, location, 1, reinterpret_cast<int*>(buffer)); break;
    case GL_UNSIGNED_INT:      glProgramUniform1uiv(_programId, location, 1, reinterpret_cast<unsigned int*>(buffer)); break;
    case GL_UNSIGNED_INT_VEC2: glProgramUniform2uiv(_programId, location, 1, reinterpret_cast<unsigned int*>(buffer)); break;
    case GL_UNSIGNED_INT_VEC3: glProgramUniform3uiv(_programId, location, 1, reinterpret_cast<unsigned int*>(buffer)); break;
    case GL_UNSIGNED_INT_VEC4: glProgramUniform4uiv(_programId, location, 1, reinterpret_cast<unsigned int*>(buffer)); break;
    case GL_FLOAT_MAT2:        glProgramUniformMatrix2fv(_programId, location, 1, false, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT2x3:      glProgramUniformMatrix2x3fv(_programId, location, 1, false, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT2x4:      glProgramUniformMatrix2x4fv(_programId, location, 1, false, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT3x2:      glProgramUniformMatrix3x2fv(_programId, location, 1, false, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT3:        glProgramUniformMatrix3fv(_programId, location, 1, false, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT3x4:      glProgramUniformMatrix3x4fv(_programId, location, 1, false, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT4x2:      glProgramUniformMatrix4x2fv(_programId, location, 1, false, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT4x3:      glProgramUniformMatrix4x3fv(_programId, location, 1, false, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT4:        glProgramUniformMatrix4fv(_programId, location, 1, false, reinterpret_cast<float*>(buffer)); break;
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
    case GL_FLOAT_MAT2x3:      glGetUniformfv(_programId, location, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT2x4:      glGetUniformfv(_programId, location, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT3x2:      glGetUniformfv(_programId, location, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT3:        glGetUniformfv(_programId, location, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT3x4:      glGetUniformfv(_programId, location, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT4x2:      glGetUniformfv(_programId, location, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT4x3:      glGetUniformfv(_programId, location, reinterpret_cast<float*>(buffer)); break;
    case GL_FLOAT_MAT4:        glGetUniformfv(_programId, location, reinterpret_cast<float*>(buffer)); break;
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