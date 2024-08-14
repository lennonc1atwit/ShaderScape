#pragma once
#include <memory>
#include <map>
#include <string>
#include <vector>
#include <GLFW/glfw3.h>
#include <numeric>

namespace Scape {
	class Profiler
	{
		struct data
		{
			int offset;
			double t1;
			double avg;
		};
	public:
		static Profiler* Get() 
		{
			if (_instance == nullptr)
				_instance = new Profiler();
			return _instance;
		}

		inline const static int CAPTURE_WINDOW = 240;

		void Update();
		void Begin(std::string name);
		void End(std::string name);
		void Clear(std::string name);

		std::vector<std::string> GetTimings();

		double* GetFrameData(std::string name);
		double* GetTimeData(std::string name);
		double GetDataOffset(std::string name);
		double GetAverageFrameTime(std::string name);

		static int GetFrame() { return _frame; }
		void CalcAverages();
		
	protected:
		Profiler() 
		{
			_frameTimeData = std::map<std::string, double*>();
			_timeData = std::map <std::string, double*>();
			_arrayData = std::map <std::string, std::shared_ptr<data>>();
		}
		~Profiler()
		{
			delete _instance;
		}

		inline static Profiler* _instance = nullptr;
		
		inline static int _frame = 0;

		std::map <std::string, double*> _frameTimeData;
		std::map <std::string, double*> _timeData;
		std::map <std::string, std::shared_ptr<data>> _arrayData;
	};
}