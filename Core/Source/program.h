#pragma once
#include <Window.h>
#include <ImGuiLayer.h>
#include <cstdarg>
#include <Event.h>
#include <vector>

class Application
{
public:
    Application();
    ~Application();

    void Run();
    void OnEvent(Scape::Event& event);

    static Application& Get() { return *_instance; }

    std::shared_ptr<Window> GetWindow() { return _window; }

    template<class T>
    void InstanceLayer() { _layers.push_back(std::make_shared<T>()); }

private:
    std::vector<std::shared_ptr<Scape::Layer>> _layers;
    Scape::EventDispatcher* _eventDispatcher;

    std::shared_ptr<Window> _window;
    std::unique_ptr<Scape::ImGuiLayer> _imGuiLayer;

    inline static Application* _instance = nullptr;

    unsigned int _ticksPerSecond = 1;

    double _currentTime = 0;
    double _lastTime = 0;
    double _deltaTime = 0;
    double _fixedDeltaTime = 1.0 / _ticksPerSecond;
    double _accumulator = 0;
};