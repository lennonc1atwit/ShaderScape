#pragma once
#include "Layer.h"
#include <bit>
#include <imgui.h>
#include <implot.h>
#include <numeric>
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
		void OnEvent() override;

	private:
        static const int CAPTURE_WINDOW_SIZE = 240;
        int _nextOffset;
        
        double _averageFrameTime;
        double _curIndex;

        ImVec4 _avgColor;
        float _avgThickness;
        bool _showMarker;

        ImVec4 _lineColor;
        float _lineThickness;

        ImVec4 _fillColor;

        double* _timeData;
        double* _frameTimeData;

        inline static double yTickValues[7] = { 
            0, 
            1, 
            4.166666,  
            8.333333, 
            16.66666, 
            33.33333, 
            66.66667 
        };

        inline static const char* yTickLabels[7] = {
            " inffps   0ms",
            "1000fps   1ms",
            " 240fps   4ms",
            " 120fps   8ms",
            "  60fps  17ms",
            "  30fps  33ms",
            "  15fps  67ms"
        };
	};
}

