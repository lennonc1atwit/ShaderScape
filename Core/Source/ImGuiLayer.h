#pragma once
#include "Layer.h"
#include "Window.h"
#include <Profiler.h>

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
		virtual void OnEvent(std::shared_ptr<Event> event) override;

		void Begin();
		void End();

	private:
		std::shared_ptr<Window> _window;

		//inline static std::unordered_map<ImGuiCol_, ImColor> _styleColorMap =
		//{
		//	//ImGuiCol_Border,			ImColor(0, 18, 25)},
		//	//ImGuiCol_BorderShadow,		ImColor(0, 18, 25)},
		//	{ImGuiCol_Separator,		ImColor(233,216,166)},
		//	{ImGuiCol_SliderGrab,		ImColor(148,210,189)},
		//	{ImGuiCol_SliderGrabActive, ImColor(10,147,150)},
		//	{ImGuiCol_FrameBg,			ImColor(0,95,115)},
		//};

		/*$theme - colors: (
			"light":      #dce3ec,
			"dark":       #0a060e,
			"primary":    #224def,
			"secondary":  #4188f4,
			"info":       #55b3fc,
			"accent1":    #d06617,
			"accent2":    #fc8c2f,
			"accent3":    #b4bfc6,
			"success":    #10bc6f,
			"warning":    #e9d20d,
			"danger":     #fb1f05,
			);*/
	};
}