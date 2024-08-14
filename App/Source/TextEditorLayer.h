#pragma once
#include <layer.h>

namespace Scape 
{
	class TextEditorLayer : public Layer
	{
	public:
		TextEditorLayer();
		~TextEditorLayer() { OnDetach(); }

		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnUpdate(double time, double timeStep);
		virtual void OnFixedUpdate(double fixedTimeStep);
		virtual void OnImGuiRender();
		virtual void OnEvent(std::shared_ptr<Event> event);
	private:

		struct TextEditorData
		{
			const char* FilePath;
			std::string FileContents;
			double FileLastSaved;
		};

		enum EditorTypes
		{

		};

	};
}