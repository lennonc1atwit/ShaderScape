#pragma once
#include "Layer.h"
#include "Shader.h"
#include "Logging.h"

#include <glad/gl.h>
#include <map>
#include <memory>
#include <imgui.h>
#include <fstream>
#include <filesystem>
#include <Quad.h>

namespace Scape {
    class ShaderLayer :  public Layer
    {
	public:
        ShaderLayer() : Layer("ShaderLayer") { OnAttach(); }
		~ShaderLayer() { OnDetach(); }

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(double time, double timeStep)  override;
		void OnFixedUpdate(double fixedTimeStep) override;
		void OnImGuiRender() override;
		void OnEvent() override;

        void OnShaderLink();

	private:
        void ShowBuilder();
        void ShowUniforms();

        class UniformUiData
        {
        public:
            UniformUiData(GLenum type)
            {
                Type = type;
                // Allocate for double as this is our largest datatype
                Min = calloc(1, sizeof(double));
                Max = calloc(1, sizeof(double));

                IsColor = false;
            }
            ~UniformUiData()
            {
                free(Min);
                free(Max);
            }

            GLenum Type;
            void* Min;
            void* Max;
            bool IsColor;
        };

        inline static std::map<ImGuiDataType, const char*> UNIFORM_FORMAT_MAP =
        {
            {Uniform::DataType::Float, "%.3f"},
            {Uniform::DataType::Double, "%.3f"},
            {Uniform::DataType::Signed32, "%d"},
            {Uniform::DataType::UnSigned32, "%u"}
        };

        std::map <std::string, std::shared_ptr<UniformUiData>> _uniformData;

        std::shared_ptr<Quad> _quad;

        std::string _selectedShaderFileName = "None";

        GLuint _curShader = 0;
    };
}

