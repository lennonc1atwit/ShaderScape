#pragma once
#include <layer.h>
#include <Shader.h>

#include <imgui.h>
#include <glad/gl.h>

#include <map>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>

namespace Scape
{
	class ShaderUniformLayer : public Layer
	{
	public:
		ShaderUniformLayer(std::shared_ptr<Shader> shader);
		~ShaderUniformLayer() { OnDetach(); }

		void OnAttach() override;
		void OnDetach() override {}
		void OnUpdate(double time, double timeStep)  override {}
		void OnFixedUpdate(double fixedTimeStep) override {}
		void OnImGuiRender() override;
		void OnEvent(std::shared_ptr<Event> event) override {}
	private:
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

		struct UniformUIData
		{
			// Shader Related Info
			GLenum Type = -1;
			GLuint Location = -1;
			const char* TypeString = "";
			float TypeStringOffset = 0;

			// Data
			void* Buffer = nullptr;
			size_t BufferSize = 0;
			DataType DataType = COUNT;

			void* SliderMin = nullptr;
			void* SliderMax = nullptr;

			// Gui Vars
			glm::vec2 Dim = glm::vec2(0,0);
			bool IsColor = false;
			bool LogScale = false;
			ImGuiSliderFlags SliderFlags = 0;

			std::string WidgetID = "";
			std::string MinWidgetId = "";
			std::string MaxWidgetId = "";
			std::string ColorWidgetId = "";
		};

		inline static std::map<ImGuiDataType, const char*> UNIFORM_FORMAT_MAP =
		{
			{DataType::Float,   "%.10f"},
			{DataType::Double,  "%.10f"},
			{DataType::Signed32,   "%d"},
			{DataType::UnSigned32, "%u"}
		};

		struct GLDataTypeInfo
		{
			DataType DataType;
			glm::vec2 Components;
			const char* Name;
		};

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

		inline static std::map<GLenum, GLDataTypeInfo> UNIFORM_INFO_MAP =
		{
			{GL_FLOAT,             GLDataTypeInfo{ DataType::Float,      glm::vec2(1,1), "float"}},
			{GL_FLOAT_VEC2,        GLDataTypeInfo{ DataType::Float,      glm::vec2(1,2), "vec2"}},
			{GL_FLOAT_VEC3,        GLDataTypeInfo{ DataType::Float,      glm::vec2(1,3), "vec3"}},
			{GL_FLOAT_VEC4,        GLDataTypeInfo{ DataType::Float,      glm::vec2(1,4), "vec4"}},
			{GL_DOUBLE,            GLDataTypeInfo{ DataType::Double,     glm::vec2(1,1), "double"}},
			{GL_DOUBLE_VEC2,       GLDataTypeInfo{ DataType::Double,     glm::vec2(1,2), "dvec2"}},
			{GL_DOUBLE_VEC3,       GLDataTypeInfo{ DataType::Double,     glm::vec2(1,3), "dvec3"}},
			{GL_DOUBLE_VEC4,       GLDataTypeInfo{ DataType::Double,     glm::vec2(1,4), "dvec4"}},
			{GL_INT,               GLDataTypeInfo{ DataType::Signed32,   glm::vec2(1,1), "int"}},
			{GL_INT_VEC2,          GLDataTypeInfo{ DataType::Signed32,   glm::vec2(1,2), "ivec2"}},
			{GL_INT_VEC3,          GLDataTypeInfo{ DataType::Signed32,   glm::vec2(1,3), "ivec3"}},
			{GL_INT_VEC4,          GLDataTypeInfo{ DataType::Signed32,   glm::vec2(1,4), "ivec4"}},
			{GL_UNSIGNED_INT,      GLDataTypeInfo{ DataType::UnSigned32, glm::vec2(1,1), "uint"}},
			{GL_UNSIGNED_INT_VEC2, GLDataTypeInfo{ DataType::UnSigned32, glm::vec2(1,2), "uvec2"}},
			{GL_UNSIGNED_INT_VEC3, GLDataTypeInfo{ DataType::UnSigned32, glm::vec2(1,3), "uvec3"}},
			{GL_UNSIGNED_INT_VEC4, GLDataTypeInfo{ DataType::UnSigned32, glm::vec2(1,4), "uvec4"}},
			{GL_FLOAT_MAT2,        GLDataTypeInfo{ DataType::Float,      glm::vec2(2,2), "mat2x2"}},
			{GL_FLOAT_MAT2x3,      GLDataTypeInfo{ DataType::Float,      glm::vec2(2,3), "mat2x3"}},
			{GL_FLOAT_MAT2x4,      GLDataTypeInfo{ DataType::Float,      glm::vec2(2,4), "mat2x4"}},
			{GL_FLOAT_MAT3x2,      GLDataTypeInfo{ DataType::Float,      glm::vec2(3,2), "mat3x2"}},
			{GL_FLOAT_MAT3,        GLDataTypeInfo{ DataType::Float,      glm::vec2(3,3), "mat3x3"}},
			{GL_FLOAT_MAT3x4,      GLDataTypeInfo{ DataType::Float,      glm::vec2(3,4), "mat3x4"}},
			{GL_FLOAT_MAT4x2,      GLDataTypeInfo{ DataType::Float,      glm::vec2(4,2), "mat4x2"}},
			{GL_FLOAT_MAT4x3,      GLDataTypeInfo{ DataType::Float,      glm::vec2(4,3), "mat4x3"}},
			{GL_FLOAT_MAT4,        GLDataTypeInfo{ DataType::Float,      glm::vec2(4,4), "mat4x4"}},
			{GL_BOOL,              GLDataTypeInfo{ DataType::Signed32,   glm::vec2(1,1), "bool"}},
			{GL_BOOL_VEC2,         GLDataTypeInfo{ DataType::Signed32,   glm::vec2(1,1), "bvec2"}},
			{GL_BOOL_VEC3,         GLDataTypeInfo{ DataType::Signed32,   glm::vec2(1,1), "bvec3"}},
			{GL_BOOL_VEC4,         GLDataTypeInfo{ DataType::Signed32,   glm::vec2(1,1), "bvec4"}},
		};

		std::map<std::string, GLenum> _builtInUniforms= {
			{"iTime",		GL_FLOAT},
			{"iTimeDelta",	GL_FLOAT},
			{"iMouse",		GL_FLOAT_VEC4},
			{"iResolution",	GL_FLOAT_VEC3},
			{"iFrame",		GL_INT},
		};

		std::map<std::string, std::shared_ptr<UniformUIData>> _uniformData;
		std::shared_ptr<Shader> _shaderProgram;

		void ShowUniformConfig(const char* name, std::shared_ptr<UniformUIData> data, float lineWidth);
		void OnShaderProgramLink();
		void UpdateUniformLocation(std::string name);
		void AddNewUniform(std::string name, Shader::UniformInfo info);

		bool IsBuiltInUniform(std::string name, GLenum type);
	};
}