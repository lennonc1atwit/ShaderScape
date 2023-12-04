#include <Profiler.h>

namespace Scape {
	void Profiler::Update()
	{
		_frame++;
	}

	void Profiler::Begin(std::string name)
	{
		if (_arrayData.find(name) == _arrayData.end()) {
			_arrayData[name] = std::make_shared<data>(data{ 0, 0, 0 });
			_frameTimeData[name] = (double*)malloc(sizeof(double)*CAPTURE_WINDOW);
			_timeData[name] = (double*)malloc(sizeof(double) * CAPTURE_WINDOW);
		}
		_arrayData[name]->t1 = glfwGetTime();
	}
	void Profiler::End(std::string name)
	{
		data* arrayData = _arrayData[name].get();
		double dt = glfwGetTime() - arrayData->t1;
		int vectorOffset = arrayData->offset;

		_frameTimeData[name][vectorOffset] = dt * 1000.0;
		_timeData[name][vectorOffset] = _frame;

		arrayData->offset = (vectorOffset + 1) % CAPTURE_WINDOW;
	}

	void Profiler::Clear(std::string name)
	{

	}

	std::vector<std::string> Profiler::GetTimings()
	{
		std::vector<std::string> names = std::vector<std::string>();
		for (auto& [name, data] : _timeData) names.push_back(name);
		return names;
	}

	double* Profiler::GetFrameData(std::string name)
	{
		if (_frameTimeData.find(name) == _frameTimeData.end())
			return nullptr;
		return _frameTimeData[name];
	}

	double* Profiler::GetTimeData(std::string name)
	{
		if (_timeData.find(name) == _timeData.end())
			return nullptr;
		return _timeData[name];
	}

	double Profiler::GetDataOffset(std::string name)
	{
		if (_arrayData.find(name) == _arrayData.end())
			return 0;
		return _arrayData[name]->offset;
	}

	void Profiler::CalcAverages()
	{
		for (auto& [name, data] : _frameTimeData)
		{
			_arrayData[name]->avg = std::accumulate(&data[0], &data[CAPTURE_WINDOW - 1], 0.0) / (double)CAPTURE_WINDOW;
		}
	}
}