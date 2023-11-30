#pragma once
#include <string>

namespace Scape
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer") : m_DebugName(name) {}
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(double time, double timeStep) {}
		virtual void OnFixedUpdate(double fixedTimeStep) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent() {}

		const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}