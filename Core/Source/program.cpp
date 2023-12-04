// Local headers
#include <program.h>
#include <iostream>
#include <Profiler.h>

Application::Application()
{
    _instance = this;
    _layers = std::vector<std::shared_ptr<Scape::Layer>>();

    // Create Window and our UI layer
    _window = std::make_shared<Window>("ShaderScape", 1200, 600);
    _imGuiLayer = std::make_unique<Scape::ImGuiLayer>(_window);

    _eventDispatcher = Scape::EventDispatcher::Instance();
}

Application::~Application()
{
    // Layers detach in deconstructor
    //for (auto layer : _layers)
    //    layer->OnDetach();

    //_imGuiLayer->OnDetach();
}

void Application::Run()
{
    while (!_window->ShouldClose())
    {
        Scape::Profiler::Get()->Begin("Total");
        // See how much time the previous frame took
        _currentTime = glfwGetTime();
        _deltaTime = _currentTime - _lastTime;
        _lastTime = _currentTime;

        // Update fixed timesteps depending on how much time has passed since last tick and how many ticks should have passed
        _accumulator += _deltaTime;
        Scape::Profiler::Get()->Begin("FixedUpdate");
        while (_accumulator >= _fixedDeltaTime)
        {
            for (auto &layer : _layers)
                layer->OnFixedUpdate(_fixedDeltaTime);
            _accumulator -= _fixedDeltaTime;
        }
        Scape::Profiler::Get()->End("FixedUpdate");

        //TODO profile sections: Event, Render, Update, Fixed Update

        // Begin Render
        _window->BeginFrame();
        
            // Raw Drawing
            Scape::Profiler::Get()->Begin("Update");
            for (auto &layer : _layers)
                layer->OnUpdate(_currentTime, _deltaTime);
            Scape::Profiler::Get()->End("Update");

            // Gui
            _imGuiLayer->Begin();
                Scape::Profiler::Get()->Begin("UI");
                for (auto &layer : _layers)
                    layer->OnImGuiRender();
                Scape::Profiler::Get()->End("UI");
            _imGuiLayer->End();
        
        // Finish Render
        Scape::Profiler::Get()->Begin("Render");
        _window->EndFrame();
        Scape::Profiler::Get()->End("Render");

        // Handle events
        //TODO event system
        glfwPollEvents();

        Scape::Profiler::Get()->Begin("Event");
        while (_eventDispatcher->EventsInQueue())
        {
            std::shared_ptr<Scape::Event> event = _eventDispatcher->DispatchNextEvent();
            
            
            for (auto &layer : _layers)
                layer->OnEvent(event);
            
        }
        Scape::Profiler::Get()->End("Event");

        Scape::Profiler::Get()->End("Total");
    }
}

void Application::OnEvent(Scape::Event& event)
{
   
}

