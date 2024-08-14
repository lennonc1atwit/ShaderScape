#include "ProfilerLayer.h"


namespace Scape {
	void ProfilerLayer::OnAttach()
	{
        _profiler = Profiler::Get();

        for (int i = 0; i < Profiler::CAPTURE_WINDOW; i++)
        {
            _timeIndicies[i] = i;
        }
	}

	void ProfilerLayer::OnDetach() { }

    void ProfilerLayer::OnUpdate(double time, double timeStep) { _profiler->Update(); }

	void ProfilerLayer::OnFixedUpdate(double fixedTimeStep){ _profiler->CalcAverages(); }

    void ProfilerLayer::OnEvent(std::shared_ptr<Event> event) { }

	void ProfilerLayer::OnImGuiRender()
	{
        if(ImGui::Begin("Profiler"))
        { 
            if (ImPlot::BeginPlot("##FrameTimes", ImGui::GetContentRegionAvail(), ImPlotFlags_NoTitle))
            {
                ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_NoTickLabels);
                ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_SymLog);
                ImPlot::SetupAxisTicks(ImAxis_Y1, _yTickValues, 9, _yTickLabels, false);
                ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, 200);
                ImPlot::SetupAxisZoomConstraints(ImAxis_Y1, 1, 200);
                //ImPlot::SetupAxisLimits(ImAxis_X1, _profiler->GetFrame() - Profiler::CAPTURE_WINDOW, _profiler->GetFrame(), ImPlotCond_Always);
                ImPlot::SetupLegend(ImPlotLocation_NorthWest, ImPlotLegendFlags_Horizontal);

                for(auto section : _profiler->GetTimings()) { 
                    if (_plotData.find(section) == _plotData.end())
                        _plotData[section] = std::make_shared<LineData>(LineData{ GetNextColor(), 1.0f, false, "Thickness##" + section, "Color##" + section, "Show Avg##" + section});
                    std::shared_ptr<LineData> plotData = _plotData[section];        

                    ImPlot::SetNextLineStyle(plotData->Color, plotData->Thickness);
                    ImPlot::PlotLine(section.c_str(), _timeIndicies, _profiler->GetFrameData(section), Profiler::CAPTURE_WINDOW, ImPlotLineFlags_None, 0);

                    if (plotData->ShowAverage) {
                        double avg = _profiler->GetAverageFrameTime(section);
                        ImPlot::TagY(avg, plotData->Color, "%.1fms", avg);
                    }

                    if (ImPlot::BeginLegendPopup(section.c_str())) {
                        ImGui::SliderFloat(plotData->ThicknessLabel.c_str(), &plotData->Thickness, 0, 10);
                        ImGui::ColorEdit4(plotData->ColorLabel.c_str(), &plotData->Color.x);
                        ImGui::Checkbox(plotData->AverageLabel.c_str(), &plotData->ShowAverage);
                        ImPlot::EndLegendPopup();
                    }
                }

                ImPlot::EndPlot();
            }
        }
        ImGui::End();
	}

	
}