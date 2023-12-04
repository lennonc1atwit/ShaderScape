#include "ProfilerLayer.h"


namespace Scape {
	void ProfilerLayer::OnAttach()
	{
        _averageFrameTime = 0;

        _avgThickness = 1;
        _avgColor = {0.0, 1.0, 0.5, 1.0};
        
        _lineThickness = 1;
        _lineColor = { 1.0, 0.5, 0.0, 1.0 };

        _fillColor = { 0.0, 0.5, 1.0, 0.125 };

		// Create data buffers
        _profiler = Profiler::Get();
	}

	void ProfilerLayer::OnDetach() { }

    void ProfilerLayer::OnUpdate(double time, double timeStep) { _profiler->Update(); }

	void ProfilerLayer::OnFixedUpdate(double fixedTimeStep){ _profiler->CalcAverages(); }

	void ProfilerLayer::OnImGuiRender()
	{
        if(ImGui::Begin("Profiler"))
        {
            ImVec2 contentSize = ImGui::GetContentRegionAvail();
            if (ImPlot::BeginPlot("##FrameTimes",contentSize))
            {
                double* timeData = _profiler->GetTimeData("Total");
                double* frameData = _profiler->GetFrameData("Total");
                int offset = _profiler->GetDataOffset("Total");

                ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_NoTickLabels);
                ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_SymLog);
                ImPlot::SetupAxisTicks(ImAxis_Y1, yTickValues, 7, yTickLabels, false);

                ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, 200);
                ImPlot::SetupAxisZoomConstraints(ImAxis_Y1, 5, 200);
                ImPlot::SetupAxisLimits(ImAxis_X1, timeData[offset], timeData[(offset - 1) % Profiler::CAPTURE_WINDOW], ImPlotCond_Always);
                
                for(auto section : _profiler->GetTimings()) {
                    

                    /*ImPlot::SetNextLineStyle(_avgColor, _avgThickness);
                    ImPlot::PlotInfLines("Avg", &_averageFrameTime, 1, ImPlotInfLinesFlags_Horizontal);
                    if (_showMarker)
                        ImPlot::TagY(_averageFrameTime, _avgColor, "%.1fms", _averageFrameTime);*/

                    frameData = _profiler->GetFrameData(section);
                    offset = _profiler->GetDataOffset(section);


                    //ImPlot::SetNextLineStyle(_lineColor, _lineThickness);
                    ImPlot::PlotLine(section.c_str(), timeData, frameData, Profiler::CAPTURE_WINDOW, 0, offset);

                    //ImPlot::SetNextFillStyle(_fillColor);
                    //ImPlot::PlotShaded(fill.c_str(), timeData, frameData, Profiler::CAPTURE_WINDOW, 0, 0, offset);

                    /*if (ImPlot::BeginLegendPopup("Avg")) {
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
                    }*/
                }

                ImPlot::EndPlot();
            }
        }
        ImGui::End();
	}

	void ProfilerLayer::OnEvent(std::shared_ptr<Event> event)
	{

	}
}