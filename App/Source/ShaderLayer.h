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
#include <Event.h>

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
		void OnEvent(std::shared_ptr<Event> event) override;

        void OnShaderLink();
	private:
        class UniformUiData
        {
        public:
            UniformUiData(GLenum type, const char* typeString, std::string name)
            {
                Type = type;
                WidgetID = "##" + name;

                sprintf_s(TypeString, "(%s)", typeString);
                TypeOffset = ImGui::CalcTextSize(TypeString).x + 16;

                MinWidgetId = "Min" + WidgetID;
                Min = calloc(1, sizeof(double));

                MaxWidgetId = "Max" + WidgetID;
                Max = calloc(1, sizeof(double));

                ColorWidgetId = "Color" + WidgetID;
                IsColor = false;
            }
            ~UniformUiData()
            {
                free(Min);
                free(Max);
            }

            GLenum Type;
            char TypeString[16];
            float TypeOffset;

            void* Min;
            void* Max;
            bool IsColor;

            std::string WidgetID;
            std::string MinWidgetId;
            std::string MaxWidgetId;
            std::string ColorWidgetId;
        };

        inline static std::map<ImGuiDataType, const char*> UNIFORM_FORMAT_MAP =
        {
            {Uniform::DataType::Float, "%.3f"},
            {Uniform::DataType::Double, "%.3f"},
            {Uniform::DataType::Signed32, "%d"},
            {Uniform::DataType::UnSigned32, "%u"}
        };

        void ShowBuilder();
        void ShowUniforms();
        void ShowUniformConfig(const char* name, std::shared_ptr<Uniform> uniform, std::shared_ptr<UniformUiData> data, ImVec2 windowSize);
        void ShowOutput();
        void ShowFileErrorPopup();

        bool IsShaderLoaded() { return _selectedShaderFileName.compare("None") != 0; }
        bool LoadShaderFromPath(std::string filePath);
        void UnLoadShader();

        double GetLastWriteTime();

        std::map <std::string, std::shared_ptr<UniformUiData>> _uniformData;

        std::shared_ptr<Quad> _quad;
        std::shared_ptr<Scape::Shader> _shader;
        GLuint _curShader = 0;

        std::string _selectedShaderFileName = "None";
        std::string _selectedShaderFilePath = "";

        std::string _errorMsg = "";
        bool _showError = false;

        double _lastSaveTime;

        int _iFrame = 0;
    };
}

