#pragma once
#include <layer.h>
#include <Shader.h>

#include <imgui.h>
#include <glad/gl.h>

#include <map>
#include <glm/ext/vector_float2.hpp>
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
			void* Buffer = nullptr;
			void* SliderMin = nullptr;
			void* SliderMax = nullptr;
			size_t BufferSize = 0;
			size_t SliderStride = 0;
			const char* TypeString = "";
			const char* DisplayFormat = "";

			char WidgetID[16];
			char MinWidgetId[16];
			char MaxWidgetId[16];
			char ColorWidgetId[16];
			
			GLenum Type = -1;
			GLuint Location = -1;
			DataType DataType = COUNT;
			glm::vec2 Dim = glm::vec2(0, 0);

			float TypeStringOffset = 0;
			ImGuiSliderFlags SliderFlags = 0;
	
			// TODO make flags int
			bool IsColor = false;
			bool CouldBeColor = false;
			bool LogScale = false;
		};

		struct GLenumTypeInfo
		{
			size_t Size;
			DataType ComponentDataType;
			glm::vec2 Dim;
			const char* Name;
		};

		inline static std::map<GLenum, GLenumTypeInfo> GL_TYPE_INFO_MAP
		{
			{GL_FLOAT,             GLenumTypeInfo{ sizeof(float) * 1, DataType::Float, glm::vec2(1,1), "float"}},
			{GL_FLOAT_VEC2,        GLenumTypeInfo{ sizeof(float) * 2, DataType::Float, glm::vec2(1,2), "vec2"}},
			{GL_FLOAT_VEC3,        GLenumTypeInfo{ sizeof(float) * 3, DataType::Float, glm::vec2(1,3), "vec3"}},
			{GL_FLOAT_VEC4,        GLenumTypeInfo{ sizeof(float) * 4, DataType::Float, glm::vec2(1,4), "vec4"}},
			{GL_DOUBLE,            GLenumTypeInfo{ sizeof(double) * 1, DataType::Double, glm::vec2(1,1), "double"}},
			{GL_DOUBLE_VEC2,       GLenumTypeInfo{ sizeof(double) * 2, DataType::Double, glm::vec2(1,2), "dvec2"}},
			{GL_DOUBLE_VEC3,       GLenumTypeInfo{ sizeof(double) * 3, DataType::Double, glm::vec2(1,3), "dvec3"}},
			{GL_DOUBLE_VEC4,       GLenumTypeInfo{ sizeof(double) * 4, DataType::Double, glm::vec2(1,4), "dvec4"}},
			{GL_INT,               GLenumTypeInfo{ sizeof(int) * 1, DataType::Signed32, glm::vec2(1,1), "int"}},
			{GL_INT_VEC2,          GLenumTypeInfo{ sizeof(int) * 2, DataType::Signed32, glm::vec2(1,2), "ivec2"}},
			{GL_INT_VEC3,          GLenumTypeInfo{ sizeof(int) * 3, DataType::Signed32, glm::vec2(1,3), "ivec3"}},
			{GL_INT_VEC4,          GLenumTypeInfo{ sizeof(int) * 4, DataType::Signed32, glm::vec2(1,4), "ivec4"}},
			{GL_UNSIGNED_INT,      GLenumTypeInfo{ sizeof(unsigned int) * 1, DataType::UnSigned32, glm::vec2(1,1), "uint"}},
			{GL_UNSIGNED_INT_VEC2, GLenumTypeInfo{ sizeof(unsigned int) * 2, DataType::UnSigned32, glm::vec2(1,2), "uvec2"}},
			{GL_UNSIGNED_INT_VEC3, GLenumTypeInfo{ sizeof(unsigned int) * 3, DataType::UnSigned32, glm::vec2(1,3), "uvec3"}},
			{GL_UNSIGNED_INT_VEC4, GLenumTypeInfo{ sizeof(unsigned int) * 4, DataType::UnSigned32, glm::vec2(1,4), "uvec4"}},
			{GL_FLOAT_MAT2,        GLenumTypeInfo{ sizeof(float) * 2 * 2, DataType::Float, glm::vec2(2,2), "mat2x2"}},
			{GL_FLOAT_MAT2x3,      GLenumTypeInfo{ sizeof(float) * 2 * 3, DataType::Float, glm::vec2(2,3), "mat2x3"}},
			{GL_FLOAT_MAT2x4,      GLenumTypeInfo{ sizeof(float) * 2 * 4, DataType::Float, glm::vec2(2,4), "mat2x4"}},
			{GL_FLOAT_MAT3x2,      GLenumTypeInfo{ sizeof(float) * 3 * 2, DataType::Float, glm::vec2(3,2), "mat3x2"}},
			{GL_FLOAT_MAT3,        GLenumTypeInfo{ sizeof(float) * 3 * 3, DataType::Float, glm::vec2(3,3), "mat3x3"}},
			{GL_FLOAT_MAT3x4,      GLenumTypeInfo{ sizeof(float) * 3 * 4, DataType::Float, glm::vec2(3,4), "mat3x4"}},
			{GL_FLOAT_MAT4x2,      GLenumTypeInfo{ sizeof(float) * 4 * 2, DataType::Float, glm::vec2(4,2), "mat4x2"}},
			{GL_FLOAT_MAT4x3,      GLenumTypeInfo{ sizeof(float) * 4 * 3, DataType::Float, glm::vec2(4,3), "mat4x3"}},
			{GL_FLOAT_MAT4,        GLenumTypeInfo{ sizeof(float) * 4 * 4, DataType::Float, glm::vec2(4,4), "mat4x4"}}
		};

		std::map<std::string, GLenum> _builtInUniforms= {
			{"iTime",		GL_FLOAT},
			{"iTimeDelta",	GL_FLOAT},
			{"iMouse",		GL_FLOAT_VEC4},
			{"iResolution",	GL_FLOAT_VEC3},
			{"iFrame",		GL_INT},
		};

		inline static std::map<ImGuiDataType, const char*> UNIFORM_FORMAT_MAP =
		{
			{DataType::Float,   "%.6f"},
			{DataType::Double,  "%.6f"},
			{DataType::Signed32,   "%d"},
			{DataType::UnSigned32, "%u"}
		};

		std::map<std::string, std::shared_ptr<UniformUIData>> _uniformData;
		std::shared_ptr<Shader> _shaderProgram;

		void ShowUniformConfig(const char* name, std::shared_ptr<UniformUIData> data, float lineWidth);
		void OnShaderProgramLink();
		void UpdateUniformLocation(std::string name);
		void AddNewUniform(const char* name, Shader::UniformInfo info);

		bool IsBuiltInUniform(std::string name, GLenum type);
	};
}