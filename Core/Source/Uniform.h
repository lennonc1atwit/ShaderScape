#pragma once
#include <glad/gl.h>
#include <string>
#include <map>

#include <glm/ext/vector_float2.hpp>

class Uniform
{
public:
	Uniform(std::string name, GLenum type, GLuint program) : _name(name), _type(type), _programId(program)
	{
		_location = glGetUniformLocation(_programId, _name.c_str());
		_buffer = malloc(GetBufferSize());
	}
	~Uniform()
	{
		free(_buffer);
	}

	enum DataType
	{
		Signed8,    // signed char / char
		UnSigned8,  // unsigned char
		Signed16,   // short
		UnSigned16, // unsigned short
		Signed32,   // int
		UnSigned32, // unsigned int
		Signed64,   // long long / __int64
		UnSigned64, // unsigned long long / unsigned __int64
		Float,      // float
		Double,     // double
		COUNT
	};

	const std::string& GetName() { return _name; }
	GLenum GetType() { return _type; }

	GLint GetLocation() { return _location; }
	GLint UpdateLocation() { _location = glGetUniformLocation(_programId, _name.c_str()); return _location; }

	template<typename T>
	T* GetBuffer() { return reinterpret_cast<T*>(_buffer); }
	size_t GetBufferSize() { return TYPE_SIZE_MAP[_type]; }

	Uniform::DataType GetDataType() { return UNIFORM_INFO_MAP[_type].DataType; }
	glm::vec2 GetComponentCount() { return UNIFORM_INFO_MAP[_type].Components; }
	const char* GetUniformTypeName() { return UNIFORM_INFO_MAP[_type].Name; }

private:
	std::string _name;
	GLenum _type;
	GLuint _programId;
	GLint _location;

	void* _buffer;

	inline static std::map<GLenum, size_t> TYPE_SIZE_MAP = {
		{GL_FLOAT,      sizeof(float) * 1},
		{GL_FLOAT_VEC2, sizeof(float) * 2},
		{GL_FLOAT_VEC3, sizeof(float) * 3},
		{GL_FLOAT_VEC4, sizeof(float) * 4},
		{GL_DOUBLE,      sizeof(double) * 1},
		{GL_DOUBLE_VEC2, sizeof(double) * 2},
		{GL_DOUBLE_VEC3, sizeof(double) * 3},
		{GL_DOUBLE_VEC4, sizeof(double) * 4},
		{GL_INT,      sizeof(int) * 1},
		{GL_INT_VEC2, sizeof(int) * 2},
		{GL_INT_VEC3, sizeof(int) * 3},
		{GL_INT_VEC4, sizeof(int) * 4},
		{GL_UNSIGNED_INT,      sizeof(unsigned int) * 1},
		{GL_UNSIGNED_INT_VEC2, sizeof(unsigned int) * 2},
		{GL_UNSIGNED_INT_VEC3, sizeof(unsigned int) * 3},
		{GL_UNSIGNED_INT_VEC4, sizeof(unsigned int) * 4},
		{GL_FLOAT_MAT2,   sizeof(float) * 2 * 2},
		{GL_FLOAT_MAT2x3, sizeof(float) * 2 * 3},
		{GL_FLOAT_MAT2x4, sizeof(float) * 2 * 4},
		{GL_FLOAT_MAT3x2, sizeof(float) * 3 * 2},
		{GL_FLOAT_MAT3,   sizeof(float) * 3 * 3},
		{GL_FLOAT_MAT3x4, sizeof(float) * 3 * 4},
		{GL_FLOAT_MAT4x2, sizeof(float) * 4 * 2},
		{GL_FLOAT_MAT4x3, sizeof(float) * 4 * 3},
		{GL_FLOAT_MAT4,   sizeof(float) * 4 * 4},
	};

	struct GLDataTypeInfo
	{
		Uniform::DataType DataType;
		glm::vec2 Components;
		const char* Name;
	};
	inline static std::map<GLenum, GLDataTypeInfo> UNIFORM_INFO_MAP =
	{
		{GL_FLOAT,             GLDataTypeInfo{ Uniform::DataType::Float,      glm::vec2(1,1), "float"}},
		{GL_FLOAT_VEC2,        GLDataTypeInfo{ Uniform::DataType::Float,      glm::vec2(1,2), "vec2"}},
		{GL_FLOAT_VEC3,        GLDataTypeInfo{ Uniform::DataType::Float,      glm::vec2(1,3), "vec3"}},
		{GL_FLOAT_VEC4,        GLDataTypeInfo{ Uniform::DataType::Float,      glm::vec2(1,4), "vec4"}},
		{GL_DOUBLE,            GLDataTypeInfo{ Uniform::DataType::Double,     glm::vec2(1,1), "double"}},
		{GL_DOUBLE_VEC2,       GLDataTypeInfo{ Uniform::DataType::Double,     glm::vec2(1,2), "dvec2"}},
		{GL_DOUBLE_VEC3,       GLDataTypeInfo{ Uniform::DataType::Double,     glm::vec2(1,3), "dvec3"}},
		{GL_DOUBLE_VEC4,       GLDataTypeInfo{ Uniform::DataType::Double,     glm::vec2(1,4), "dvec4"}},
		{GL_INT,               GLDataTypeInfo{ Uniform::DataType::Signed32,   glm::vec2(1,1), "int"}},
		{GL_INT_VEC2,          GLDataTypeInfo{ Uniform::DataType::Signed32,   glm::vec2(1,2), "ivec2"}},
		{GL_INT_VEC3,          GLDataTypeInfo{ Uniform::DataType::Signed32,   glm::vec2(1,3), "ivec3"}},
		{GL_INT_VEC4,          GLDataTypeInfo{ Uniform::DataType::Signed32,   glm::vec2(1,4), "ivec4"}},
		{GL_UNSIGNED_INT,      GLDataTypeInfo{ Uniform::DataType::UnSigned32, glm::vec2(1,1), "uint"}},
		{GL_UNSIGNED_INT_VEC2, GLDataTypeInfo{ Uniform::DataType::UnSigned32, glm::vec2(1,2), "uvec2"}},
		{GL_UNSIGNED_INT_VEC3, GLDataTypeInfo{ Uniform::DataType::UnSigned32, glm::vec2(1,3), "uvec3"}},
		{GL_UNSIGNED_INT_VEC4, GLDataTypeInfo{ Uniform::DataType::UnSigned32, glm::vec2(1,4), "uvec4"}},
		{GL_FLOAT_MAT2,        GLDataTypeInfo{ Uniform::DataType::Float,      glm::vec2(2,2), "mat2x2"}},
		{GL_FLOAT_MAT2x3,      GLDataTypeInfo{ Uniform::DataType::Float,      glm::vec2(2,3), "mat2x3"}},
		{GL_FLOAT_MAT2x4,      GLDataTypeInfo{ Uniform::DataType::Float,      glm::vec2(2,4), "mat2x4"}},
		{GL_FLOAT_MAT3x2,      GLDataTypeInfo{ Uniform::DataType::Float,      glm::vec2(3,2), "mat3x2"}},
		{GL_FLOAT_MAT3,        GLDataTypeInfo{ Uniform::DataType::Float,      glm::vec2(3,3), "mat3x3"}},
		{GL_FLOAT_MAT3x4,      GLDataTypeInfo{ Uniform::DataType::Float,      glm::vec2(3,4), "mat3x4"}},
		{GL_FLOAT_MAT4x2,      GLDataTypeInfo{ Uniform::DataType::Float,      glm::vec2(4,2), "mat4x2"}},
		{GL_FLOAT_MAT4x3,      GLDataTypeInfo{ Uniform::DataType::Float,      glm::vec2(4,3), "mat4x3"}},
		{GL_FLOAT_MAT4,        GLDataTypeInfo{ Uniform::DataType::Float,      glm::vec2(4,4), "mat4x4"}},
		{GL_BOOL,              GLDataTypeInfo{ Uniform::DataType::Signed32,   glm::vec2(1,1), "bool"}},
		{GL_BOOL_VEC2,         GLDataTypeInfo{ Uniform::DataType::Signed32,   glm::vec2(1,1), "bvec2"}},
		{GL_BOOL_VEC3,         GLDataTypeInfo{ Uniform::DataType::Signed32,   glm::vec2(1,1), "bvec3"}},
		{GL_BOOL_VEC4,         GLDataTypeInfo{ Uniform::DataType::Signed32,   glm::vec2(1,1), "bvec4"}},
	};
};