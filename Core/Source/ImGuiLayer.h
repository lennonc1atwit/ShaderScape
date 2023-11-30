#pragma once
#include "Layer.h"
#include "Window.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <implot.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Scape 
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer(std::shared_ptr<Window> window) : _window(window), Layer("ImGuiLayer") { OnAttach(); }
		~ImGuiLayer() { OnDetach(); }

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent() override;

		void Begin();
		void End();

	private:
		std::shared_ptr<Window> _window;
	};
}