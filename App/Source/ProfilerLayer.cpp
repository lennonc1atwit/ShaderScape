#include "ProfilerLayer.h"


namespace Scape {
	void ProfilerLayer::OnAttach()
	{
        _averageFrameTime = 0;
		_nextOffset = 0;
        _curIndex = 0;

        _avgThickness = 1;
        _avgColor = {0.0, 1.0, 0.5, 1.0};
        
        _lineThickness = 1;
        _lineColor = { 1.0, 0.5, 0.0, 1.0 };

        _fillColor = { 0.0, 0.5, 1.0, 0.125 };

		// Create data buffers
		_timeData = (double*)calloc(CAPTURE_WINDOW_SIZE, sizeof(double));
        _frameTimeData = (double*)calloc(CAPTURE_WINDOW_SIZE, sizeof(double));
	}
	void ProfilerLayer::OnDetach()
	{
        free(_timeData);
        free(_frameTimeData);
	}

	void ProfilerLayer::OnUpdate(double time, double timeStep)
	{
		_timeData[_nextOffset] = _curIndex++; // Im not using time here so dx is constant
		_frameTimeData[_nextOffset] = timeStep * 1000.0;

		_nextOffset = (_nextOffset + 1) % CAPTURE_WINDOW_SIZE;
	}

	void ProfilerLayer::OnFixedUpdate(double fixedTimeStep)
	{
        _averageFrameTime = std::accumulate( &_frameTimeData[0], &_frameTimeData[CAPTURE_WINDOW_SIZE-1] , 0.0) / (double)CAPTURE_WINDOW_SIZE;
	}

	void ProfilerLayer::OnImGuiRender()
	{
        if(ImGui::Begin("Profiler"))
        {
            ImVec2 contentSize = ImGui::GetContentRegionAvail();
            if (ImPlot::BeginPlot("##FrameTimes",contentSize))
            {
                ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_NoTickLabels);
                ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_SymLog);
                ImPlot::SetupAxisTicks(ImAxis_Y1, yTickValues, 7, yTickLabels, false);

                ImPlot::SetupAxisLimits(ImAxis_X1, _timeData[_nextOffset], _timeData[(_nextOffset - 1) % CAPTURE_WINDOW_SIZE], ImPlotCond_Always);

                ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, 200);
                ImPlot::SetupAxisZoomConstraints(ImAxis_Y1, 5, 200);

                ImPlot::SetNextLineStyle(_avgColor, _avgThickness);
                ImPlot::PlotInfLines("Avg", &_averageFrameTime, 1, ImPlotInfLinesFlags_Horizontal);
                if(_showMarker)
                    ImPlot::TagY(_averageFrameTime, _avgColor, "%.1fms", _averageFrameTime);

                ImPlot::SetNextLineStyle(_lineColor, _lineThickness);
                ImPlot::PlotLine("Line", _timeData, _frameTimeData, CAPTURE_WINDOW_SIZE, 0, _nextOffset);

                ImPlot::SetNextFillStyle(_fillColor);
                ImPlot::PlotShaded("Fill", _timeData, _frameTimeData, CAPTURE_WINDOW_SIZE, 0, 0, _nextOffset);

                if (ImPlot::BeginLegendPopup("Avg")) {
                    ImGui::SliderFloat("Avg Thickness", &_avgThickness, 0, 10);
                    ImGui::ColorEdit4("Avg Color", &_avgColor.x);
                    ImGui::Checkbox("Show Marker", &_showMarker);
                    ImPlot::EndLegendPopup();
                }

                if (ImPlot::BeginLegendPopup("Line")) {
                    ImGui::SliderFloat("Line Thickness", &_lineThickness, 0, 10);
                    ImGui::ColorEdit4("Line Color", &_lineColor.x);
                    ImPlot::EndLegendPopup();
                }

                if (ImPlot::BeginLegendPopup("Fill")) {
                    ImGui::ColorEdit4("Fill Color", &_fillColor.x);
                    ImPlot::EndLegendPopup();
                }
            
                ImPlot::EndPlot();
            }
        }
        ImGui::End();
	}

	void ProfilerLayer::OnEvent()
	{

	}
}