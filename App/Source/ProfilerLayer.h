#pragma once
#include "Layer.h"
#include <bit>
#include <imgui.h>
#include <implot.h>
#include <numeric>
#include <Event.h>
#include <Profiler.h>
namespace Scape {
	class ProfilerLayer : public Layer
	{
	public:
        ProfilerLayer(): Layer("ProfilerLayer") { OnAttach(); }
        ~ProfilerLayer() { OnDetach(); }

		void OnAttach() override;
		void OnDetach() override;
        void OnUpdate(double time, double timeStep)  override;
		void OnFixedUpdate(double fixedTimeStep) override;
		void OnImGuiRender() override;
		void OnEvent(std::shared_ptr<Event> event) override;

	private:
        struct LineData
        {
            ImVec4 Color;
            float Thickness;
            bool ShowAverage;
            std::string ColorLabel;
            std::string ThicknessLabel;
            std::string AverageLabel;
        };

        Profiler* _profiler;
        std::map<std::string, std::shared_ptr<LineData>> _plotData;

        ImColor GetNextColor() { return _lineColors[_plotData.size() % IM_ARRAYSIZE(_lineColors)]; }

        inline static double _yTickValues[9] = { 
            0, 
            1, 
            2,
            4,  
            8, 
            16, 
            32, 
            64, 
            128
        };

        inline static const char* _yTickLabels[9] = {
            " inffps   0ms",
            "1000fps   1ms",
            " 500fps   2ms",
            " 250fps   4ms",
            " 125fps   8ms",
            "  62fps  16ms",
            "  31fps  32ms",
            "  15fps  64ms",
            "   8fps 128ms"
        };

        inline static ImColor _lineColors[] = {
            ImColor(255, 255, 255),
            ImColor(255, 202, 58),
            ImColor(138, 201, 38),
            ImColor(25, 130, 196),
            ImColor(106, 76, 147),
            ImColor(255, 89, 94),
        };
	};
}

