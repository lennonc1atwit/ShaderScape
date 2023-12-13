#pragma once
#include <Shader.h>
#include <Layer.h>
#include <imgui.h>
#include <Logging.h>
#include <GlslTokenizer.h>

#include <filesystem>

#include <fstream>

namespace Scape
{
	class ShaderBuilderLayer : public Layer
	{
	public:
		ShaderBuilderLayer(std::shared_ptr<Shader> shader);
		~ShaderBuilderLayer() { OnDetach(); }

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(double time, double timeStep)  override;
		void OnFixedUpdate(double fixedTimeStep) override;
		void OnImGuiRender() override;
		void OnEvent(std::shared_ptr<Event> event) override;
	private:
		struct ShaderFileData
		{
			GLuint ShaderId;
			GLenum ShaderType;
			std::string FilePath;
			std::string FileName;
			double FileTimeStamp;

		};
		void ShowFileDropDown(std::string title, std::string& selectedItem, std::vector<std::string> options, GLenum shaderType);
		bool TryLoadShaderFromFile(const std::string& source, GLenum type);
		void CheckForFileUpdates(std::shared_ptr<ShaderFileData>);
		void RefreshFileCache();
		void ShowFileErrorPopup();
		
		std::shared_ptr<Shader> _shaderProgram;
		std::map<GLenum, std::shared_ptr<ShaderFileData>> _attachedShaderFiles;

		std::vector<std::string> _availableFragmentShaders;
		std::vector<std::string> _availableComputeShaders;

		inline static const char* SHADER_DIR = "../Shaders/";
		inline static std::string BUILTIN_SHADER = "BuiltIns.glsl";
		inline static std::string BUILTIN_SOURCE = "";
		inline static const std::set<std::string> FRAGMENT_EXTENSIONS = { ".f", ".frag" , ".fragment" };
		inline static const std::set<std::string> COMPUTE_EXTENSIONS = { ".comp", ".compute" };

		std::string _errorMsg = "";
		bool _showError = false;
	};
}