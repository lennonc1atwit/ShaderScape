#pragma once
#include <Layer.h>
#include <Quad.h>
#include <Shader.h>

#include <imgui.h>

#include <memory>


namespace Scape
{
	class ShaderDisplayLayer : public Layer
	{
	public:
		ShaderDisplayLayer(std::shared_ptr<Shader> shader);
		~ShaderDisplayLayer() { OnDetach(); }

		void OnAttach() override;
		void OnUpdate(double time, double timeStep)  override;
		void OnImGuiRender() override;
	private:

		void OnShaderLink();

		size_t _iFrame;
		float _iTime;

		std::shared_ptr<Shader> _shaderProgram;
		std::unique_ptr<Quad> _quad;
	};
};